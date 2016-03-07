/*
 * Copyright (c) 2015-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The gladius plugin manager.
 */

#pragma once

#include "gpa/core/gladius-plugin.h"

#include <cstdint>
#include <string>
#include <map>

namespace gladius {
// The "Gladius Plugin Architecture (GPA) " namespace.
namespace gpa {

/**
 *
 */
struct GladiusPluginPack {
    // IDs of the different kinds of plugin packs that can be returned.
    enum PluginPackType {
        PluginFE = 0,
        PluginBE,
    };
    // ID to name map of required plugins.
    static const std::map<uint8_t, std::string> sRequiredPlugins;
    //
    gpi::GladiusPluginInfo *pluginInfo = nullptr;
};

/**
 * Gladius plugin manager.
 */
class GladiusPluginManager {
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
    GladiusPluginManager(void) { ; }
    //
    ~GladiusPluginManager(void) { ; }
    //
    GladiusPluginManager(
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
    GladiusPluginPack
    getPluginPackFrom(
        GladiusPluginPack::PluginPackType pluginPackType,
        const std::string &validPluginPackPath
    );
    // TODO close handles! Add function to close plugin pack.
};

} // end gpa namespace
} // end gladius namespace
