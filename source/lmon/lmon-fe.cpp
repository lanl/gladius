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

using namespace gladius;
using namespace gladius::toolfe;

namespace {
// This component's name.
static const std::string CNAME = "lmon-fe";
// CNAME's color code.
static const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::DGRAY);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
/// What to use for remote login
static const std::string REMOTE_LOGIN = "/usr/bin/ssh";

/**
 * LaunchMON status callback.
 */
int
statusFuncCallback(int *status)
{
    using namespace std;

    if (!status) {
        GLADIUS_THROW_INVLD_ARG();
    }
    int stcp = *status;
    if (WIFREGISTERED(stcp)) {
        COMP_COUT << "* Session Registered" << endl;
    }
    else {
        COMP_COUT << "* Session Not Registered" << endl;
    }
    if (WIFBESPAWNED(stcp)) {
        COMP_COUT << "* Back-End Daemons Spawned" << endl;
    }
    else {
        COMP_COUT << "* Back-End Daemons Not Spawned or Exited" << endl;
    }
    if (WIFMWSPAWNED(stcp)) {
        COMP_COUT << "* MW Daemons Spawned" << endl;
    }
    else {
        COMP_COUT << "* MW Daemons Not Spawned or Exited" << endl;
    }
    if (WIFDETACHED(stcp)) {
        COMP_COUT << "* The Job is Detached" << endl;
    }
    else {
        if (WIFKILLED(stcp)) {
            COMP_COUT << "* The Job is Killed" << endl;
        }
        else {
            COMP_COUT << "* The Job Has Not Been Killed" << endl;
        }
    }
    return 0;
}

/**
 *
 */
std::string
genNotInPathErrString(
    const std::string &whatsNotInPath
) {
    auto msg = "It appears as if '" + whatsNotInPath
               + "', is not in your $PATH.\n"
               + "Please update your $PATH to include its location.";
    return msg;
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
  , mRemoteLogin(REMOTE_LOGIN) { ; }

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
LaunchMonFE::mEnvSane(
    std::string &whatsWrong
) {
    whatsWrong = "";
    if (mBeVerbose) {
        COMP_COUT << "Checking Sanity of our Environment." << std::endl;
    }
    // Make sure that our tool daemon is in our PATH.
    auto status = core::utils::which(sToolDName, mToolD);
    if (GLADIUS_SUCCESS != status) {
        whatsWrong = genNotInPathErrString(sToolDName);
        return false;
    }
    // Do the same for launchmon
    status = core::utils::which(sLaunchMONName, mEnginePath);
    if (GLADIUS_SUCCESS != status) {
        whatsWrong = genNotInPathErrString(sLaunchMONName);
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
LaunchMonFE::init(void)
{
    if (mBeVerbose) {
        COMP_COUT << "Initializing LaunchMon Front-End." << std::endl;
    }
    std::string whatsWrong;
    if (!mEnvSane(whatsWrong)) {
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
    // If being verbose, register LMON status callbacks.
    if (mBeVerbose) {
        rc = LMON_fe_regStatusCB(mSessionNum, statusFuncCallback);
        // Not a fatal failure, but warn about this failure.
        if (LMON_OK != rc) {
            GLADIUS_WARN("LMON_fe_regStatusCB Failed...");
        }
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
    int
    (*packFeBeFn) (
        void *udata,
        void *msgbuf,
        int msgbufmax,
        int *msgBufLen
    )
) {
    mFEToBePackFn = packFeBeFn;
}

/**
 *
 */
void
LaunchMonFE::launchAndSpawnDaemons(
    const core::Args &appArgs,
    toolcommon::Hosts &outRemoteHosts
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
        GLADIUS_COUT_STAT << "Launching and Spawning Daemons" << std::endl;
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
        // Get and set RM things
        mSetRMInfo();
        // Create and populate process table.
        mCreateAndPopulateProcTab();
        // Update flag
        mIsLaunched = true;
        // Populate target hosts. Note: this is a set of host names, so the size
        // of this set is equal to the number of servers (nodes) that are being
        // used in this job. This "should" be equal to the number of daemons
        // that were also spawned.
        mHosts = toolcommon::Hosts(mProcTab);
        // While we're at it, pass hosts to caller.
        outRemoteHosts = mHosts;
        // Let the people know what's going on
        GLADIUS_COUT_STAT << "Launcher PID: "
                          << mRMInfo.rm_launcher_pid
                          << std::endl;
        GLADIUS_COUT_STAT << "Number of Spawned Daemons: "
                          << mHosts.nHosts()
                          << std::endl;
        int jobidSize = 0;
        char jobid[PATH_MAX];
        LMON_fe_getResourceHandle(
            mSessionNum,
            jobid,
            &jobidSize,
            PATH_MAX
        );
        LMON_fe_recvUsrDataBe(mSessionNum, NULL);
        LMON_fe_sendUsrDataBe(mSessionNum, NULL);
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
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
