/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"

#include "core/core.h"

#include <string>

using namespace gladius;

/**
 *
 */
Gladius::~Gladius(void)
{
    if (mArgV) core::Utils::freeDupArgv(mArgV);
    if (mUI) delete mUI;
}

/**
 *
 */
Gladius::Gladius(
    int argc,
    const char **argv,
    const char **envp
) {
    try {
        mArgc = argc;
        mArgV = core::Utils::dupArgv(mArgc, (char **)argv);
        mEnvp = (char **)envp;
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
        mUI->interact();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
