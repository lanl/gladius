/*
 * Copyright (c) 2015-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Gladius Plugin Interface (GPI).
 */

#pragma once

#include "core/args.h"
#include "core/macros.h"
#include "tool-common/tool-common.h"

#include <functional>

// Forward declarations.
namespace MRN {
    class Stream;
    class Network;
} // end namespace MRN

namespace gladius {
namespace gpi {

/**
 * Update when breaking plugin ABI.
 */
#define GLADIUS_PLUGIN_ABI 0

/**
 * The plugin entry poing (symbol name).
 */
#define GLADIUS_PLUGIN_ENTRY_POINT GladiusPluginInfo

/**
 * The name of the plugin entry point.
 */
#define GLADIUS_PLUGIN_ENTRY_POINT_NAME                                        \
GLADIUS_TOSTRING(GLADIUS_PLUGIN_ENTRY_POINT)

#define GLADIUS_PLUGIN(pluginImpl, pluginName, pluginVersion)                  \
extern "C" {                                                                   \
/* Return pointer here because of C linkage... Sigh... */                      \
gladius::gpi::GladiusPlugin *                                                  \
constructPlugin(void) {                                                        \
    static pluginImpl singleton;                                               \
    return &singleton;                                                         \
}                                                                              \
                                                                               \
gladius::gpi::GladiusPluginInfo GLADIUS_PLUGIN_ENTRY_POINT = {                 \
    GLADIUS_PLUGIN_ABI,                                                        \
    pluginName,                                                                \
    pluginVersion,                                                             \
    constructPlugin                                                            \
};                                                                             \
                                                                               \
}

/**
 * A structure that hold arguments that are passed to pluginMain.
 */
struct GladiusPluginArgs {
    //
    std::string myHome;
    //
    gladius::core::Args appArgs;
    // The stream that is setup by the tool FE and handed to the plugin for use
    // for tool front-end <--> tool back-end protocol communication.
    MRN::Stream *protoStream = nullptr;
    //
    MRN::Network *network = nullptr;
    //
    GladiusPluginArgs(void) { ; }
    //
    GladiusPluginArgs(
        const std::string &home,
        const gladius::core::Args &args,
        MRN::Stream *protoStream,
        MRN::Network *mrnetNet
    ) : myHome(home)
      , appArgs(args)
      , protoStream(protoStream)
      , network(mrnetNet) { ; }
    //
    ~GladiusPluginArgs(void) { ; }
};

/**
 * The Gladius Plugin Interface (GPI) interface that plugins must
 * adhere to.
 */
class GladiusPlugin {
public:
    /**
     *
     */
    GladiusPlugin(void) { ; }

    /**
     *
     */
    virtual ~GladiusPlugin(void) { ; }
    //
    virtual void
    pluginMain(
        GladiusPluginArgs &pluginArgs
    ) = 0;
};

/**
 * Exposes plugin info and plugin entry point.
 */
struct GladiusPluginInfo {
    // Plugin ABI.
    int pluginABI;
    // Plugin name.
    const char *pluginName;
    // Plugin version string.
    const char *pluginVersion;
    // Plugin activation.
    std::function<GladiusPlugin *(void)> pluginConstruct;
    //
    GladiusPluginInfo(void)
        : pluginABI(0)
        , pluginName(nullptr)
        , pluginVersion(nullptr)
        , pluginConstruct(nullptr) { ; }
    //
    GladiusPluginInfo(
        int pabi,
        const char *pname,
        const char *pver,
        const std::function<GladiusPlugin *(void)> &pconst
    )   : pluginABI(pabi)
        , pluginName(pname)
        , pluginVersion(pver)
        , pluginConstruct(pconst) { ; }
};

} // end gpi namespace
} // end gladius namespace
