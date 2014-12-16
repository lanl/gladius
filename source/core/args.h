/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_ARGS_H_INCLUDED
#define GLADIUS_CORE_ARGS_H_INCLUDED

#include "core.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

namespace gladius {
namespace core {

class Args {
private:
    int mArgC = 0;
    char **mArgV = nullptr;
    char **mEnv = nullptr;

public:
    /**
     *
     */
    Args(void) { ; }

    /**
     *
     */
    Args(int argc,
         const char **argv,
         const char **envp = nullptr
    ) {
        mArgC = argc;
        mArgV = core::Utils::dupArgv(argc, argv);
        mEnv = const_cast<char **>(envp);
    }

    /**
     *
     */
    ~Args(void)
    {
        if (mArgV) core::Utils::freeDupArgv(mArgV);
    }

    /**
     *
     */
    Args(const Args &other)
    {
        mArgC = other.mArgC;
        mArgV = core::Utils::dupArgv(other.argc(), (const char **)other.argv());
        mEnv = other.mEnv;
    }

    /**
     *
     */
    Args &
    operator=(const Args &other)
    {
        mArgC = other.mArgC;
        mArgV = core::Utils::dupArgv(other.argc(), (const char **)other.argv());
        mEnv = other.mEnv;
        return *this;
    }

    /**
     *
     */
    int
    argc(void) const
    {
        return mArgC;
    }

    /**
     *
     */
    char **
    argv(void) const
    {
        return mArgV;
    }

    /**
     *
     */
    char **
    envp(void) const
    {
        return mEnv;
    }
};

} // end core namespace
} // end gladius namespace

#endif
