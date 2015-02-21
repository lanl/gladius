/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the Domain-specific plugin manager.
 */

#include "dspa/core/dsp-manager.h"

#include "core/utils.h"
#include "core/session.h"
#include "core/env.h"

#include <cassert>
#include <dlfcn.h>

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

// TODO ADD "PluginBackEnd.so"
const std::map<uint8_t, std::string> DSPluginPack::sRequiredPlugins = {
    {PluginFE, "PluginFrontEnd.so"}
};

const std::map<uint8_t, std::string> DSPluginPack::sOptionalPlugins = {
    {PluginFilter, "PluginFilters.so"}
};

/**
 *
 */
bool
DSPManager::pluginPackAvailable(
    std::string &pathToPluginPackIfAvail
) {
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
    std::string fullPackPathIfFound;
    for (const auto &basePath : searchPaths) {
        VCOMP_COUT(
            "Looking for " + mTargetModeName + " in: " + basePath << endl
        );
        fullPackPathIfFound = basePath + utils::osPathSep + mTargetModeName;
        pluginBaseFound = utils::fileExists(fullPackPathIfFound);
        if (pluginBaseFound) break;
    }
    //
    VCOMP_COUT(
        mTargetModeName + " Found: "
        + (pluginBaseFound ? "Yes" : "No") << endl
    );
    // If we found the base directory, now let's check for the usability of the
    // plugin pack directory. Make sure that all the files we care about exist
    // and are actually plugins.
    if (pluginBaseFound) {
        bool looksGood = mPluginPackLooksGood(fullPackPathIfFound);
        // If things look good, then return the path to the plugin pack.
        if (looksGood) pathToPluginPackIfAvail = fullPackPathIfFound;
        return looksGood;
    }
    // Otherwise, it's no use.
    else return false;
}

/**
 *
 */
bool
DSPManager::mPluginPackLooksGood(
    const std::string &pathToPackBase
) {
    using namespace std;

    VCOMP_COUT(
        "Making Sure " + pathToPackBase + " Holds Usable Plugin Pack." << endl;
    );
    // Developer sanity...
    assert(utils::fileExists(pathToPackBase) && "Bogus Path!");
    //
    // We are looking for a few things here.
    // 1. PluginFrontEnd.so - the front-end plugin.
    // 2. PluginBackEnd.so - the back-end plugin.
    // 3. (Optionally) PluginFilters.so - the MRNet filter plugins.
    //
    for (const auto &mapItem : DSPluginPack::sRequiredPlugins) {
        auto exists = utils::fileExists(
            pathToPackBase + utils::osPathSep + mapItem.second
        );
        if (!exists) {
            GLADIUS_CERR << "Cannot find required plugin, " + mapItem.second
                         << ", in " + pathToPackBase + "." << endl;
            return false;
        }
    }
    // TODO add check for optional plugins and note whether or not they exist.
    VCOMP_COUT("Yup. Looks good!" << endl);
    return true;
}

/**
 *
 */
DSPluginPack
DSPManager::getPluginPackFrom(
    const std::string &validPluginPackPath
) {
    DSPluginPack pluginPack;

    // For each required plugin, pull required info and store in plugin pack.
    for (auto &reqPluginMapItem : DSPluginPack::sRequiredPlugins) {
        auto pluginPathStr = validPluginPackPath
                           + utils::osPathSep
                           + reqPluginMapItem.second;
        // Developer sanity...
        assert(utils::fileExists(pluginPathStr) && "Bogus Path!");
        //
        auto *soHandle = dlopen(
            pluginPathStr.c_str(),
            RTLD_LAZY
        );
        if (!soHandle) {
            auto dlerrs = std::string(dlerror());
            auto errs = "dlopen Failed to Open " + pluginPathStr + "." + dlerrs;
            GLADIUS_THROW(errs);
        }
        // Clear errors.
        dlerror();
        //
        dspi::DomainSpecificPluginInfo *pluginInfoHandle = nullptr;
        pluginInfoHandle = (decltype(pluginInfoHandle))dlsym(
            soHandle,
            GLADIUS_PLUGIN_ENTRY_POINT_NAME
        );
        char *dlError = nullptr;
        if (NULL != (dlError= dlerror()))  {
            auto errs = "dlsym Failed While Processing "
                      + pluginPathStr + "." + std::string(dlError);
            GLADIUS_THROW(errs);
        }
        // Now stash in plugin pack.
        pluginPack.pluginInfo[reqPluginMapItem.first] = pluginInfoHandle;
        // TODO close handles! Add function to close plugin pack.
    }
    return pluginPack;
}
