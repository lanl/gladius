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
prompt(EditLine *el)
{
    GLADIUS_UNUSED(el);
    static char a[] = "\1\033[7m\1Edit$\1\033[0m\1 ";
    static char b[] = "Edit> ";
    return continuation ? b : a;
}

unsigned char
complete(
    EditLine *el,
    int ch
) {
    GLADIUS_UNUSED(ch);
    DIR *dd = opendir(".");
    struct dirent *dp;
    const char* ptr;
    const LineInfo *lf = el_line(el);
    size_t len;
    int res = CC_ERROR;

    /*
     * Find the last word
     */
    for (ptr = lf->cursor - 1;
        !isspace((unsigned char)*ptr) && ptr > lf->buffer; ptr--)
        continue;
    len = lf->cursor - ++ptr;

    for (dp = readdir(dd); dp != NULL; dp = readdir(dd)) {
        if (len > strlen(dp->d_name))
            continue;
        if (strncmp(dp->d_name, ptr, len) == 0) {
            if (el_insertstr(el, &dp->d_name[len]) == -1)
                res = CC_ERROR;
            else
                res = CC_REFRESH;
            break;
        }
    }

    closedir(dd);
    return res;
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
    HistEvent ev;

    (void)setlocale(LC_CTYPE, "");

    if (NULL == (hist = history_init())) {
        GLADIUS_THROW_CALL_FAILED("history_init");
    }
    // Remember events
    if (history(hist, &ev, H_SETSIZE, histSize) < 0) {
        GLADIUS_THROW_CALL_FAILED("history");
    }
    tokenizer = tok_init(NULL);
    if (NULL == (editLine = el_init(*argv, stdin, stdout, stderr))) {
        GLADIUS_THROW_CALL_FAILED("el_init");
    }
    // Set default editor : vi, folks
    if (el_set(editLine, EL_EDITOR, "vi") < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_EDITOR");
    }
    // Handle signals gracefully
    if (el_set(editLine, EL_SIGNAL, 1) < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_SIGNAL");
    }
    // Set the prompt function
    if (el_set(editLine, EL_PROMPT_ESC, prompt, '\1') < 0) {
        GLADIUS_THROW_CALL_FAILED("el_set EL_PROMPT_ESC");
    }
    // Tell editline to use this history interface
    el_set(editLine, EL_HIST, history, hist);
    // Add a user-defined function
    el_set(editLine, EL_ADDFN, "ed-complete", "Complete argument", complete);
    // Bind tab to it
    el_set(editLine, EL_BIND, "^I", "ed-complete", NULL);
    // Bind j, k in vi command mode to previous and next line, instead of
    // previous and next history.
    el_set(editLine, EL_BIND, "-a", "k", "ed-prev-line", NULL);
    el_set(editLine, EL_BIND, "-a", "j", "ed-next-line", NULL);
    /*
     * Source the user's defaults file.
     */
    el_source(editLine, NULL);

    const char *buf;
    int num;
    int ncontinuation;
    while ((buf = el_gets(editLine, &num)) != NULL && num != 0)  {
        int ac, cc, co;
#ifdef DEBUG
        int i;
#endif
        const char **av;
        const LineInfo *li;
        li = el_line(editLine);
#ifdef DEBUG
        (void) fprintf(stderr, "==> got %d %s", num, buf);
        (void) fprintf(stderr, "  > li `%.*s_%.*s'\n",
            (li->cursor - li->buffer), li->buffer,
            (li->lastchar - 1 - li->cursor),
            (li->cursor >= li->lastchar) ? "" : li->cursor);

#endif
        if (gotsig) {
            (void) fprintf(stderr, "Got signal %d.\n", gotsig);
            gotsig = 0;
            el_reset(editLine);
        }

        if (!continuation && num == 1)
            continue;

        ac = cc = co = 0;
        ncontinuation = tok_line(tokenizer, li, &ac, &av, &cc, &co);
        if (ncontinuation < 0) {
            (void) fprintf(stderr, "Internal error\n");
            continuation = 0;
            continue;
        }
#ifdef DEBUG
        (void) fprintf(stderr, "  > nc %d ac %d cc %d co %d\n",
            ncontinuation, ac, cc, co);
#endif
#if 0
        if (continuation) {
            /*
             * Append to the right event in case the user
             * moved around in history.
             */
            if (history(hist, &ev, H_SET, lastevent) == -1)
                err(1, "%d: %s", lastevent, ev.str);
            history(hist, &ev, H_ADD , buf);
        } else {
            history(hist, &ev, H_ENTER, buf);
            lastevent = ev.num;
        }
#else
                /* Simpler */
        history(hist, &ev, continuation ? H_APPEND : H_ENTER, buf);
#endif

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

        if (strcmp(av[0], "history") == 0) {
            int rv;

            switch (ac) {
            case 1:
                for (rv = history(hist, &ev, H_LAST); rv != -1;
                    rv = history(hist, &ev, H_PREV))
                    (void) fprintf(stdout, "%4d %s",
                        ev.num, ev.str);
                break;

            case 2:
                if (strcmp(av[1], "clear") == 0)
                     history(hist, &ev, H_CLEAR);
                else
                     goto badhist;
                break;

            case 3:
                if (strcmp(av[1], "load") == 0)
                     history(hist, &ev, H_LOAD, av[2]);
                else if (strcmp(av[1], "save") == 0)
                     history(hist, &ev, H_SAVE, av[2]);
                break;

            badhist:
            default:
                (void) fprintf(stderr,
                    "Bad history arguments\n");
                break;
            }
        } else if (el_parse(editLine, ac, av) == -1) {
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
                if (wait(&num) == -1)
                    perror("wait");
                (void) fprintf(stderr, "Exit %x\n", num);
                break;
            }
        }

        tok_reset(tokenizer);
    }

    el_end(editLine);
    tok_end(tokenizer);
    history_end(hist);
}

/**
 *
 */
Terminal::~Terminal(void)
{
}
