/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_UTILS_H_INCLUDED
#define GLADIUS_CORE_UTILS_H_INCLUDED

#include "core/ret-codes.h"
#include "core/gladius-exception.h"
#include "core/macros.h"

#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

namespace gladius {
namespace core {

class utils {
private:
    //
    utils(void) { ; }
    //
    ~utils(void) { ; }
public:
    /**
     * OS-specific path separator.
     */
    static const std::string osPathSep;
    //
    static char **
    dupArgv(
        int argc,
        const char **argv
    );
    //
    static void
    freeDupArgv(char **dup);

    /**
     * Returns the strerror error string that corresponds to the provided errno.
     */
    static std::string
    getStrError(int errNum)
    {
        return std::string(strerror(errNum));
    }

    /**
     * Leading whitespace stripper for a given string.
     */
    static std::string &
    ltrim(std::string &s) {
        using namespace std;
        s.erase(s.begin(), find_if(s.begin(), s.end(),
                                   not1(ptr_fun<int, int>(isspace))));
        return s;
    }

    /**
     * Trailing whitespace stripper for a given string.
     */
    static std::string &
    rtrim(std::string &s) {
        using namespace std;
        s.erase(find_if(s.rbegin(), s.rend(),
                        not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
    }

    /**
     * Leading and trailing whitespace stripper for a given string.
     */
    static std::string &
    trim(std::string &s)
    {
        return ltrim(rtrim(s));
    }
    //
    static std::string
    getHostname(void);
    //
    static std::string
    getEnv(const std::string &envVarName);
    //
    static void
    setEnv(
        const std::string &envVarName,
        const std::string &value,
        bool overwrite = true
    );
    //
    static int
    unsetEnv(
        const std::string &envVarName,
        int &errNo
    );

    /**
     * Returns whether or not a given environment variable is defined.
     */
    static bool
    envVarSet(const std::string &envVarName)
    {
        return (NULL != getenv(envVarName.c_str()));
    }

    /**
     * Returns whether or not a given path is an absolute path or not.
     */
    static bool
    isAbsolutePath(std::string path)
    {
        // If the path starts with a '/', then it's absolute.
        return std::string(path[0], 1) == utils::osPathSep;
    }
    //
    static int
    which(std::string execName,
          std::string &result);

    /**
     * Returns whether or now a given file exists.
     */
    static bool
    fileExists(const std::string &file)
    {
        return (0 == access(file.c_str(), F_OK));
    }
    //
    static int
    mkDir(
        const std::string &path,
        int &errnoOnFailure
    );
    //
    static int
    getSelfPath(
        std::string &maybeRes,
        int &errnoIfNotSuccess
    );
    //
    template <typename T>
    static int
    getEnvAs(
        const std::string &envString,
        T &as,
        int base = 10
    ) {
        using std::is_same;

        if (!envVarSet(envString)) {
            return GLADIUS_ENV_NOT_SET;
        }
        // Else it's set, so grab its value.
        auto envVal = getEnv(envString);
        if (is_same<T, int>::value) {
            as = std::stoi(envVal, 0, base);
        }
        else if (is_same<T, long>::value) {
            as = std::stol(envVal, 0, base);
        }
        else {
            // Type not supported.
            GLADIUS_THROW("Type case not implemented!");
        }
        return GLADIUS_SUCCESS;
    }
};

} // end core namespace
} // end gladius namespace

#endif
