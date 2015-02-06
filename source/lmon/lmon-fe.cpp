/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements LaunchMON actions shim.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lmon/lmon-fe.h"
#include "core/core.h"
#include "tool-common/tool-common.h"

#include <iomanip>

using namespace gladius;
using namespace gladius::lmonfe;

namespace {
// This component's name.
const std::string CNAME = "lmon-fe";
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
/// What to use for remote login
const std::string REMOTE_LOGIN = "/usr/bin/ssh";

////////////////////////////////////////////////////////////////////////////////
namespace LaunchMonFEGlobals {
// Global variable that indicates whether or not this component will be verbose.
bool beVerbose = false;
// Global variable holding latest LaunchMON state (set by statusFuncCallback).
int lmonState = 0;
}

/**
 * LaunchMON status callback.
 */
int
statusFuncCallback(
    int *status
) {
    using namespace std;

    if (!status) {
        GLADIUS_THROW_INVLD_ARG();
    }
    // Set global state.
    LaunchMonFEGlobals::lmonState = *status;
    if (LaunchMonFEGlobals::beVerbose) {
        //
        if (WIFREGISTERED(LaunchMonFEGlobals::lmonState)) {
            COMP_COUT << "* Session Registered" << endl;
        }
        else {
            COMP_COUT << "* Session Not Registered" << endl;
        }
        if (WIFBESPAWNED(LaunchMonFEGlobals::lmonState)) {
            COMP_COUT << "* Back-End Daemons Spawned" << endl;
        }
        else {
            COMP_COUT << "* Back-End Daemons Not Spawned or Exited" << endl;
        }
        if (WIFMWSPAWNED(LaunchMonFEGlobals::lmonState)) {
            COMP_COUT << "* MW Daemons Spawned" << endl;
        }
        else {
            COMP_COUT << "* MW Daemons Not Spawned or Exited" << endl;
        }
        if (WIFDETACHED(LaunchMonFEGlobals::lmonState)) {
            COMP_COUT << "* The Job is Detached" << endl;
        }
        else {
            if (WIFKILLED(LaunchMonFEGlobals::lmonState)) {
                COMP_COUT << "* The Job is Killed" << endl;
            }
            else {
                COMP_COUT << "* The Job Has Not Been Killed" << endl;
            }
        }
    }
    return 0;
}

} // end nameless namespace

const std::string LaunchMonFE::sLaunchMONName = "launchmon";
const std::string LaunchMonFE::sToolDName = "gladius-toold";

/**
 * Constructor.
 */
LaunchMonFE::LaunchMonFE(
    void
) : mBeVerbose(false)
  , mHostname(core::utils::getHostname())
  , mToolD("")
  , mPrefixPath("")
  , mEnginePath("")
  , mRemoteLogin(REMOTE_LOGIN
) {
    LaunchMonFEGlobals::lmonState = 0;
    LaunchMonFEGlobals::beVerbose = mBeVerbose;
}

/**
 * Destructor.
 */
LaunchMonFE::~LaunchMonFE(void) { ; }

/**
 * Sets some environment variables that impact the behavior of LaunchMON.
 */
void
LaunchMonFE::mSetEnvs(void)
{
    core::utils::setEnv("LMON_PREFIX", mPrefixPath);
    core::utils::setEnv("LMON_LAUNCHMON_ENGINE_PATH", mEnginePath);
    core::utils::setEnv("LMON_REMOTE_LOGIN", mRemoteLogin);
}

/**
 * Creates and populates the process table.
 */
void
LaunchMonFE::mCreateAndPopulateProcTab(void)
{
    try {
        unsigned int numProcTabEntries = 0;
        auto rc = LMON_fe_getProctableSize(
                      mSessionNum,
                      &numProcTabEntries
                  );
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_getProctableSize", rc);
        }
        // Construct the process table and allocate room for the entries now
        // that we know how many entries we need to accommodate.
        mProcTab = toolcommon::ProcessTable(numProcTabEntries);
        // Now populate the thing...
        unsigned int pSize = 0;
        rc = LMON_fe_getProctable(
                 mSessionNum,
                 mProcTab.procTab(),
                 &pSize,
                 numProcTabEntries // Max Length
             );
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_getProctable", rc);
        }
        if (mBeVerbose) {
            COMP_COUT << "Done Getting Process Table" << std::endl;
            mProcTab.dumpTo(std::cout);
        }
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
bool
LaunchMonFE::mDetermineAndSetPaths(
    std::string &whatsWrong
) {
    whatsWrong = "";
    VCOMP_COUT("Determining and Setting Paths." << std::endl);
    // Make sure that our tool daemon is in our PATH.
    auto status = core::utils::which(sToolDName, mToolD);
    if (GLADIUS_SUCCESS != status) {
        whatsWrong = toolcommon::utils::genNotInPathErrString(sToolDName);
        return false;
    }
    // Do the same for launchmon
    status = core::utils::which(sLaunchMONName, mEnginePath);
    if (GLADIUS_SUCCESS != status) {
        whatsWrong = toolcommon::utils::genNotInPathErrString(sLaunchMONName);
        return false;
    }
    // While we're at it, set LaunchMON's prefix.
    mPrefixPath = mGetLmonPrefixFromEnginePath(mEnginePath);
    return true;
}

/**
 *
 */
std::string
LaunchMonFE::mGetLmonPrefixFromEnginePath(
    const std::string &whichString
) {
    std::string badness = "Could not determine LaunchMON's installation "
                          "prefix by inspecting the following path:"
                          "'" + whichString + "'";
    std::string prefix = whichString;
    std::string last = "/bin/" + sLaunchMONName;
    auto found = prefix.rfind(last);
    // Not found, so something is wrong.
    if (std::string::npos == found) {
        GLADIUS_THROW(badness);
    }
    prefix = prefix.substr(0, found);
    return prefix;
}

/**
 *
 */
void
LaunchMonFE::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    LaunchMonFEGlobals::beVerbose = mBeVerbose;
    VCOMP_COUT("Initializing LaunchMon Front-End." << std::endl);
    std::string whatsWrong;
    if (!mDetermineAndSetPaths(whatsWrong)) {
        GLADIUS_THROW(whatsWrong);
    }
    mSetEnvs();
    // Init LaunchMON
    auto rc = LMON_fe_init(LMON_VERSION);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED("LMON_fe_init");
    }
}

/**
 *
 */
int
LaunchMonFE::getState(void)
{
    return LaunchMonFEGlobals::lmonState;
}

/**
 * Starts a new session.
 */
void
LaunchMonFE::mStartSession(void)
{
    auto rc = LMON_fe_createSession(&mSessionNum);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED("LMON_fe_createSession");
    }
    //
    rc = LMON_fe_regPackForFeToBe(mSessionNum, mFEToBePackFn);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_getRMInfo", rc);
    }
    // Register LMON status callbacks.
    rc = LMON_fe_regStatusCB(mSessionNum, statusFuncCallback);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_regStatusCB", rc);
    }
}

/**
 * Ends an existing session and cleans up session things.
 */
void
LaunchMonFE::mEndSession(void)
{
    // TODO Finish
    if (mIsLaunched && -1 != mSessionNum) {
        auto rc = LMON_fe_detach(mSessionNum);
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_detach", rc);
        }
    }
}

/**
 *
 */
void
LaunchMonFE::mSetRMInfo(void)
{
    auto rc = LMON_fe_getRMInfo(mSessionNum, &mRMInfo);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_getRMInfo", rc);
    }
    mLauncherPID = mRMInfo.rm_launcher_pid;
}

/**
 *
 */
void
LaunchMonFE::regPackForFeToBe(
    toolcommon::FEToBePackFnP packFeBeFn
) {
    mFEToBePackFn = packFeBeFn;
}

/**
 *
 */
void
LaunchMonFE::launchAndSpawnDaemons(
    const core::Args &appArgs
) {
    using namespace std;
    try {
        char **launcherArgv = appArgs.argv();
        string launcherPath = launcherArgv[0];
        // If we weren't provided an absolute path to the launcher, then fix
        // that.
        if (!core::utils::isAbsolutePath(launcherPath)) {
            auto status = core::utils::which(launcherArgv[0], launcherPath);
            if (GLADIUS_SUCCESS != status) {
                GLADIUS_CERR << "Could not find '" << launcherArgv[0]
                             << "' in $PATH.\nProviding an absolute "
                             << "path to the launcher may fix this."
                             << std::endl;
                return;
            }
        }
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // New sesh...
        mStartSession();
        GLADIUS_COUT_STAT << "Launching and Spawning Daemons." << std::endl;
        auto rc = LMON_fe_launchAndSpawnDaemons(
                      mSessionNum,
                      mHostname.c_str(),
                      launcherPath.c_str(), // launcher absolute path
                      launcherArgv,         // all of the launch command
                      mToolD.c_str(),       // tool daemon's absolute path
                      NULL,
                      NULL,
                      NULL
                  );
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_launchAndSpawnDaemons", rc);
        }
        // Update state, so we know that things have been launched.
        mIsLaunched = true;
        // Get and set RM things
        mSetRMInfo();
        // Create and populate process table.
        mCreateAndPopulateProcTab();
        // Populate target hosts. Note: this is a set of host names, so the size
        // of this set is equal to the number of servers (nodes) that are being
        // used in this job. This "should" be equal to the number of daemons
        // that were also spawned.
        auto hosts = toolcommon::Hosts(mProcTab);
        // Let the people know what's going on
        GLADIUS_COUT_STAT << "Launcher PID: "
                          << mRMInfo.rm_launcher_pid
                          << std::endl;
        GLADIUS_COUT_STAT << "Spawning "
                          << setw(6) << setfill('0')
                          << hosts.nHosts()
                          << " Daemons"
                          << std::endl;
        int jobidSize = 0;
        char jobid[PATH_MAX];
        LMON_fe_getResourceHandle(
            mSessionNum,
            jobid,
            &jobidSize,
            PATH_MAX
        );
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
LaunchMonFE::sendDaemonInfo(
    const toolcommon::LeafInfo &leafInfo
) {
    VCOMP_COUT("Sending Daemon Info." << std::endl);
    if (!mFEToBePackFn) {
        GLADIUS_THROW("Front-end to Back-end packing function not set.");
    }
    auto rc = LMON_fe_sendUsrDataBe(mSessionNum, (void *)&leafInfo);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_sendUsrDataBe", rc);
    }
}

/**
 *
 */
void
LaunchMonFE::shutdown(void)
{
    mEndSession();
}
