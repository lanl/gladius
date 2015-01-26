/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to the environment variable registry that impact gladius' behavior.
 */

#ifndef GLADIUS_CORE_ENV_H_INCLUDED
#define GLADIUS_CORE_ENV_H_INCLUDED

#include <string>
#include <vector>
#include <map>

/**
 * If this environment variable is set, then the tool front-end will be verbose
 * about its actions.
 */
#define GLADIUS_TOOL_FE_VERBOSE_STR "GLADIUS_TOOL_FE_VERBOSE"

/**
 * If this environment variable is set, then the tool will not colorize its
 * terminal output.
 */
#define GLADIUS_NO_TERM_COLORS_STR "GLADIUS_NO_TERM_COLORS"

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
    //
    Environment(void) { ; }
    //
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

#endif
