/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
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

TermCommands Terminal::sTermCommands {
        TermCommand("help", "s", "l", helpCMDCallback),
        TermCommand("?", "s", "l", helpCMDCallback),
        TermCommand("modes", "s", "l", modesCMDCallback),
        TermCommand("history", "s", "l", historyCMDCallback),
        TermCommand("hist", "s", "l", historyCMDCallback),
        TermCommand("launch", "s", "l", launchCMDCallback),
        TermCommand("set", "s", "l", setModeCMDCallback)
};

/**
 *
 */
Terminal::Terminal(const core::Args &args)
    : UI(args)
{
    setLocale();
    disableBuffering();
    setSignalHandlers();
    if (NULL == (mHist = history_init())) {
        GLADIUS_THROW_CALL_FAILED("history_init");
    }
    // Remember events
    if (history(mHist, &mHistEvent, H_SETSIZE, sHistSize) < 0) {
        GLADIUS_THROW_CALL_FAILED("history");
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
        GLADIUS_THROW_CALL_FAILED("el_set EL_BIND");
    }
    // Source $PWD/.editrc then $HOME/.editrc
    el_source(mEditLine, NULL);
    // Initialize tokenizer
    mTokenizer = tok_init(NULL);
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
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
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
    const char *cLineBufp = NULL;
    int nCharsRead = 0, nContinuation = 0;

    while (continueREPL
           && NULL != (cLineBufp = el_gets(mEditLine, &nCharsRead))
           && 0 != nCharsRead)  {
        if (gotsig) {
            (void)fprintf(stderr, "Got signal %d.\n", gotsig);
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
            GLADIUS_THROW_CALL_FAILED("tok_line");
        }
        // Update our history
        history(mHist, &mHistEvent, continuation ? H_APPEND : H_ENTER, cLineBufp);
        continuation = nContinuation;
        nContinuation = 0;
        if (continuation) continue;
        // Process current input
        evaluateInput(tokArgc, tokArgv, &continueREPL);
    }
}

/**
 *
 */
std::vector<std::string>
Terminal::termCmds(void) const
{
    return sTermCommands.availableCommands();
}

/**
 *
 */
void
Terminal::evaluateInput(
    int argc,
    const char **argv,
    bool *continueREPL
) {
    // This command is special, so check for it outside of the lookup table.
    if (strcmp(argv[0], "quit") == 0) {
        *continueREPL = false;
        return;
    }
    // Else see if the command is in our command lookup table.
    else {
        auto maybeTermCmd = sTermCommands.getTermCMD(argv[0]);
        if (nullptr == maybeTermCmd) {
            std::cout << "error: \'" << argv[0] << "\' "
                      << "is not a valid command. Try \'help\'." << std::endl;
        }
        else {
            // Found it, so call the registered callback associated with the
            // string.
            maybeTermCmd->exec(EvalInputCmdCallBackArgs(this, argc, argv));
        }
    }
    tok_reset(mTokenizer);
    // continue REPL
    *continueREPL = true;
}

/**
 *
 */
void
Terminal::setSignalHandlers(void)
{
    (void)signal(SIGINT, sigHandler);
    (void)signal(SIGQUIT, sigHandler);
    (void)signal(SIGHUP, sigHandler);
    (void)signal(SIGTERM, sigHandler);
}

/**
 *
 */
Terminal::~Terminal(void)
{
    if (mEditLine) el_end(mEditLine);
    if (mTokenizer) tok_end(mTokenizer);
    if (mHist) history_end(mHist);
    if (mToolFE) delete mToolFE;
}
