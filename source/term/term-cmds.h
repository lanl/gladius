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

#include <string>
#include <iostream>
#include <cstdio>

namespace gladius {
namespace term {

/**
 * Displays help message.
 */
inline void
helpCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args) {
    GLADIUS_UNUSED(args);
    std::cout << PACKAGE_NAME << " help" << std::endl;
}

/**
 * Displays history.
 */
inline void
historyCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args) {
    Terminal *t = args.terminal;
    HistEvent &histEvent = t->getHistEvent();
    for (int rv = history(t->getHistory(), &histEvent, H_LAST);
         rv != -1; rv = history(t->getHistory(), &histEvent, H_PREV)) {
        (void)fprintf(stdout, "%4d %s", histEvent.num, histEvent.str);
    }
}

/**
 * Displays history.
 */
inline void
launchCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args) {
    Terminal *t = args.terminal;
    HistEvent &histEvent = t->getHistEvent();
    for (int rv = history(t->getHistory(), &histEvent, H_LAST);
         rv != -1; rv = history(t->getHistory(), &histEvent, H_PREV)) {
        (void)fprintf(stdout, "%4d %s", histEvent.num, histEvent.str);
    }
}

} // end term namespace
} // end gladius namespace

#endif
