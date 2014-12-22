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
 * Implements the tool Front-End (FE) API. The interface to the tool actions.
 */

#include "lmon.h"
#include "core/core.h"

using namespace gladius::toolfe;

namespace {
/**
 * LaunchMON static callback.
 */
int
statusFuncCB(int *status)
{
    int stcp = *status;
    fprintf(stdout, "**** status callback routine is invoked:0x%x ****\n", stcp);
    if (WIFREGISTERED(stcp)) {
        fprintf(stdout, "* session registered\n");
    }
    else {
        fprintf(stdout, "* session not registered\n");
    }
    if (WIFBESPAWNED(stcp)) {
        fprintf(stdout, "* BE daemons spawned\n");
    }
    else {
        fprintf(stdout, "* BE daemons not spawned or exited\n");
    }
    if (WIFMWSPAWNED(stcp)) {
        fprintf(stdout, "* MW daemons spawned\n");
    }
    else {
        fprintf(stdout, "* MW daemons not spawned or exited\n");
    }
    if (WIFDETACHED(stcp)) {
        fprintf(stdout, "* the job is detached\n");
    }
    else {
        if (WIFKILLED(stcp)) {
            fprintf(stdout, "* the job is killed\n");
        }
        else {
            fprintf(stdout, "* the job has not been killed\n");
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
  , mToolD("gladiustd")
{
}

/**
 *
 */
void
LaunchMon::init(void)
{
    // Stash the tool FE's host name.
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
    if (mBeVerbose) {
        rc = LMON_fe_regStatusCB(mSessionNum, statusFuncCB);
        if (LMON_OK != rc) {
            GLADIUS_WARN("LMON_fe_regStatusCB Failed...");
        }
    }
}

/**
 *
 */
void
LaunchMon::launchAndSpawnDaemons() {
}
