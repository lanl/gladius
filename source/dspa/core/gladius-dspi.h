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

#ifndef GLADIUS_DSPA_GLADIUS_DSPI_H_INCLUDED
#define GLADIUS_DSPA_GLADIUS_DSPI_H_INCLUDED

#include <functional>

namespace gladius {
namespace dspi {

/**
 * Update when breaking plugin ABI.
 */
#define GLADIUS_DSP_ABI 0

#define GLADIUS_PLUGIN(pluginImpl, pluginName, pluginVersion)                  \
extern "C" {                                                                   \
                                                                               \
gladius::dspi::DomainSpecificPlugin *                                          \
constructPlugin(void) {                                                        \
    static pluginImpl singleton;                                               \
    return &singleton;                                                         \
}                                                                              \
                                                                               \
gladius::dspi::DomainSpecificPluginInfo GladiusDomainSpecificPlugin = {        \
    GLADIUS_DSP_ABI,                                                           \
    pluginName,                                                                \
    pluginVersion,                                                             \
    constructPlugin                                                            \
};                                                                             \
                                                                               \
}

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
    // XXX do we need this?
    virtual void
    activate(void) = 0;
    // TODO should pass all the things here... args, env, infra, etc.
    virtual void
    mainLoop(void) = 0;
    // XXX and this?
    virtual void
    destroy(void) = 0;
};

/**
 * Exposes plugin info.
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
