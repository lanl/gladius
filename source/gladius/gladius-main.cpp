/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * This is the main driver for gladius.
 */

#include "gladius.h"

#include "core/core.h"

#include <cstdlib>
#include <iostream>

#include <signal.h>

namespace {
// I don't like this, but such is life. Thanks, signals.
gladius::Gladius *gGladius = nullptr;
/**
 *
 */
void
quitSigHandler(int i)
{
    GLADIUS_UNUSED(i);
    if (gGladius) {
        // Really gonig to quit?
        if (gGladius->shutdown()) {
            delete gGladius;
            exit(EXIT_SUCCESS);
        }
    }
    else exit(EXIT_SUCCESS);
}
/**
 *
 */
void
installSignalHandlers(void)
{
    (void)signal(SIGINT, quitSigHandler);
}
} // end namespace

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
        // Install our signal handlers before we begin.
        installSignalHandlers();
        Args args(argc,
                  const_cast<const char **>(argv),
                  const_cast<const char **>(envp));
        gGladius = new Gladius(args);
        gGladius->run();
    }
    catch (GladiusException &e) {
        GLADIUS_CERR << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (gGladius) delete gGladius;
    return EXIT_SUCCESS;
}
