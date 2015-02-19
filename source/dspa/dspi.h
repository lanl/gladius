/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Domain-Specific Plugin (DSP) interface.
 */

#ifndef GLADIUS_DSPA_DSPI_H_INCLUDED
#define GLADIUS_DSPA_DSPI_H_INCLUDED

#include <functional>

namespace gladius {
namespace dspa {

/**
 * Update when breaking plugin ABI.
 */
#define GLADIUS_DSP_ABI 0

#define GLADIUS_PLUGIN(pluginImpl, pluginName, pluginVersion)                  \
extern "C" {                                                                   \
gladius::dspa::DomainSpecificPlugin *                                          \
constructPlugin(void) {                                                        \
    static pluginImpl singleton;                                               \
    return &singleton;                                                         \
}                                                                              \
                                                                               \
gladius::dspa::DomainSpecificPluginInfo GladiusDomainSpecificPlugin = {        \
    GLADIUS_DSP_ABI,                                                           \
    pluginName,                                                                \
    pluginVersion,                                                             \
    constructPlugin                                                            \
};                                                                             \
}

/**
 * The Domain-Specific Plugin (DSP) interface that plugins must adhere to.
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
    activate(void) = 0;
    //
    virtual void
    mainLoop(void) = 0;
    //
    virtual void
    destroy(void) = 0;
};

/**
 * Exposes plugin info.
 */
struct DomainSpecificPluginInfo {
    // Plugin ABI.
    int pluginABI = 0;
    // Plugin name.
    const char *pluginName = nullptr;
    // Plugin version string.
    const char *pluginVersion = nullptr;
    // Plugin activation.
    std::function<void()> pluginConstruct = nullptr;
};

} // end dspa namespace
} // end gladius namespace

#endif
