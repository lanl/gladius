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

#ifndef GLADIUS_TERM_TERM_CMDS_H_INCLUDED
#define GLADIUS_TERM_TERM_CMDS_H_INCLUDED

#include "term.h"

#include "core/utils.h"

#include <string>
#include <iostream>

#include <locale.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius::term;

/**
 *
 */
void
helpCMDCallback(const Terminal::EvalInputCmdCallBackArgs &args) {
    GLADIUS_UNUSED(args);
    std::cout << PACKAGE_NAME << " help" << std::endl;
}

/**
 * Map between command name and callback function.
 */
std::map<std::string, void (*)(const Terminal::EvalInputCmdCallBackArgs &)>
Terminal::sEvalCMDMap = {
    {"help", helpCMDCallback},
    {"?", helpCMDCallback}
};

#endif
