/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-fe.h"

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius::toolfe;

/**
 * Returns tool front-end help string.
 */
std::string
ToolFE::helpText(void) const
{
    return std::string {"toolFE help string goes here..."};
}

/**
 * Responsible for application launch.
 */

void
ToolFE::launch(
    const core::Args &args
) {
    std::cout << "starting application launch..." << std::endl;
    const char **argv = const_cast<const char **>(args.argv());
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
