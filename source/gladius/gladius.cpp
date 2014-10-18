/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"
#include "core/core-includes.h"
#include "term/term.h"

#include <string>

using namespace gladius;

/**
 *
 */
Gladius::~Gladius(void)
{
    using namespace gladius;
    if (argv) core::Utils::freeDupArgv(argv);
    if (dbe) delete dbe;
    if (mrnetFE) delete mrnetFE;
    if (term) delete term;
}

/**
 *
 */
void
Gladius::localBody(void)
{
    try {
        term = new term::Terminal(argc, (const char **)argv);
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
Gladius::Gladius(
    int argc,
    const char **argv,
    const char **envp
) {
    using namespace gladius;

    try {
        this->argc = argc;
        this->argv = core::Utils::dupArgv(argc, (char **)argv);
        this->envp = (char **)envp;
        localBody();
#if 0
        dbe = new dbe::GladiusDBE(argc, argv, envp);
        mrnetFE = new mrnet::MRNetFE();
#endif

    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
Gladius::run(void)
{
#if 0
    try {
        dbe->mainLoop();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
#endif
}
