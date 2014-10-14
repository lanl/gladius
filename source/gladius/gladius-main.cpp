/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/gladius-core-includes.h"
#include "gladius.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace {
void
disableBuffering(void)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}
} // end private routines namespace

int
main(
    int argc,
    char **argv,
    char **envp
) {
    using namespace gladius;
    using namespace gladius::core;
    try {
        disableBuffering();
        Gladius gladius(argc, argv, envp);
        gladius.mainLoop();
    } catch (GladiusException &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
