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
    if (mArgV) core::Utils::freeDupArgv(mArgV);
    if (mDBE) delete mDBE;
    if (mMRNetFE) delete mMRNetFE;
    if (mTerm) delete mTerm;
}

/**
 *
 */
void
Gladius::localBody(void)
{
    try {
        mTerm = new term::Terminal(mArgc, (const char **)mArgV);
        mTerm->enterREPL();
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
        mArgc = argc;
        mArgV = core::Utils::dupArgv(mArgc, (char **)argv);
        mEnvp = (char **)envp;
#if 0
        mDBE = new dbe::GladiusDBE(mArgc, mArgV, mEnvp);
        mMRNetFE = new mrnet::MRNetFE();
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
    try {
        localBody();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
