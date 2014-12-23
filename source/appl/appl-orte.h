/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_APPL_APPL_ORTE_H_INCLUDED
#define GLADIUS_APPL_APPL_ORTE_H_INCLUDED

#include "appl.h"
#include "core/core.h"

#include <string>
#include <vector>

namespace gladius {
namespace appl {

/**
 * Base Application Launcher (AppL) (virtual) class.
 */
class AppLORTE : public AppL {
public:
    virtual std::vector<std::string>
    constructLaunchCMD(
        uint32_t totalNProc,
        uint32_t procsPerHost,
        const std::vector<std::string> &appArgv
    ) { ; }
};

} // end appl namespace
} // end gladius namespace

#endif
