/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_APPL_APPL_H_INCLUDED
#define GLADIUS_APPL_APPL_H_INCLUDED

#include "core/core.h"

#include <vector>
#include <string>

namespace gladius {
namespace appl {

/**
 * Base Application Launcher (AppL) (virtual) class.
 */
class AppL {
public:
    /**
     *
     */
    AppL(void);

    /**
     *
     */
    virtual ~AppL(void);

    /**
     *
     */
    virtual std::vector<std::string>
    constructLaunchCMD(
        uint32_t totalNProc,
        uint32_t procsPerHost,
        const std::vector<std::string> &appArgv
    ) = 0;
};

} // end appl namespace
} // end gladius namespace

#endif
