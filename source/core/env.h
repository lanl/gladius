/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to the environment variable registry that impact gladius' behavior.
 */

#pragma once

#include <string>
#include <vector>
#include <map>

/**
 * NOTE: Environment Variable Naming Convention:
 * GLADIUS_ENV_X_NAME
 */

/**
 * If this environment variable is set, then the tool front-end will be verbose
 * about its actions.
 */
#define GLADIUS_ENV_TOOL_FE_VERBOSE_NAME "GLADIUS_TOOL_FE_VERBOSE"

/**
 * If this environment variable is set, then the tool back-end will be verbose
 * about its actions.
 */
#define GLADIUS_ENV_TOOL_BE_VERBOSE_NAME "GLADIUS_TOOL_BE_VERBOSE"

/**
 * If this environment variable is set, then the tool back-ends will log their
 * output to the specified path. This assumes that all tool processes can reach
 * the specified path and that the path exists and is usable by each process.
 */
#define GLADIUS_ENV_TOOL_BE_LOG_DIR_NAME "GLADIUS_TOOL_BE_LOG_DIR"


/**
 * If this environment variable is set, then the tool will not colorize its
 * terminal output.
 */
#define GLADIUS_ENV_NO_TERM_COLORS_NAME "GLADIUS_NO_TERM_COLORS"

/**
 * A colon-delimited list of paths to search for Gladius plugins.
 */
#define GLADIUS_ENV_PLUGIN_PATH_NAME "GLADIUS_PLUGIN_PATH"

/**
 * Name of the session's default "domain mode" (i.e. domain plugin).
 */
#define GLADIUS_ENV_DOMAIN_MODE_NAME "GLADIUS_DOMAIN_MODE"

namespace gladius {
namespace core {
/**
 *
 */
struct EnvironmentVar {
    // The name of the environment variable.
    std::string envName;
    // A description of what the variable is and does.
    std::string envDesc;
};
/**
 *
 */
class Environment {
    //
    std::map<std::string, std::string> mEnvVarNameCompNameMap;
    //
    std::map< std::string, std::map<std::string, EnvironmentVar> >
        mCompNameVarMap;
    /**
     *
     */
    Environment(void) { ; }
    /**
     *
     */
    ~Environment(void) { ; }

public:
    //
    static Environment &
    TheEnvironment(void);

    /**
     * Disable copy constructor.
     */
    Environment(const Environment &that) = delete;
    //
    Environment &
    operator=(const Environment &other);
    //
    size_t
    addToRegistry(
        const std::string &compName,
        const std::vector<EnvironmentVar> &envVars
    );
    //
    void
    prettyPrint(void);
};

}
}
