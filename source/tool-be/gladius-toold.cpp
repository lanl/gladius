/*
 * Copyright (c) 2014-2015 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 */

#include "tool-be.h"

#include "core/core.h"
#include "core/session.h"
#include "core/env.h"
#include "tool-be/tool-be.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <map>

#include <sys/types.h>
#include <unistd.h>

namespace {
// This component's name.
const std::string CNAME = "*gtoold";
//
const auto COMPC = gladius::core::colors::NONE;
// CNAME's color code.
const std::string NAMEC = gladius::core::colors::color().ansiBeginColor(COMPC);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
}

////////////////////////////////////////////////////////////////////////////////
namespace toold {

/**
 *
 */
struct Setup {
    bool beVerbose = false;
    std::string logDir = "";
};

/**
 *
 */
std::map<std::string, std::string>
getToolEnvMap(void)
{
    using namespace gladius::core;
    // The list of environment variables that we care about.
    static const std::vector<std::string> envs = {
        GLADIUS_ENV_TOOL_BE_LOG_DIR_NAME,
        GLADIUS_ENV_TOOL_BE_VERBOSE_NAME
    };
    // A map of environment variable names to values.
    std::map<std::string, std::string> theMap;
    // For each env var that we care about, see if its set and if it is then add
    // the key, value to the map.
    for (const auto &env : envs) {
        if (utils::envVarSet(env)) {
            theMap[env] = utils::getEnv(env);
        }
    }
    return theMap;
}

/**
 *
 */
Setup
getSetup(
    const std::map<std::string, std::string> &envMap
) {
    Setup theSetup;
    //
    auto mapi = envMap.find(GLADIUS_ENV_TOOL_BE_LOG_DIR_NAME);
    if (mapi != envMap.end()) {
        theSetup.logDir = mapi->second;
    }
    //
    mapi = envMap.find(GLADIUS_ENV_TOOL_BE_VERBOSE_NAME);
    theSetup.beVerbose = (mapi != envMap.end());
    //
    return theSetup;
}

/**
 *
 */
void
setupLogging(
    const Setup &setup
) {
    using namespace gladius::toolbe;
    try {
        if ("" != setup.logDir) {
            ToolBE::redirectOutputTo(setup.logDir);
        }
    }
    // If things go south here, just catch the exception and return.
    catch (...) {
        return;
    }
}

}

/**
 * Tool daemon main.
 */
int
main(
    int argc,
    char **argv,
    char **envp
) {
    using namespace gladius;
    using namespace gladius::toolbe;
    using namespace toold;
    // Return status.
    int rs = EXIT_SUCCESS;
    //
    try {
        // No stdout buffering.
        setbuf(stdout, NULL);
        // Turn off colors first. They make logs look awful.
        core::colors::color().colorize(false);
        // Get the environment variables really early on because they may change
        // the way in which we behave.
        auto envMap = getToolEnvMap();
        //
        const Setup theSetup = getSetup(envMap);
        // Set verbosity level.
        const auto beVerbose = theSetup.beVerbose;
        // Setup logging.
        setupLogging(theSetup);
        // Let it begin!
        COMP_COUT << "Tool Daemon Started." << std::endl;
        COMP_COUT << "*** PID: " << getpid() << std::endl;
        //
        core::Args args(
            argc,
            const_cast<const char **>(argv),
            const_cast<const char **>(envp)
        );
        toolbe::ToolBE toolBE;
        toolBE.init(args, beVerbose);
        //
        toolBE.connect();
        //
        toolBE.enterPluginMain();
        //
        toolBE.finalize();
    }
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
        rs = EXIT_FAILURE;
    }
    exit(rs);
}
