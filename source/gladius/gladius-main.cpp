/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * This is the main driver for gladius.
 */

#include "gladius.h"

#include "core/core-includes.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

/**
 * main
 */
int
main(
    int argc,
    char **argv,
    char **envp
) {
    using namespace gladius;
    using namespace gladius::core;
    try {
        Gladius gladius(argc, const_cast<const char **>(argv),
                        const_cast<const char **>(envp));
        gladius.run();
    } catch (GladiusException &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
