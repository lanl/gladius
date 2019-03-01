/*
 * Copyright (c) 2014-2015 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/env.h"

#include "core/core.h"
#include "core/utils.h"

using namespace gladius;
using namespace gladius::core;

/**
 * Access to the environment singleton.
 */
Environment &
Environment::TheEnvironment(void)
{
    static Environment singleton;
    return singleton;
}

/**
 * Just return the singleton.
 */
Environment &
Environment::operator=(const Environment &other) {
    GLADIUS_UNUSED(other);
    return Environment::TheEnvironment();
}

/**
 * Registers the provided environment variables and returns the number of items
 * that were successfully registered.
 */
size_t
Environment::addToRegistry(
    const std::string &compName,
    const std::vector<EnvironmentVar> &envVars
) {
    size_t nRegd = 0;
    for (const auto &var : envVars) {
        // If not already in there
        if (0 == mEnvVarNameCompNameMap.count(var.envName)) {
            // Update environment variable --> component name mapping.
            mEnvVarNameCompNameMap[var.envName] = compName;
            // Add entry to the given component's map of environment variables.
            mCompNameVarMap[compName][var.envName] = var;
            ++nRegd;
        }
    }
    return nRegd;
}

/**
 *
 */
void
Environment::prettyPrint(void)
{
    using namespace std;
    for (const auto &cnvmItem : mCompNameVarMap) {
        cout << "Component: " << cnvmItem.first << endl;
        for (const auto &nameVarMap : cnvmItem.second) {
            const auto &envName = nameVarMap.first;
            const auto &envVar = nameVarMap.second;
            bool set = utils::envVarSet(envName);
            std::string varVal = "[Not Set]";
            if (set) varVal = utils::getEnv(envName);
            cout << envName << ": "
                 << varVal << endl
                 << "Description: " << envVar.envDesc << endl;
        }
        cout << endl;
    }
}
