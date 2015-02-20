/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the DSPA.
 */

#include "dspa/core/dsp-manager.h"

#include "core/utils.h"
#include "core/session.h"
#include "core/env.h"

using namespace gladius;
using namespace gladius::core;
using namespace gladius::dspa;


namespace {
// This component's name.
const std::string CNAME = "*dspman";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::GREEN);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)
} // end namespace

/**
 *
 */
bool
DSPManager::pluginPackAvailable(void)
{
    using namespace std;

    VCOMP_COUT("Checking for Availability of " + mTargetModeName + "." << endl);
    // All the paths that we are going to search for the plugin pack. Order
    // matters here. Always push the base location of package installations
    // first. Then push the rest, in the order provided by GLADIUS_PLUGIN_PATH.
    vector<string> searchPaths;
    const auto packagePluginBase = SessionFE::TheSession().execPrefix()
                                 + utils::osPathSep
                                 + "lib";
    // Alwasy first!
    searchPaths.push_back(packagePluginBase);
    // Now get vector of path strings from GLADIUS_PLUGIN_PATH.
    if (!utils::envVarSet(GLADIUS_ENV_PLUGIN_PATH_NAME)) {
        VCOMP_COUT(
            GLADIUS_ENV_PLUGIN_PATH_NAME " Not Set. "
            "No External Plugin Paths to Search..." << std::endl
        );
    }
    else {
        VCOMP_COUT(
            "Adding Paths Found in: " GLADIUS_ENV_PLUGIN_PATH_NAME << endl
        );
        const auto &envStr = utils::getEnv(GLADIUS_ENV_PLUGIN_PATH_NAME);
        const auto &pathVec = utils::strTok(envStr, ":");
        for (const auto &path : pathVec) {
            searchPaths.push_back(path);
        }
    }
    //
    bool pluginBaseFound = false;
    for (const auto &basePath : searchPaths) {
        VCOMP_COUT(
            "Looking for " + mTargetModeName + " in: " + basePath << endl
        );
        pluginBaseFound = utils::fileExists(
                              basePath
                              + utils::osPathSep
                              + mTargetModeName
                          );
        if (pluginBaseFound) break;
    }
    //
    VCOMP_COUT(
        mTargetModeName + " Found: "
        + (pluginBaseFound ? "Yes" : "No") << endl
    );
    return pluginBaseFound;
}
