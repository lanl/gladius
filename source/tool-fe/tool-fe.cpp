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
echoLaunchStart(
    const gladius::core::Args &largs,
    const gladius::core::Args &aargs
)
{
    using namespace std;
    // Construct the entire launch command.
    auto argv = largs.toArgv();
    auto aArgv = aargs.toArgv();
    argv.insert(end(argv), begin(aArgv), end(aArgv));
    // To a single string
    string lstr;
    for (const auto & arg : argv) {
        lstr += (arg + " ");
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
int
ToolFE::mGetStateFromEnvs(void)
{
    if (core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME)) {
        mBeVerbose = true;
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
        GLADIUS_CERR << "Error encountered while trying to "
                        "read environment variable: "
                     << ENV_VAR_CONNECT_MAX_RETRIES << std::endl;
        return rc;
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
        GLADIUS_CERR << "Error encountered while trying to "
                        "read environment variable: "
                     << ENV_VAR_CONNECT_TIMEOUT_IN_SEC << std::endl;
        return rc;
    }
    if (mConnectionTimeoutInSec <= 0) {
        mConnectionTimeoutInSec = toolcommon::unlimitedTimeout;
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 * Tool front-end constructor.
 */
ToolFE::ToolFE(
    void
) : mBeVerbose(false)
  , mConnectionTimeoutInSec(toolcommon::unlimitedTimeout)
  , mMaxRetries(toolcommon::unlimitedRetries)
  , mPathToPluginPack("") { ; }

/**
 * Returns whether or not the tool-fe's environment setup is sane.
 */
int
ToolFE::mSetupCore(void)
{
    std::string whatsWrong;
    static const auto envMode = GLADIUS_ENV_DOMAIN_MODE_NAME;
    int rc = GLADIUS_SUCCESS;
    //
    if (GLADIUS_SUCCESS != (rc = mGetStateFromEnvs())) {
        return rc;
    }
    //
    if (!core::utils::envVarSet(envMode)) {
        whatsWrong = "Cannot determine current mode.\nPlease set '"
                   + std::string(envMode) +  "' and try again.";
        GLADIUS_CERR << whatsWrong << std::endl;
        return GLADIUS_ERR;
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
        GLADIUS_CERR << whatsWrong << std::endl;
        return GLADIUS_ERR;
    }
    // Set member, so we can get the plugin pack later...
    mPathToPluginPack = pathToPluginPackIfAvail;
    //
    return mInitializeParallelLauncher();
}

/**
 *
 */
int
ToolFE::mPreToolInitActons(void)
{
#if 0
    using namespace std;
    // Dup here before we start tool infrastructure lash-up. Someone in
    // there makes stdio act funny. This is a workaround to fix that.
    mStdInCopy = dup(STDIN_FILENO);
    if (-1 == mStdInCopy) {
        int err = errno;
        GLADIUS_CERR << "dup(2): " + core::utils::getStrError(err) << endl;
        return GLADIUS_ERR;
    }
    if (-1 == close(STDIN_FILENO)) {
        int err = errno;
        GLADIUS_CERR << "close(2): " + core::utils::getStrError(err) << endl;
        return GLADIUS_ERR;
    }
#endif
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
ToolFE::mPostToolInitActons(void)
{
#if 0
    // Restore stdin. This is the counterpart to the workaround in
    // mPreToolInitActons.
    if (-1 == dup2(mStdInCopy, STDIN_FILENO)) {
        int err = errno;
        GLADIUS_CERR << "Call to dup2(2) failed: "
                     << core::utils::getStrError(err) << std::endl;
        return GLADIUS_ERR;
    }
    close(mStdInCopy);
#endif
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
ToolFE::mInitializeParallelLauncher(void)
{
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mCommandr.init(mLauncherArgs))) {
        return rc;
    }
    VCOMP_COUT(
        "Application launcher personality: " <<
        mCommandr.getPersonalityName() << std::endl
    );
    VCOMP_COUT("Which launcher: " << mCommandr.which() << std::endl);
    return rc;
}

/**
 * Responsible for running the tool front-end instance. This is the tool-fe
 * entry point from a caller's perspective.
 */
int
ToolFE::main(
    const core::Args &appArgv,
    const core::Args &launcherArgv
) {
    VCOMP_COUT("Entering main." << std::endl);
    int rc = GLADIUS_SUCCESS;
    //
    try {
        mAppArgs = appArgv;
        mLauncherArgs = launcherArgv;
        // Make sure that all the required bits are set before we get to
        // launching anything.
        if (GLADIUS_SUCCESS != (rc = mSetupCore())) {
            return rc;
        }
        // Perform any actions that need to take place before lash-up.
        if (GLADIUS_SUCCESS != (rc = mPreToolInitActons())) {
            return rc;
        }
        //
        if (GLADIUS_SUCCESS != (rc = mDetermineProcLandscape())) {
            return rc;
        }
        //
        if (GLADIUS_SUCCESS != (rc = mBuildNetwork())) {
            return rc;
        }
        ////////////////////////////////////////////////////////////////////////
        // If we are here, then our environment is sane enough to start...
        ////////////////////////////////////////////////////////////////////////
        // Start lash-up.
        if (GLADIUS_SUCCESS != (rc = mInitiateToolLashUp())) {
            return rc;
        }
        //
        if (GLADIUS_SUCCESS != (rc = mPostToolInitActons())) {
            return rc;
        }
        // Now that the base infrastructure is up, now load the user-specified
        // plugin pack.
        mLoadPlugins();
        // Let the BEs know what plugins they are loading.
        mSendPluginInfoToBEs();
        // Now turn it over to the plugin.
        mEnterPluginMain();
    }
    catch (const std::exception &e) {
        GLADIUS_THROW(e.what());
    }
    // TODO - gracefully shutdown things. We may need to tear down a bunch of
    // infrastructure.
    return rc;
}

/**
 *
 */
int
ToolFE::mDetermineProcLandscape(void)
{
    VCOMP_COUT("Determining process landscape..." << std::endl);
    int rc = GLADIUS_SUCCESS;
    try {
        if (GLADIUS_SUCCESS != (rc = mDSI.init(mCommandr, mBeVerbose))) {
            return rc;
        }
        if (GLADIUS_SUCCESS != (rc = mDSI.getProcessLandscape(mProcLandscape))) {
            return rc;
        }
        GLADIUS_COUT_STAT << "Job Statistics:" << std::endl;
        GLADIUS_COUT_STAT << "o Number of Application Processes: "
                          << mProcLandscape.nProcesses() << std::endl;
        GLADIUS_COUT_STAT << "o Number of Hosts: "
                          << mProcLandscape.nHosts() << std::endl;
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    VCOMP_COUT("Done determining process landscape..." << std::endl);
    return rc;
}

/**
 *
 */
int
ToolFE::mBuildNetwork(void)
{
    int rc = GLADIUS_SUCCESS;
    try {
        if (GLADIUS_SUCCESS != (rc = mMRNFE.init(mBeVerbose))) {
            return rc;
        }
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    return rc;
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
int
ToolFE::mInitiateToolLashUp(void)
{
    try {
        echoLaunchStart(mLauncherArgs, mAppArgs);
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
    //
    return GLADIUS_SUCCESS;
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
