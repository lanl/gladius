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
 *
 * University of Illinois/NCSA Open Source License
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 * For more details, please see LICENSE.TXT in the LLDB distirubtion.
 */

#include "term.h"

#include "core/utils.h"

#include <string>

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
    static char p[] = "(" PACKAGE_NAME ") ";
    return p;
}

}

using namespace gladius::term;

/**
 *
 */
Terminal:: Terminal(
    int argc,
    const char **argv
) {
    GLADIUS_UNUSED(argc);

    (void)setlocale(LC_CTYPE, "");

    if (NULL == (mHist = history_init())) {
        GLADIUS_THROW_CALL_FAILED("history_init");
    }
    // Remember events
    if (history(mHist, &mHistEvent, H_SETSIZE, sHistSize) < 0) {
        GLADIUS_THROW_CALL_FAILED("history");
    }
    if (NULL == (mEditLine = el_init(*argv, stdin, stdout, stderr))) {
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
}

/**
 * Top-level routine to start the read–eval–print loop (REPL).
 */
void
Terminal::enterREPL(void)
{
    // Points to current line
    const char *cLineBufp;
    int nCharsRead;
    int ncontinuation;
    mTokenizer = tok_init(NULL);

    while (NULL != (cLineBufp = el_gets(mEditLine, &nCharsRead)) &&
           0 != nCharsRead)  {
        int ac, cc, co;
        const char **av;
        const LineInfo *li;
        li = el_line(mEditLine);
        if (gotsig) {
            (void) fprintf(stderr, "Got signal %d.\n", gotsig);
            gotsig = 0;
            el_reset(mEditLine);
        }

        if (!continuation && nCharsRead == 1)
            continue;

        ac = cc = co = 0;
        ncontinuation = tok_line(mTokenizer, li, &ac, &av, &cc, &co);
        if (ncontinuation < 0) {
            (void) fprintf(stderr, "Internal error\n");
            continuation = 0;
            continue;
        }
        history(mHist, &mHistEvent, continuation ? H_APPEND : H_ENTER, cLineBufp);
        continuation = ncontinuation;
        ncontinuation = 0;
        if (continuation)
            continue;
#ifdef DEBUG
        for (i = 0; i < ac; i++) {
            (void) fprintf(stderr, "  > arg# %2d ", i);
            if (i != cc)
                (void) fprintf(stderr, "`%s'\n", av[i]);
            else
                (void) fprintf(stderr, "`%.*s_%s'\n",
                    co, av[i], av[i] + co);
        }
#endif
        if (strcmp(av[0], "quit") == 0) {
            break;
        }
        else if (strcmp(av[0], "history") == 0) {
            int rv;

            switch (ac) {
            case 1:
                for (rv = history(mHist, &mHistEvent, H_LAST); rv != -1;
                    rv = history(mHist, &mHistEvent, H_PREV))
                    (void) fprintf(stdout, "%4d %s",
                        mHistEvent.num, mHistEvent.str);
                break;

            case 2:
                if (strcmp(av[1], "clear") == 0)
                     history(mHist, &mHistEvent, H_CLEAR);
                else
                     goto badhist;
                break;

            case 3:
                if (strcmp(av[1], "load") == 0)
                     history(mHist, &mHistEvent, H_LOAD, av[2]);
                else if (strcmp(av[1], "save") == 0)
                     history(mHist, &mHistEvent, H_SAVE, av[2]);
                break;

            badhist:
            default:
                (void) fprintf(stderr,
                    "Bad history arguments\n");
                break;
            }
        } else if (el_parse(mEditLine, ac, av) == -1) {
            switch (fork()) {
            case 0:
                execvp(av[0], (char *const *)(av));
                perror(av[0]);
                _exit(1);
                /*NOTREACHED*/
                break;

            case -1:
                perror("fork");
                break;

            default:
                if (wait(&nCharsRead) == -1)
                    perror("wait");
                (void) fprintf(stderr, "Exit %x\n", nCharsRead);
                break;
            }
        }
        tok_reset(mTokenizer);
    }
}

/**
 *
 */
Terminal::~Terminal(void)
{
    if (mEditLine) el_end(mEditLine);
    if (mTokenizer) tok_end(mTokenizer);
    if (mHist) history_end(mHist);
}
