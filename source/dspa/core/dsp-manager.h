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

#include "dspa/core/gladius-dspi.h"

#include <cstdint>
#include <string>
#include <map>

namespace gladius {
namespace dspa {

/**
 *
 */
struct DSPluginPack {
    // ID to name map of required plugins.
    static const std::map<uint8_t, std::string> sRequiredPlugins;
    // ID to name map of optional plugins.
    static const std::map<uint8_t, std::string> sOptionalPlugins;
    //
    enum RequiredPluginID {
        PluginFE = 0,
        PluginBE,
    };
    //
    enum OptionalPluginID {
        PluginFilter = 0
    };
    //
    std::map<uint8_t, dspi::DomainSpecificPluginInfo *> pluginInfo;
};

/**
 * Domain-Specific Plugin (DSP) manager.
 */
class DSPManager {
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose = false;
    //
    std::string mTargetModeName;
    //
    bool
    mPluginPackLooksGood(
        const std::string &pathToPackBase
    );
public:
    //
    DSPManager(void) { ; }
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
    pluginPackAvailable(
        std::string &pathToPluginPackIfAvail
    );
    //
    DSPluginPack
    getPluginPackFrom(
        const std::string &validPluginPackPath
    );
};

} // end dspa namespace
} // end gladius namespace

#endif
