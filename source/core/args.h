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
#include <vector>
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
    //
    void
    mNewArgvFrom(const std::vector<std::string> &argv)
    {
        // Free up previously allocated argv.
        if (mArgV) core::utils::freeDupArgv(mArgV);
        //
        mArgC = argv.size();
        char **tmpArgv = (char **)calloc(mArgC, sizeof(char *));
        if (!tmpArgv) GLADIUS_THROW_OOR();
        for (int argi = 0; argi < mArgC; ++argi) {
            tmpArgv[argi] = (char *)argv[argi].c_str();
        }
        mArgV = core::utils::dupArgv(mArgC, (const char **)tmpArgv);
        // Done with this
        free(tmpArgv);
    }

public:
    /**
     *
     */
    Args(void) = default;

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
     * Same as above, just initialize from vector of strings.
     */
    Args(const std::vector<std::string> &argv) {
        mNewArgvFrom(argv);
    }

    /**
     *
     */
    void
    argvAppend(const Args &args)
    {
        // Construct new vector of args.
        auto a0 = toArgv();
        auto a1 = args.toArgv();
        a0.insert(std::end(a0), std::begin(a1), std::end(a1));
        mNewArgvFrom(a0);
    }

    /**
     *
     */
    ~Args(void)
    {
        mArgC = 0;
        if (mArgV) {
            core::utils::freeDupArgv(mArgV);
            mArgV = nullptr;
        }
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
    std::vector<std::string>
    toArgv(void) const
    {
        std::vector<std::string> res;
        for (int i = 0; i < mArgC; ++i) {
            res.push_back(mArgV[i]);
        }
        return res;
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
