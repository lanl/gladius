/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-fe.h"
#include "tool-be/tool-be.h"

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius::toolfe;

/**
 * Tool front-end constructor.
 */
ToolFE::ToolFE(
    void
) : mBeVerbose(false)
{
    if (core::Utils::envVarSet(GLADIUS_TOOL_FE_VERBOSE_STR)) {
        mBeVerbose = true;
        mLMON.verbose(mBeVerbose);
    }
}

/**
 * Returns tool front-end help string.
 */
std::string
ToolFE::helpText(void)
{
    return std::string {"toolFE help string goes here..."};
}

/**
 * Responsible for running the tool front-end instance. This is the tool-fe
 * entry point from a caller's perspective.
 */
void
ToolFE::run(
    const core::Args &args
) {
    mAppArgs = args;
    mLocalBody();
}

/**
 * The local tool front-end thread (the main thread).
 */
void
ToolFE::mLocalBody(void)
{
    try {
        mLMON.init();
        std::thread beThread(&ToolFE::mRemoteBody, this);
        std::unique_lock<std::mutex> lock(mtFEBELock);
        mtBELaunchComplete.wait(lock);
        beThread.join();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * The thread that interacts with the tool back-end.
 */
void
ToolFE::mRemoteBody(void)
{
    try {
        std::cout << "starting be" << std::endl;
        mLMON.launchAndSpawnDaemons();
        sleep(2);
        mtBELaunchComplete.notify_one();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
