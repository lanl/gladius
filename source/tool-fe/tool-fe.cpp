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
#include <thread>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius::toolfe;

/**
 * Tool front-end constructor.
 */
ToolFE::ToolFE(void)
{
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
 * The "local" tool front-end that is responsible for all the tool setup.
 */
void
ToolFE::mLocalBody(void)
{
    std::thread beThread(toolbe::ToolBE::foo, 12);
    beThread.join();
}
