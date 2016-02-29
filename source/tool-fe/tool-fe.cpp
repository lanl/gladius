/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * To fully understand the entire set of interactions here, you'll also need to
 * see the back-end versions of the tool, LaunchMon, and MRNet.
 */

#include "tool-fe.h"

#include "core/utils.h"
#include "core/env.h"
#include "app-launcher/app-launcher.h"
#include "tool-be/tool-be.h"

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius;
using namespace gladius::toolfe;

namespace {
// This component's name.
const std::string CNAME = "tool-fe";
//
const auto COMPC = core::colors::GREEN;
// CNAME's color code.
const std::string NAMEC = core::colors::color().ansiBeginColor(COMPC);
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
//
#define ENV_VAR_CONNECT_TIMEOUT_IN_SEC "GLADIUS_TOOL_FE_CONNECT_TIMEOUT_S"
//
#define ENV_VAR_CONNECT_MAX_RETRIES "GLADIUS_TOOL_FE_CONNECT_MAX_RETRIES"

namespace {
static const std::vector<core::EnvironmentVar> compEnvVars = {
    {ENV_VAR_CONNECT_TIMEOUT_IN_SEC,
     "Connection timeout in seconds. Default: " +
     std::to_string(ToolFE::sDefaultTimeout) + "."
    },
    {ENV_VAR_CONNECT_MAX_RETRIES,
     "Maximum number of connection retries. Default: " +
     std::to_string(ToolFE::sDefaultMaxRetries) + "."
    }
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Component registration.
 */
void
ToolFE::registerComponent(void)
{
    // Register this component's environment variables with the central
    // registry.
    core::Environment::TheEnvironment().addToRegistry(
        CNAME,
        compEnvVars
    );
}

/**
 *
 */
void
ToolFE::mGetStateFromEnvs(void)
{
    if (core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME)) {
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
  , mPathToPluginPack("")
{
    mGetStateFromEnvs();
}

/**
 * Returns whether or not the tool-fe's environment setup is sane.
 */
bool
ToolFE::mBaseCoreUsable(std::string &whatsWrong)
{
    static const auto envMode = GLADIUS_ENV_DOMAIN_MODE_NAME;
    //
    if (!core::utils::envVarSet(envMode)) {
        whatsWrong = "Cannot determine current mode.\nPlease set '"
                   + std::string(envMode) +  "' and try again.";
        return false;
    }
    auto modeName = core::utils::getEnv(envMode);
    // Initialize the DSP manager.
    mDSPManager = dspa::DSPManager(modeName, mBeVerbose);
    // The path to the plugin pack if we find a usable one.
    std::string pathToPluginPackIfAvail;
    if (!mDSPManager.pluginPackAvailable(pathToPluginPackIfAvail)) {
        // TODO Make better. Provide an example.
        whatsWrong = "Cannot find a usable plugin pack for '"
                   + modeName + "'.\nPlease make sure that the directory "
                     "where this plugin pack lives is in "
                     GLADIUS_ENV_PLUGIN_PATH_NAME " and all required plugins "
                     "are installed." ;
        return false;
    }
    // Set member, so we can get the plugin pack later...
    mPathToPluginPack = pathToPluginPackIfAvail;

    return true;
}

/**
 *
 */
void
ToolFE::mPreToolInitActons(void)
{
    // Dup here before we start tool infrastructure lash-up. Someone in
    // there makes stdio act funny. This is a workaround to fix that.
    mStdInCopy = dup(STDIN_FILENO);
    if (-1 == mStdInCopy) {
        int err = errno;
        GLADIUS_THROW_CALL_FAILED(
            "dup(2): " + core::utils::getStrError(err)
        );
    }
    if (-1 == close(STDIN_FILENO)) {
        int err = errno;
        GLADIUS_THROW_CALL_FAILED(
            "close(2): " + core::utils::getStrError(err)
        );
    }
}

/**
 *
 */
void
ToolFE::mPostToolInitActons(void)
{
    // Restore stdin. This is the counterpart to the workaround in
    // mPreToolInitActons.
    if (-1 == dup2(mStdInCopy, STDIN_FILENO)) {
        int err = errno;
        GLADIUS_THROW_CALL_FAILED(
            "dup2(2): " + core::utils::getStrError(err)
        );
    }
    close(mStdInCopy);
}

/**
 *
 */
void
ToolFE::mInitializeParallelLauncher(
    void
) {
    // The first argument should be the launcher name.
    mAppLauncherName = std::string(mAppArgs.argv()[0]);
    mAppLauncher.init(mAppLauncherName);
    VCOMP_COUT(
        "Application launcher personality: " <<
        mAppLauncher.getPersonalityName() << std::endl
    );
    VCOMP_COUT("Which launcher: " << mAppLauncher.which() << std::endl);
}

/**
 * Responsible for running the tool front-end instance. This is the tool-fe
 * entry point from a caller's perspective.
 */
void
ToolFE::main(
    const core::Args &args
) {
    VCOMP_COUT("Entering main." << std::endl);
    //
    try {
        mAppArgs = args;
        // Make sure that all the required bits are set before we get to
        // launching anything.
        std::string whatsWrong;
        if (!mBaseCoreUsable(whatsWrong)) {
            GLADIUS_CERR << std::endl << whatsWrong << std::endl;
            return;
        }
        ////////////////////////////////////////////////////////////////////////
        // If we are here, then our environment is sane enough to start...
        ////////////////////////////////////////////////////////////////////////
        // Perform any actions that need to take place before lash-up.
        mPreToolInitActons();
        //
        mInitializeToolInfrastructure();
        // Start lash-up.
        mInitiateToolLashUp();
        //
        mPostToolInitActons();
        // Now that the base infrastructure is up, now load the user-specified
        // plugin pack.
        mLoadPlugins();
        // Let the BEs know what plugins they are loading.
        mSendPluginInfoToBEs();
        // Now turn it over to the plugin.
        mEnterPluginMain();
    }
    // If something went south, just print the haps and return to the top-level
    // REPL. Insulate the caller by catching things and handling them here.
    catch (const std::exception &e) {
        // TODO - gracefully shutdown things. We may need to tear down a bunch
        // of infrastructure.
        GLADIUS_CERR << e.what() << std::endl;
    }
    // TODO return code to caller
}

/**
 *
 */
void
ToolFE::mInitializeToolInfrastructure(void)
{
    try {
        mInitializeParallelLauncher();
        mDSI.init(mAppLauncher, mBeVerbose);
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
ToolFE::mConnectMRNetTree(void)
{
    // TODO add a timer here
    decltype(mMaxRetries) attempt = 1;
    bool connectSuccess = false;
    do {
        VCOMP_COUT("Connection attempt: " << attempt << std::endl);
        // Take a break and let things happen...
        sleep(1);
#if 0 // TODO
        // First make sure that the daemons are okay.
        if (!WIFBESPAWNED(mLMONFE.getState())) {
            GLADIUS_THROW("The Tool Daemons Have Exited.");
        }
#endif
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
 * Let LaunchMON know what environment variables we would like to forward to the
 * remote environments. When adding a new environment variable, please also
 * update the code in gladius-toold.cpp.
 */
void
ToolFE::mForwardEnvsToBEsIfSetOnFE(void)
{
    using namespace std;
    // Environment variable forwarding to daemons. Note that this is a complete
    // list of environment variables that we would forward if they are set
    // within the tool front-end's environment.
    static const vector<string> envVars = {
        GLADIUS_ENV_TOOL_BE_LOG_DIR_NAME,
        GLADIUS_ENV_TOOL_BE_VERBOSE_NAME
    };
    //
    vector <pair<string, string> > envTups;
    // If the environment variable is set, then capture its value.
    for (auto &envVar : envVars) {
        // Filter out those that are not set.
        if (core::utils::envVarSet(envVar)) {
            envTups.push_back(make_pair(envVar, core::utils::getEnv(envVar)));
        }
    }
#if 0 // TODO
    mLMONFE.forwardEnvsToBEs(envTups);
#endif
}

/**
 * Initiates the tool lash-up bits.
 */
void
ToolFE::mInitiateToolLashUp(void)
{
    try {
        echoLaunchStart(mAppArgs);
        // TODO
        // Setup environment variable forwarding to the remote environments.
        //mForwardEnvsToBEsIfSetOnFE();
#if 0 // TODO
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
#endif
        // Wait for MRNet tree connections.
        mConnectMRNetTree();
        // Setup connected MRNet network.
        mMRNFE.networkInit();
        // Make sure that our core filters are working by performing a handshake
        // between the tool front-end and all the tool leaves (where all
        // communication is going through a set of core filters).
        mMRNFE.handshake();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
ToolFE::mLoadPlugins(void)
{
    using namespace std;

    VCOMP_COUT("Loading plugins." << std::endl);
    // Get the front-end plugin pack.
    mPluginPack = mDSPManager.getPluginPackFrom(
                      dspa::DSPluginPack::PluginFE,
                      mPathToPluginPack
                  );
    auto *fePluginInfo = mPluginPack.pluginInfo;
    GLADIUS_COUT_STAT << "Front-End Plugin Info:" << endl;
    GLADIUS_COUT_STAT << "*Name      : " << fePluginInfo->pluginName << endl;
    GLADIUS_COUT_STAT << "*Version   : " << fePluginInfo->pluginVersion << endl;
    GLADIUS_COUT_STAT << "*Plugin ABI: " << fePluginInfo->pluginABI << endl;
    mFEPlugin = fePluginInfo->pluginConstruct();

    VCOMP_COUT("Done loading plugins." << std::endl);
}

/**
 *
 */
void
ToolFE::mSendPluginInfoToBEs(void)
{
    VCOMP_COUT("Sending plugin info to back-ends." << std::endl);
    // MRNet knows how to do this...
    mMRNFE.pluginInfoBCast(
        std::string(mPluginPack.pluginInfo->pluginName),
        mPathToPluginPack
    );
}

/**
 *
 */
void
ToolFE::mEnterPluginMain(void)
{
    VCOMP_COUT("Entering plugin main." << std::endl);

    try {
        // TODO FIXME
        toolcommon::ProcessTable fake;
        dspi::DSPluginArgs pluginArgs(
            mPathToPluginPack,
            mAppArgs,
#if 0 // TODO
            mLMONFE.getProcTab(),
#else
            fake,
#endif
            mMRNFE.getProtoStream(),
            mMRNFE.getNetwork()
        );
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Front-end Plugin Entry Point.
        mFEPlugin->pluginMain(pluginArgs);
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }

    VCOMP_COUT("Exited plugin main." << std::endl);
}
