/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"

#include <string>

using namespace gladius;

/**
 *
 */
Gladius::Gladius(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    (void)envp;
    lldb::SBDebugger::Initialize();
}

/**
 *
 */
void
Gladius::mainLoop(void)
{
    ;
}
