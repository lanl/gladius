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

#include "lmon.h"
#include "lmon-paths.h"

#include "core/core.h"

using namespace gladius::toolfe;

namespace {
/// The name of the tool daemon.
static const std::string TOOLD_NAME = "gladius-toold";
/// What to use for remote login
static const std::string REMOTE_LOGIN = "/usr/bin/ssh";

/**
 * LaunchMON static callback.
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
        cout << "* session registered" << endl;
    }
    else {
        cout << "* session not registered" << endl;
    }
    if (WIFBESPAWNED(stcp)) {
        cout << "* BE daemons spawned" << endl;
    }
    else {
        cout << "* BE daemons not spawned or exited" << endl;
    }
    if (WIFMWSPAWNED(stcp)) {
        cout << "* MW daemons spawned" << endl;
    }
    else {
        cout << "* MW daemons not spawned or exited" << endl;
    }
    if (WIFDETACHED(stcp)) {
        cout << "* the job is detached" << endl;
    }
    else {
        if (WIFKILLED(stcp)) {
            cout << "* the job is killed" << endl;
        }
        else {
            cout << "* the job has not been killed" << endl;
        }
    }
    return 0;
}

} // end nameless namespace

/**
 *
 */
LaunchMon::LaunchMon(
    void
) : mBeVerbose(false)
  , mHostname(core::utils::getHostname())
  , mToolD(TOOLD_NAME)
  , mPrefixPath(GLADIUS_TOOL_FE_LMON_PREFIX)
  , mEnginePath(GLADIUS_TOOL_FE_LMON_ENGINE_PATH)
{
}

/**
 * Sets some environment variables that impact the behavior of LaunchMON.
 */
void
LaunchMon::setEnvs(void)
{
    core::utils::setEnv("LMON_PREFIX", mPrefixPath);
    core::utils::setEnv("LMON_LAUNCHMON_ENGINE_PATH", mEnginePath);
    core::utils::setEnv("LMON_REMOTE_LOGIN", REMOTE_LOGIN);
}

/**
 *
 */
void
LaunchMon::init(void)
{
    setEnvs();
    // Init LaunchMON
    auto rc = LMON_fe_init(LMON_VERSION);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED("LMON_fe_init");
    }
    rc = LMON_fe_createSession(&mSessionNum);
    if (LMON_OK != rc) {
        GLADIUS_THROW_CALL_FAILED("LMON_fe_createSession");
    }
    if (mBeVerbose) {
        rc = LMON_fe_regStatusCB(mSessionNum, statusFuncCallback);
        // Not a fatal failure, but warn about this failure.
        if (LMON_OK != rc) {
            GLADIUS_WARN("LMON_fe_regStatusCB Failed...");
        }
    }
}

/**
 *
 */
void
LaunchMon::launchAndSpawnDaemons(
    const core::Args &appArgs
) {
    using namespace std;
    try {
        char **launcherArgv = appArgs.argv();
        string launcherPath;
        // If we weren't provided an absolute path to the launcher, then fix
        // that.
        if (!core::utils::isAbsolutePath(launcherArgv[0])) {
            auto status = core::utils::which(launcherArgv[0], launcherPath);
            if (GLADIUS_SUCCESS != status) {
                GLADIUS_CERR << "Could not find '" << launcherArgv[0]
                             << "' in $PATH.\nProviding an absolute "
                             << "path to the launcher may fix this. "
                             << std::endl;
                return;
            }
        }
        auto rc = LMON_fe_launchAndSpawnDaemons(
                      mSessionNum,
                      mHostname.c_str(),
                      launcherPath.c_str(), // launcher name
                      launcherArgv,    // all of the launch command
                      mToolD.c_str(),
                      NULL,
                      NULL,
                      NULL
                  );
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED("LMON_fe_launchAndSpawnDaemons");
        }
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
