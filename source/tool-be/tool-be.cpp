/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-be.h"

#include <cstdlib>
#include <string>

using namespace gladius::toolbe;

/**
 * Tool front-end constructor.
 */
ToolBE::ToolBE(void)
{
}

#if 0
/**
 * The "local" tool front-end that is responsible for all the tool setup.
 */
void
ToolBE::mLocalBody(void)
{
    const char **argv = const_cast<const char **>(mAppArgs.argv());
    // argv[0] will be "launch", so eat that before dealing with the line
    char **appArgv = const_cast<char **>(&argv[1]);
    switch (fork()) {
        // Child (spawned process)
        case 0:
            execvp(argv[1], (char *const *)(appArgv));
            // Print out any errors that may have occurred during the execvp
            perror(argv[0]);
            _exit(EXIT_FAILURE);
            // Not reached
            break;
        // Fork error
        case -1:
            perror("fork");
            break;
        // Parent
        default: {
            int status;
            if (-1 == wait(&status)) {
                GLADIUS_THROW_CALL_FAILED("wait");
            }
            break;
        }
    }
}
#endif

/**
 * Tool daemon main
 */
int
main(
    int argc,
    char **argv,
    char **envp
) {
    return EXIT_SUCCESS;
}
