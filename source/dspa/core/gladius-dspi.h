/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Domain-Specific Plugin Interface (DSPI).
 */

#ifndef GLADIUS_DSPA_GLADIUS_DSPI_H_INCLUDED
#define GLADIUS_DSPA_GLADIUS_DSPI_H_INCLUDED

#include "core/args.h"
#include "core/macros.h"
#include "tool-common/tool-common.h"

#include <functional>

namespace gladius {
namespace dspi {

/**
 * Update when breaking plugin ABI.
 */
#define GLADIUS_DSP_ABI 0

/**
 * The plugin entry poing (symbol name).
 */
#define GLADIUS_PLUGIN_ENTRY_POINT GladiusDomainSpecificPluginInfo

/**
 * The name of the plugin entry point.
 */
#define GLADIUS_PLUGIN_ENTRY_POINT_NAME                                        \
GLADIUS_TOSTRING(GLADIUS_PLUGIN_ENTRY_POINT)

#define GLADIUS_PLUGIN(pluginImpl, pluginName, pluginVersion)                  \
extern "C" {                                                                   \
/* Return pointer here because of C linkage... Sigh... */                      \
gladius::dspi::DomainSpecificPlugin *                                          \
constructPlugin(void) {                                                        \
    static pluginImpl singleton;                                               \
    return &singleton;                                                         \
}                                                                              \
                                                                               \
gladius::dspi::DomainSpecificPluginInfo GLADIUS_PLUGIN_ENTRY_POINT = {         \
    GLADIUS_DSP_ABI,                                                           \
    pluginName,                                                                \
    pluginVersion,                                                             \
    constructPlugin                                                            \
};                                                                             \
                                                                               \
}

/**
 * A structure that hold arguments that are passed to pluginMain.
 */
struct DSPluginArgs {
    //
    std::string myHome;
    //
    gladius::core::Args appArgs;
    //
    gladius::toolcommon::ProcessTable procTab;
    // The stream that is setup by the tool FE and handed to the plugin for use
    // for tool front-end <--> tool back-end protocol communication.
    MRN::Stream *protoStream = nullptr;
    //
    MRN::Network *network = nullptr;
    //
    DSPluginArgs(void) { ; }
    //
    DSPluginArgs(
        const std::string &home,
        const gladius::core::Args &args,
        const gladius::toolcommon::ProcessTable &pTab,
        MRN::Stream *protoStream,
        MRN::Network *mrnetNet
    ) : myHome(home)
      , appArgs(args)
      , procTab(pTab)
      , protoStream(protoStream)
      , network(mrnetNet) { ; }
    //
    ~DSPluginArgs(void) { ; }
};

/**
 * The Domain-Specific Plugin Interface (DSPI) interface that plugins must
 * adhere to.
 */
class DomainSpecificPlugin {
public:
    /**
     *
     */
    DomainSpecificPlugin(void) { ; }

    /**
     *
     */
    virtual ~DomainSpecificPlugin(void) { ; }
    //
    virtual void
    pluginMain(
        DSPluginArgs &pluginArgs
    ) = 0;
};

/**
 * Exposes plugin info and plugin entry point.
 */
struct DomainSpecificPluginInfo {
    // Plugin ABI.
    int pluginABI;
    // Plugin name.
    const char *pluginName;
    // Plugin version string.
    const char *pluginVersion;
    // Plugin activation.
    std::function<DomainSpecificPlugin *(void)> pluginConstruct;
    //
    DomainSpecificPluginInfo(void)
        : pluginABI(0)
        , pluginName(nullptr)
        , pluginVersion(nullptr)
        , pluginConstruct(nullptr) { ; }
    //
    DomainSpecificPluginInfo(
        int pabi,
        const char *pname,
        const char *pver,
        const std::function<DomainSpecificPlugin *(void)> &pconst
    )   : pluginABI(pabi)
        , pluginName(pname)
        , pluginVersion(pver)
        , pluginConstruct(pconst) { ; }
};

} // end dspi namespace
} // end gladius namespace

#endif
