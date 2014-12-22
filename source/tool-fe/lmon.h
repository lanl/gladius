/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Front-End (FE) API. The interface to the tool actions.
 */

#ifndef GLADIUS_TOOL_FE_LMON_H_INCLUDED
#define GLADIUS_TOOL_FE_LMON_H_INCLUDED

#include "core/core.h"

#include "lmon_api/lmon_fe.h"

#include <string>
#include <unistd.h>

namespace gladius {
namespace toolfe {

class LaunchMon {
    // The PID of the target application launcher (srun, mpirun, aprun, etc.)
    pid_t mLauncherPID;
    // LMON session number (handle).
    int mSessionNum;
    // The hostname of tool front-end.
    std::string mHostname;
    // The name of the tool daemon.
    std::string mToolD;
    // Daemon option string
    std::string mDaemonOpts;

public:
    /**
     *
     */
    LaunchMon(void) {
        mToolD = "gladiustd";
    }

    /**
     *
     */
    void
    init(void) {
        mHostname = core::Utils::getHostname();
        // Init LaunchMON
        auto rc = LMON_fe_init(LMON_VERSION);
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED("LMON_fe_init");
        }
        rc = LMON_fe_createSession(&mSessionNum);
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED("LMON_fe_createSession");
        }
    }

    /**
     *
     */
    void
    attachAndSpawnDaemons(pid_t launcherPID) {
        // Stash the PID of the app launcher.
        mLauncherPID = launcherPID;
        std::string launcherPIDStr = std::to_string(mLauncherPID);
        auto rc = LMON_fe_attachAndSpawnDaemons(
                      mSessionNum,
                      mHostname.c_str(),
                      mLauncherPID,
                      mToolD.c_str(),
                      NULL,
                      NULL,
                      NULL
                  );
        if (LMON_OK != rc) {
            auto rcs = std::to_string(rc);
            auto errStr = "LMON_fe_attachAndSpawnDaemons. RC = " + rcs;
            GLADIUS_THROW_CALL_FAILED(errStr);
        }
    }
};

} // end toolfe namespace
} // end gladius namespace

#endif
