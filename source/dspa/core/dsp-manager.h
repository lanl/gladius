/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Domain-Specific Plugin (DSP) manager.
 */

#ifndef GLADIUS_DSPA_DSP_MANAGER_H_INCLUDED
#define GLADIUS_DSPA_DSP_MANAGER_H_INCLUDED

#include <string>

namespace gladius {
namespace dspa {

/**
 * Domain-Specific Plugin (DSP) manager.
 */
class DSPManager {
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose = false;
    //
    std::string mTargetModeName;
    //
    DSPManager(void) { ; }
    //
public:
    //
    ~DSPManager(void) { ; }
    //
    DSPManager(
        const std::string &targetModeName,
        bool beVerbose = false
    ) : mBeVerbose(beVerbose)
      , mTargetModeName(targetModeName) { ; }
    //
    bool
    pluginPackAvailable(void);
    // TODO Get plugin pack.
};


} // end dspa namespace
} // end gladius namespace

#endif
