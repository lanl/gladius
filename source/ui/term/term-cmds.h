/*
 * Copyright (c) 2014-2016 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include "term.h"
#include "core/core.h"
#include "core/colors.h"
#include "core/env.h"
#include "tool-fe/tool-fe.h"

#include <string>
#include <vector>
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
    GLADIUS_CERR_WARN << "Quitting. Do you really want to proceed: [Y/n]: "
                      << flush;
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
 * launch application [OPTION]... with launcher [OPTION]...
 */
inline bool
launchCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    using namespace std;
    // Recall: 'launch' will always be the first argument in argv
    // launch should have at least 4 arguments. So, print out the usage.
    if (args.argc < 4) {
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    // Index of where the launcher argv begins. 1 to skip 'launch'
    int li = 1;
    // Grab application argv.
    vector<string> appArgv;
    for (; li < args.argc; ++li) {
        const string arg = args.argv[li];
        if ("with" == arg) break;
        appArgv.push_back(arg);
    }
    if (appArgv.empty() || li == args.argc ||
        "with" != string(args.argv[li])) {
        GLADIUS_CERR << "Malformed launch command... Please try again." << endl;
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    // Grab launcher argv.
    // Skip 'with'
    li += 1;
    vector<string> launcherArgv;
    for (; li < args.argc; ++li) {
        launcherArgv.push_back(args.argv[li]);
    }
    // Make sure with what is provided.
    if (launcherArgv.empty()) {
        GLADIUS_CERR << "Launcher command not provided. 'with' what?" << endl;
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    //
    args.terminal->TheTerminal().uninstallSignalHandlers();
    // A new instance every time we are here.
    toolfe::ToolFE toolFE;
    // Enter the tool's main loop.
    (void)toolFE.main(core::Args(appArgv), core::Args(launcherArgv));
    //
    args.terminal->TheTerminal().installSignalHandlers();
    // Continue REPL
    return true;
}

/**
 * Prints environment variables.
 * Expecting:
 * env
 */
inline bool
envCMDCallback(const EvalInputCmdCallBackArgs &args)
{
    // env should have 1 argument
    if (args.argc != 1) {
        echoCommandUsage(args, args.argv[0]);
        return true;
    }
    core::Environment::TheEnvironment().prettyPrint();
    /* Continue REPL */
    return true;
}

} // end term namespace
} // end ui namespace
} // end gladius namespace
