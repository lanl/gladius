/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include "core/utils.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

namespace gladius {
namespace core {

class Args {
private:
    //
    int mArgC = 0;
    //
    char **mArgV = nullptr;
    //
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
        mArgV = core::utils::dupArgv(argc, argv);
        mEnv = const_cast<char **>(envp);
    }

    /**
     *
     */
    ~Args(void)
    {
        if (mArgV) core::utils::freeDupArgv(mArgV);
    }

    /**
     *
     */
    Args(const Args &other)
    {
        mArgC = other.mArgC;
        mArgV = core::utils::dupArgv(other.argc(), (const char **)other.argv());
        mEnv = other.mEnv;
    }

    /**
     *
     */
    Args &
    operator=(const Args &other)
    {
        mArgC = other.mArgC;
        mArgV = core::utils::dupArgv(other.argc(), (const char **)other.argv());
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
