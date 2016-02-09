/*
 * Copyright (c) 2015-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Domain-Specific Plugin (DSP) manager.
 */

#pragma once

#include "dspa/core/gladius-dspi.h"

#include <cstdint>
#include <string>
#include <map>

namespace gladius {
// The "Domain-Specific Plugin Architecture (DSPA) " namespace.
namespace dspa {

/**
 *
 */
struct DSPluginPack {
    // IDs of the different kinds of plugin packs that can be returned.
    enum PluginPackType {
        PluginFE = 0,
        PluginBE,
    };
    // ID to name map of required plugins.
    static const std::map<uint8_t, std::string> sRequiredPlugins;
    //
    dspi::DomainSpecificPluginInfo *pluginInfo = nullptr;
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
        DSPluginPack::PluginPackType pluginPackType,
        const std::string &validPluginPackPath
    );
    // TODO close handles! Add function to close plugin pack.
};

} // end dspa namespace
} // end gladius namespace
