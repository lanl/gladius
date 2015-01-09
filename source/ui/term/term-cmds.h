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
#include "core/colors.h"

#include <string>
#include <iostream>
#include <sys/types.h>
#include <signal.h>

namespace {
/**
 *
 */
void
echoCommandUsage(
    const gladius::ui::term::EvalInputCmdCallBackArgs &args,
    const std::string &cmdName
) {
    using namespace gladius;

    auto trmCMD = args.terminal->getTermCommands().getTermCMD(cmdName);
    GLADIUS_CERR_WARN << "Usage: "
                      << trmCMD->shortUsage()
                      << std::endl;
}

}

namespace gladius {
namespace ui {
namespace term {
/**
 * Clears the screen.
 */
inline bool
clearCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    static char clearHex[2] = {0x0C, '\0'};
    el_push(args.terminal->getEditLine(), clearHex);
    return true;
}
/**
 * Quits.
 */
inline bool
quitCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    using namespace std;
    GLADIUS_UNUSED(args);
    GLADIUS_CERR_WARN << "Quitting. Do you really want to proceed: [Y/n]: " << flush;
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
    using namespace gladius::core;
    using namespace std;
    string header(PACKAGE_NAME " help");
    stringstream hbot;
    for (auto ci = header.cbegin(); ci != header.cend(); ++ci) {
        hbot << "-";
    }
    // Print help banner.
    cout << '\n'
         << colors::color().ansiBeginColor(colors::MAGENTA)
         << header
         << colors::color().ansiEndColor()
         << '\n' << hbot.str() << endl;
    // Print available commands.
    cout << "o Available Commands" << endl;
    for (const auto cmdp : args.terminal->cmdPairs()) {
        cout << "- " << cmdp.first << " : "
             << cmdp.second << endl;
    }
    cout << endl;
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
 * Sets an environment variable during a debug session.
 * Form: setenv ENV_VAR VAL
 */
inline bool
setEnvCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    using std::string;
    using namespace gladius::core;

    if (args.argc != 3) {
        // should have 3 arguments. So, print out the usage.
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    try {
        char **argv = args.argv;
        utils::setEnv(string(argv[1]), string(argv[2]));
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    // Continue REPL
    return true;
}

/**
 * Unsets an environment variable during a debug session.
 * Form: unsetenv ENV_VAR
 */
inline bool
unsetEnvCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    using std::string;
    using namespace gladius::core;

    if (args.argc != 2) {
        // should have 3 arguments. So, print out the usage.
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    char **argv = args.argv;
    auto err = 0;
    if (GLADIUS_SUCCESS != utils::unsetEnv(string(argv[1]), err)) {
        string whatsWrong = utils::getStrError(err);
        GLADIUS_CERR_WARN << "unsetenv Failed: " << whatsWrong << std::endl;
    }
    // Continue REPL
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
 * Expecting:
 * launch <LAUNCHER> [LAUNCHER_ARGS] APP
 */
inline bool
launchCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    // launch should have at least 2 arguments. So, print out the usage.
    if (args.argc < 2) {
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    // If here then run to tool front-end and enter its REPL.
    // Adjust the argv that we are gonig to pass to the toolfe by removing the
    // launch command string and adjusting the arg count.
    core::Args launchArgs(args.argc - 1, (const char **)args.argv + 1);
    toolfe::ToolFE *toolFE = args.terminal->getToolFE();
    toolFE->run(launchArgs);
    /* Continue REPL */
    return true;
}

} // end term namespace
} // end ui namespace
} // end gladius namespace

#endif
