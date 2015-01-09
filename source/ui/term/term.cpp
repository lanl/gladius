/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 1992,1993 The Regents of the University of California.
 *                         All rights reserved.
 * See tc1.c in the libedit distribution for more details.
 */

#include "term.h"
#include "core/core.h"
#include "term-cmds.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <string.h>
#include <locale.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace {
int continuation = 0;
volatile sig_atomic_t gotsig = 0;

/**
 *
 */
char *
getPromptCallback(EditLine *el)
{
    GLADIUS_UNUSED(el);
    // TODO add mode name to prompt.
    static char p[] = "(" PACKAGE_NAME ") ";
    return p;
}

/**
 *
 */
void
sigHandler(int i)
{
    gotsig = i;
    switch(i) {
        default: std::cout << std::endl << std::flush;
    }
}

/**
 *
 */
void
setLocale(void)
{
    (void)setlocale(LC_CTYPE, "");
}

/**
 * Disables buffering for stdio and stderr.
 */
void
disableBuffering(void)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
}

} // end namespace for private things

using namespace gladius::ui::term;

/**
 * Name of the history file.
 */
const std::string Terminal::sHistFileName = "history";

/**
 * Terminal commands registry.
 */
TermCommands Terminal::sTermCommands {
    TermCommand(
        "quit",
        "exit, q",
        "quit",
        "quit Help",
        quitCMDCallback
    ),
    TermCommand(
        "help",
        "?",
        "help",
        "help Help",
        helpCMDCallback
    ),
    TermCommand(
        "launch",
        "l",
        "launch [OPTIONS...] executable [args...]",
        "launch Help",
        launchCMDCallback
    ),
    TermCommand(
        "modes",
        "",
        "modes",
        "modes Help",
        modesCMDCallback
    ),
    TermCommand(
        "history",
        "hist, h",
        "history",
        "history Help",
        historyCMDCallback
    ),
    TermCommand(
        "setenv",
        "",
        "setenv ENV_VAR VAL",
        "setenv Help",
        setEnvCMDCallback
    ),
    TermCommand(
        "unsetenv",
        "",
        "unsetenv ENV_VAR",
        "unsetenv Help",
        unsetEnvCMDCallback
    ),
    TermCommand(
        "clear",
        "",
        "clear",
        "clear",
        clearCMDCallback
    )
};

/**
*
 */
Terminal &
Terminal::TheTerminal(void)
{
    static Terminal *singleton = new Terminal();
    return *singleton;
}

/**
 * Implements UI init functionality.
 */
void
Terminal::init(const core::Args &args)
{
    mArgs = args;
    setLocale();
    disableBuffering();
    installSignalHandlers();
    if (NULL == (mHist = history_init())) {
        GLADIUS_THROW_CALL_FAILED("history_init");
    }
    // Remember events
    if (history(mHist, &mHistEvent, H_SETSIZE, sHistSize) < 0) {
        GLADIUS_THROW_CALL_FAILED("history");
    }
    // Save only unique events that are next to each other.
    if (history(getHistory(), &getHistEvent(), H_SETUNIQUE, 1) < 0) {
        GLADIUS_THROW_CALL_FAILED("history H_SETUNIQUE");
    }
    if (NULL == (mEditLine = el_init(*(mArgs.argv()), stdin, stdout, stderr))) {
        GLADIUS_THROW_CALL_FAILED("el_init");
    }
    // Set default editor : emacs (for now)
    if (el_set(mEditLine, EL_EDITOR, "emacs") < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_EDITOR");
    }
    // Handle signals gracefully
    if (el_set(mEditLine, EL_SIGNAL, 1) < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_SIGNAL");
    }
    // Tell editline to use this history interface
    if (el_set(mEditLine, EL_HIST, history, mHist) < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_HIST");
    }
    // Set the prompt function
    if (el_set(mEditLine, EL_PROMPT, getPromptCallback) < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_PROMPT_ESC");
    }
    // Bind C-R: Cycle through backwards search, entering string
    if (el_set(mEditLine, EL_BIND, "^r", "em-inc-search-prev", NULL) < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_BIND ^r");
    }
    // Source $PWD/.editrc then $HOME/.editrc
    el_source(mEditLine, NULL);
    // Initialize tokenizer
    mTokenizer = tok_init(NULL);
    try {
        mSession.open();
        mHistFile = mSession.sessionDir()
                  + core::utils::osPathSep
                  + sHistFileName;
        // Load history
        mLoadHistory();
        mToolFE = new toolfe::ToolFE();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * Implements UI interact functionality.
 */
void
Terminal::interact(void)
{
    try {
        mEnterREPL();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * Implements UI shutdown functionality.
 */
bool
Terminal::quit(void)
{
    EvalInputCmdCallBackArgs cba(this, 0, NULL);
    return !quitCMDCallback(cba);
}

/**
 * Top-level routine to start the read–eval–print loop (REPL).
 */
void
Terminal::mEnterREPL(void)
{
    // Flag indicating whether or not the REPL should continue.
    bool continueREPL = true;
    // Points to current line
    const char *cLineBufp = nullptr;
    int nCharsRead = 0, nContinuation = 0;

    while (continueREPL
           && NULL != (cLineBufp = el_gets(mEditLine, &nCharsRead))
           && 0 != nCharsRead)  {
        if (gotsig) {
            gotsig = 0;
            el_reset(mEditLine);
            continue;
        }
        if (!continuation && nCharsRead == 1) continue;
        // cc and co are cursor things that we aren't using (yet?).
        int tokArgc = 0, cc = 0, co = 0;
        const char **tokArgv = NULL;
        if ((nContinuation = tok_line(mTokenizer, el_line(mEditLine),
                                      &tokArgc, &tokArgv, &cc, &co)) < 0) {
            GLADIUS_CERR_WARN << "tok_line failed... Resetting things..."
                              << std::endl;
            tok_reset(mTokenizer);
            el_reset(mEditLine);
            continue;
        }
        continuation = nContinuation;
        nContinuation = 0;
        if (continuation) continue;
        // Process current input
        evaluateInput(tokArgc, tokArgv, continueREPL);
        // Update our history, but don't add bang#s to history. We only want to
        // add the command that corresponds to a !#.
        if ('!' != cLineBufp[0]) {
            history(getHistory(), &getHistEvent(),
                    continuation ? H_APPEND : H_ENTER, cLineBufp);
        }
        tok_reset(mTokenizer);
    }
}

/**
 * Returns a vector of string pairs sorted by command name.
 */
std::vector< std::pair<std::string, std::string> >
Terminal::cmdPairs(void) const
{
    using namespace std;
    set< pair<string, string> > theSet;
    for (auto tcmd : sTermCommands.availableCommands()) {
        theSet.insert(make_pair(tcmd.command(), tcmd.shortUsage()));
    }
    vector< pair<string, string> > theVec(theSet.begin(), theSet.end());
    std::sort(theVec.begin(), theVec.end());
    return theVec;
}

/**
 *
 */
bool
Terminal::mHistRecallRequest(
    const std::string &input,
    std::string &histStringIfValid
) {
    histStringIfValid = "";
    if ('!' == input.at(0)) {
        // Okay, this is the start of a request. Now make sure that the next bit
        // is a number and corresponds to a history event.
        try {
            auto numStr = input.substr(1);
            auto histNum = std::stoi(numStr);
            HistEvent &histEvent = getHistEvent();
            auto status = history(getHistory(), &histEvent,
                                  H_NEXT_EVENT, histNum);
            // Not found in history...
            if (status < 0) {
                GLADIUS_CERR << input << ": event not found" << std::endl;
                // Event was not found, so restore the history position.
                history(getHistory(), &histEvent, H_FIRST);
                // Return true also in this path so that an empty string will be
                // pushed and only one handler path will run. Trust me, this
                // seems to work...
            }
            else {
                // If we are here, then all is well. Return the history string.
                histStringIfValid = histEvent.str;
            }
            return true;
        }
        catch (...) {
            // Some invalid garbage passed to us, so reset things and return
            // false;
            history(getHistory(), &getHistEvent(), H_FIRST);
            return false;
        }
    }
    else return false;
}

/**
 *
 */
void
Terminal::evaluateInput(
    int argc,
    const char **argv,
    bool &continueREPL
) {
    // First see if this is a history recall. If so, handle that.
    std::string maybeHistRecallString;
    if (mHistRecallRequest(argv[0], maybeHistRecallString)) {
        tok_reset(mTokenizer);
        // Removing the newline before passing things to el_push provide history
        // recall behavior akin to bash's when shopt -s histverify.
        maybeHistRecallString.erase(
            std::remove(maybeHistRecallString.begin(),
                        maybeHistRecallString.end(), '\n'),
            maybeHistRecallString.end()
        );
        el_push(getEditLine(), maybeHistRecallString.c_str());
        return;
    }
    // If here, then just deal with the input as is.
    auto maybeTermCmd = sTermCommands.getTermCMD(argv[0]);
    if (!maybeTermCmd) {
        GLADIUS_CERR << "\'" << argv[0] << "\' "
                     << "is not a valid command. Try \'help\'."
                     << std::endl;
    }
    else {
        // Found it, so call the registered callback associated with the
        // string and stash whether or not we should continue the REPL.
        continueREPL = maybeTermCmd->exec(
                           EvalInputCmdCallBackArgs(this, argc, argv)
                       );
    }
}

/**
 *
 */
void
Terminal::installSignalHandlers(void)
{
    (void)signal(SIGINT, sigHandler);
#if 0
    (void)signal(SIGQUIT, sigHandler);
    (void)signal(SIGHUP, sigHandler);
    (void)signal(SIGTERM, sigHandler);
#endif
}

/**
 *
 */
void
Terminal::mLoadHistory(void)
{
    // If the file exists, then load it.
    if (core::utils::fileExists(mHistFile)) {
        if (history(mHist, &mHistEvent, H_LOAD, mHistFile.c_str()) < 0) {
            GLADIUS_CERR_WARN << "Command history couldn't be loaded from: "
                              << mHistFile << std::endl;
        }
    }
}

/**
 *
 */
void
Terminal::mSaveHistory(void)
{
    if (history(mHist, &mHistEvent, H_SAVE, mHistFile.c_str()) < 0) {
        GLADIUS_CERR_WARN << "Command history couldn't be saved to: "
                          << mHistFile << std::endl;
    }
}

/**
 *
 */
Terminal::~Terminal(void)
{
    mSaveHistory();
    if (mEditLine) el_end(mEditLine);
    if (mTokenizer) tok_end(mTokenizer);
    if (mHist) history_end(mHist);
    if (mToolFE) delete mToolFE;
}
