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
#include "tool-common/tool-common.h"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

#include "lmon_api/lmon_fe.h"
#include "lmon_api/lmon_proctab.h"

namespace gladius {
namespace toolfe {

class LaunchMon {
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
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
    // LaunchMON install prefix
    std::string mPrefixPath;
    // LaunchMON engine path.
    std::string mEnginePath;
    // Resource manager info
    lmon_rm_info_t mRMInfo;
    // The process table of the active job.
    toolcommon::ProcessTable mProcTab;
    // The hosts in our job.
    toolcommon::Hosts mHosts;

    ////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ////////////////////////////////////////////////////////////////////////////
    void
    mSetEnvs(void);
    //
    void
    mStartSession(void);
    //
    void
    mEndSession(void);
    //
    void
    mCreateAndPopulateProcTab(void);
    //
    void
    mSetRMInfo(void);

public:
    LaunchMon(void);
    //
    ~LaunchMon(void);
    //
    void
    init(void);
    //
    void
    launchAndSpawnDaemons(const core::Args &appArgs);
    //
    void
    attachAndSpawnDaemons(pid_t launcherPID) {
        GLADIUS_UNUSED(launcherPID);
    }
    /**
     * Sets whether or not LaunchMON operations will be verbose.
     */
    void
    verbose(bool bVerbose) {
        mBeVerbose = bVerbose;
    }
};

} // end toolfe namespace
} // end gladius namespace

#endif
