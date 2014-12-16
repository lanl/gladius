/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
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
    char answer[8];
    cin.getline(answer, sizeof(answer));
    if (0 == strcmp("Y", answer)) {
        // Done with REPL
        return false;
    }
    // The answer was not "Y", so continue REPL.
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
    if (args.argc < 2) {
        // launch should have at least 2 arguments. So, help out the usage.
        auto trmCMD = args.terminal->getTermCommands().getTermCMD(args.argv[0]);
        std::cout << trmCMD->shortUsage();
    }
    // If here then launch the thing...
    toolfe::ToolFE *toolFE = args.terminal->getToolFE();
    core::Args launchArgs(args.argc, (const char **)args.argv);
    // TODO add return status to see if we should continue REPL.
    toolFE->launch(launchArgs);
    /* Continue REPL */
    return true;
}

} // end term namespace
} // end ui namespace
} // end gladius namespace

#endif
