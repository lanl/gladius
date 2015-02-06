/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-fe.h"
#include "core/utils.h"
#include "tool-be/tool-be.h"

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * To fully understand the entire set of interactions here, you'll also need to
 * see the back-end versions of the tool, LaunchMon, and MRNet.
 */

using namespace gladius;
using namespace gladius::toolfe;

namespace {
// This component's name.
const std::string CNAME = "tool-fe";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::GREEN);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)

////////////////////////////////////////////////////////////////////////////////
// Place for ToolFE-specific environment variables.
////////////////////////////////////////////////////////////////////////////////
#define ENV_VAR_CONNECT_TIMEOUT_IN_SEC "GLADIUS_TOOL_FE_CONNECT_TIMEOUT_S"
#define ENV_VAR_CONNECT_MAX_RETRIES "GLADIUS_TOOL_FE_CONNECT_MAX_RETRIES"
namespace {
static const std::vector<core::EnvironmentVar> compEnvVars = {
    {ENV_VAR_CONNECT_TIMEOUT_IN_SEC, "Connection timeout in seconds."},
    {ENV_VAR_CONNECT_MAX_RETRIES, "Maximum number of connection retries."}
};
}

/**
 *
 */
void
echoLaunchStart(const gladius::core::Args &args)
{
    std::string lstr;
    for (decltype(args.argc()) i = 0; i < args.argc(); ++i) {
        lstr += args.argv()[i];
        lstr += " ";
    }
    GLADIUS_COUT_STAT << "Launch Sequence Initiated..." << std::endl;
    GLADIUS_COUT_STAT << "Starting: " << lstr << std::endl;
}

} // end namespace

const toolcommon::timeout_t ToolFE::sDefaultTimeout = 30;
const toolcommon::retry_t ToolFE::sDefaultMaxRetries = 8;

/**
 * Component registration.
 */
void
registerComponent(void)
{
    // Register this component's environment variables with the central
    // registry.
    core::Environment::TheEnvironment().addToRegistry(
        "Tool Front-end",
        compEnvVars
    );
}

/**
 *
 */
void
ToolFE::mGetStateFromEnvs(void)
{
    if (core::utils::envVarSet(GLADIUS_TOOL_FE_VERBOSE_STR)) {
        mBeVerbose = true;
    }
    else {
        mBeVerbose = false;
    }
    //
    auto rc = core::utils::getEnvAs(
                  ENV_VAR_CONNECT_MAX_RETRIES,
                  mMaxRetries
              );
    // Not set, so default to some number of retries.
    if (GLADIUS_ENV_NOT_SET == rc) {
        mMaxRetries = sDefaultMaxRetries;
    }
    else if (GLADIUS_SUCCESS != rc) {
        GLADIUS_THROW_CALL_FAILED_RC("core::utils::getEnvAs", rc);
    }
    if (mMaxRetries <= 0) mMaxRetries = toolcommon::unlimitedRetries;
    //
    rc = core::utils::getEnvAs(
             ENV_VAR_CONNECT_TIMEOUT_IN_SEC,
             mConnectionTimeoutInSec
         );
    // Not set, so default to some timeout.
    if (GLADIUS_ENV_NOT_SET == rc) {
        mConnectionTimeoutInSec = sDefaultTimeout;
    }
    else if (GLADIUS_SUCCESS != rc) {
        GLADIUS_THROW_CALL_FAILED_RC("core::utils::getEnvAs", rc);
    }
    if (mConnectionTimeoutInSec <= 0) {
        mConnectionTimeoutInSec = toolcommon::unlimitedTimeout;
    }
}

/**
 * Tool front-end constructor.
 */
ToolFE::ToolFE(
    void
) : mBeVerbose(false)
  , mConnectionTimeoutInSec(toolcommon::unlimitedTimeout)
  , mMaxRetries(toolcommon::unlimitedRetries)
{
    mGetStateFromEnvs();
}

/**
 * Returns whether or not the tool-fe's environment setup is sane.
 */
bool
ToolFE::envSane(std::string &whatsWrong)
{
    GLADIUS_UNUSED(whatsWrong);
    bool sane = true;
    return sane;
}

/**
 * Responsible for running the tool front-end instance. This is the tool-fe
 * entry point from a caller's perspective.
 */
void
ToolFE::mainLoop(
    const core::Args &args
) {
    VCOMP_COUT("Entering Main Loop." << std::endl);
    try {
        mAppArgs = args;
        // Make sure that all the required bits are
        // set before we get to launching anything.
        std::string whatsWrong;
        if (!envSane(whatsWrong)) {
            GLADIUS_CERR << whatsWrong << std::endl;
            return;
        }
        // If we are here, then our environment is sane enough to start...
        // FIXME dup stdout?
        mInitializeToolInfrastructure();
        // Start lash-up thread.
        mStartToolLashUpThread();
    }
    // If something went south, just print the haps and return to the top-level
    // REPL. Insulate the caller by catching things and handling them here.
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
    }
}

/**
 *
 */
void
ToolFE::mInitializeToolInfrastructure(void)
{
    try {
        // First init LaunchMON
        mLMONFE.init(mBeVerbose);
        // Register function that is responsible for packing data for front-end
        // to back-end transfers. The MRNetFE knows how to do this.
        mLMONFE.regPackForFeToBe(mrnetfe::MRNetFE::getFEToBePackFun());
        // Then do the same for MRNet
        mMRNFE.init(mBeVerbose);
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
ToolFE::mStartToolLashUpThread(void)
{
    std::unique_lock<std::mutex> lock(mtLashUpLock);
    std::thread luThread(&ToolFE::mInitiateToolLashUp, this);
    mtLashUpComplete.wait(lock);
    luThread.join();
    if (GLADIUS_SUCCESS != maStatus) {
        mLMONFE.shutdown();
        GLADIUS_THROW_CALL_FAILED_RC("mStartToolLashUpThread", maStatus);
    }
}

/**
 *
 */
void
ToolFE::mConnectMRNetTree(void)
{
    // TODO add a timer here
    decltype(mMaxRetries) attempt = 1;
    bool connectSuccess = false;
    do {
        VCOMP_COUT("Connection Attempt: " << attempt << std::endl);
        // Take a break and let things happen...
        sleep(1);
        // First make sure that the daemons are okay.
        if (!WIFBESPAWNED(mLMONFE.getState())) {
            GLADIUS_THROW("The Tool Daemons Have Exited.");
        }
        // Try to connect.
        auto status = mMRNFE.connect();
        // All done - Get outta here...
        if (GLADIUS_SUCCESS == status) {
            connectSuccess = true;
            break;
        }
        // Something bad happened.
        else if (GLADIUS_NOT_CONNECTED != status) {
            GLADIUS_THROW_CALL_FAILED_RC("MRNetFE::connect", status);
        }
        // Unlimited retries, so just continue.
        if (toolcommon::unlimitedRetries == mMaxRetries) continue;
        if (attempt++ >= mMaxRetries) {
            GLADIUS_THROW("Max Retries Reached! Giving Up...");
        }
    } while (true);
    //
    if (connectSuccess) {
        GLADIUS_COUT_STAT << "MRNet Network Connected." << std::endl;
    }
    else {
        GLADIUS_THROW("Could Not Setup MRNet Network.");
    }
}

/**
 * The thread that initiates the tool lash-up.  This is NOT the main thread, so
 * this is why we don't throw in the exceptional case.
 */
void
ToolFE::mInitiateToolLashUp(void)
{
    try {
        maStatus = GLADIUS_SUCCESS;
        echoLaunchStart(mAppArgs);
        // And so it begins...
        mLMONFE.launchAndSpawnDaemons(mAppArgs);
        // Make sure that the tool daemons launched.
        if (!mLMONFE.daemonsLaunched()) {
            GLADIUS_THROW("Tool Daemons Not Launched.");
        }
        // Create MRNet network FE.
        mMRNFE.createNetworkFE(mLMONFE.getProcTab());
        // Send info to daemons.
        mLMONFE.sendDaemonInfo(mMRNFE.getLeafInfo());
        // Wait for MRNet tree connections.
        mConnectMRNetTree();
    }
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
        maStatus = GLADIUS_ERR_LMON;
    }
    // Notify main thread unconditionally.
    mtLashUpComplete.notify_one();
}
