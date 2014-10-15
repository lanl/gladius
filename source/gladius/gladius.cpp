/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"
#include "core/core-includes.h"

#include <string>

using namespace gladius;

/**
 *
 */
Gladius::~Gladius(void)
{
    if (dbe) delete dbe;
}

/**
 *
 */
Gladius::Gladius(
    int argc,
    const char **argv,
    const char **envp
) {
    using namespace gladius;
    (void)argc;
    (void)argv;
    (void)envp;

    try {
        dbe = new dbe::GladiusDBE(argc, argv, envp);
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
Gladius::mainLoop(void)
{
    try {
        dbe->mainLoop();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
