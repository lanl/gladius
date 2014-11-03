/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_TERM_TERM_CMDS_H_INCLUDED
#define GLADIUS_TERM_TERM_CMDS_H_INCLUDED

#include "term.h"
#include "core/utils.h"

// TMP
#include "apa/apa.h"

#include <string>
#include <iostream>
#include <cstdio>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace gladius {
namespace term {

/**
 * Displays help message.
 */
inline void
helpCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args)
{
    GLADIUS_UNUSED(args);
    std::cout << PACKAGE_NAME << " help" << std::endl;
}

/**
 * Displays history.
 */
inline void
historyCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args)
{
    Terminal *t = args.terminal;
    HistEvent &histEvent = t->getHistEvent();
    for (int rv = history(t->getHistory(), &histEvent, H_LAST);
         rv != -1; rv = history(t->getHistory(), &histEvent, H_PREV)) {
        (void)fprintf(stdout, "%4d %s", histEvent.num, histEvent.str);
    }
}

/**
 * Launches target application.
 */
inline void
launchCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args)
{
    std::cout << "starting application launch..." << std::endl;
    int argc = args.argc;
    const char **argv = const_cast<const char **>(args.argv);
    Terminal *t = args.terminal;
    EditLine *editLine = t->getEditLine();
    // The return value is -1 if the command is unknown.
    if (-1 == el_parse(editLine, argc, argv)) {
        // argv[0] will be "launch", so eat that before dealing with the line
        char **appArgv = const_cast<char **>(&argv[1]);
        switch (fork()) {
            // Child (spawned process)
            case 0:
                execvp(argv[1], (char *const *)(appArgv));
                MPIR_being_debugged = 1;
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
}

} // end term namespace
} // end gladius namespace

#endif
