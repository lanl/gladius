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
#include "core/core.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace gladius {
namespace ui {
namespace term {

/**
 * Quits.
 */
inline bool
quitCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    using namespace std;
    GLADIUS_UNUSED(args);
    cout << endl << "Quitting. Do you really want to proceed: [Y/n]: " << flush;
    switch (cin.get()) {
        case 'Y' : return false;
        default: return true;
    }
    /* Done with REPL */
    return true;
}

/**
 * Displays help message.
 */
inline bool
helpCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    using namespace std;
    string header(PACKAGE_NAME " help");
    stringstream hbot;
    for (auto ci = header.cbegin(); ci != header.cend(); ++ci) {
        hbot << "-";
    }
    // Print help banner.
    cout << '\n' << header << '\n' << hbot.str() << endl;
    // Print available commands.
    cout << "o Available Commands" << endl;
    for (const auto cmdp : args.terminal->cmdPairs()) {
        cout << "- " << cmdp.first << " : "
             << cmdp.second << endl;
    }
    /* Continue REPL */
    return true;
}

/**
 * Displays the available modes.
 */
inline bool
modesCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    GLADIUS_UNUSED(args);
    /* Continue REPL */
    return true;
}

/**
 * Sets the debug mode.
 */
inline bool
setModeCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    GLADIUS_UNUSED(args);
    /* Continue REPL */
    return true;
}

/**
 * Displays history.
 */
inline bool
historyCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    Terminal *t = args.terminal;
    HistEvent &histEvent = t->getHistEvent();
    for (int rv = history(t->getHistory(), &histEvent, H_LAST);
         rv != -1; rv = history(t->getHistory(), &histEvent, H_PREV)) {
        (void)fprintf(stdout, "%4d %s", histEvent.num, histEvent.str);
    }
    /* Continue REPL */
    return true;
}

/**
 * Launches target application.
 */
inline bool
launchCMDCallback(const EvalInputCmdCallBackArgs &args)
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
    /* Continue REPL */
    return true;
}

} // end term namespace
} // end ui namespace
} // end gladius namespace

#endif
