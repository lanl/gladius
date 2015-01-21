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
    /**
     * Takes an argv-like structure and returns a duplicate that needs to be
     * freed by the caller by calling freeDupArgv.
     */
    static char **
    dupArgv(
        int argc,
        const char **argv
    ) {
        if (!argv) GLADIUS_THROW_INVLD_ARG();
        int i, strLen;
        char **dup = NULL;
        // allocate an extra spot for cap
        dup = (char **)calloc(argc + 1, sizeof(char *));
        if (!dup) {
            GLADIUS_THROW_OOR();
            // Never reached
            return NULL;
        }
        for (i = 0; i < argc; ++i) {
            strLen = strlen(argv[i]) + 1;
            dup[i] = (char *)calloc(strLen, sizeof(char *));
            if (NULL == dup[i]) {
                GLADIUS_THROW_OOR();
                // Never reached
                return NULL;
            }
            (void)memmove(dup[i], argv[i], strLen);
        }
        dup[i] = NULL;
        return dup;
    }

    /**
     * Frees the memory allocated by dupArgv.
     */
    static void
    freeDupArgv(char **dup)
    {
        char **str = NULL;
        if (NULL == dup) return;
        for (str = dup; NULL != *str; ++str) {
            free(*str);
            *str = NULL;
        }
        free(dup);
    }

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

    /**
     * Returns a string containing the host name of the calling process.
     */
    static std::string
    getHostname(void)
    {
        using namespace std;

        char hnBuf[HOST_NAME_MAX];
        (void)memset(hnBuf, '\0', sizeof(hnBuf));
        if (-1 == gethostname(hnBuf, HOST_NAME_MAX)) {
            int err = errno;
            string errStr = "gethostname. Why: " + getStrError(err) + ".";
            GLADIUS_THROW_CALL_FAILED(errStr);
        }
        return string(hnBuf);
    }

    /**
     * Returns a string containing the value of envVarName.
     */
    static std::string
    getEnv(const std::string &envVarName)
    {
        char *cEnvP = getenv(envVarName.c_str());
        if (!cEnvP) {
            auto errStr = "getEnv. Why: Value for '" + envVarName
                        + "' requested, but not set.";
            GLADIUS_THROW_CALL_FAILED(errStr);
        }
        return std::string(cEnvP);
    }

    /**
     * Wrapper for setenv(3).
     */
    static void
    setEnv(
        const std::string &envVarName,
        const std::string &value,
        bool overwrite = true
    ) {
        int overwriteI = overwrite ? 1 : 0;
        auto rc = setenv(envVarName.c_str(), value.c_str(), overwriteI);
        if (-1 == rc) {
            GLADIUS_THROW_CALL_FAILED("setEnv");
        }
    }

    /**
     * Wrapper for unsetenv(3).
     */
    static int
    unsetEnv(
        const std::string &envVarName,
        int &errNo
    ) {
        auto rc = unsetenv(envVarName.c_str());
        if (-1 == rc) {
            errNo = errno;
            return GLADIUS_ERR;
        }
        return GLADIUS_SUCCESS;
    }

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

    /**
     * Replicates 'which'.
     * Searches the user's $PATH for a program file.
     */
    static int
    which(std::string execName,
          std::string &result)
    {
        std::string cmdString = "which " + execName;
        FILE *fp = popen(cmdString.c_str(), "r");
        if (!fp) {
            return GLADIUS_ERR_IO;
        }
        // Else capture the output. Should either be the path or nothing on
        // first line.
        char lineBuff[PATH_MAX];
        fgets(lineBuff, sizeof(lineBuff), fp);
        auto pcrc = pclose(fp);
        // The exit status of this will tell us whether or not our command
        // succeeded or not. If success, then finish prepping the answer.
        if (0 != WEXITSTATUS(pcrc)) {
            return GLADIUS_ERR;
        }
        // Stash into result string.
        result = lineBuff;
        // Trim
        trim(result);
        // Remove newline from end of string.
        result.erase(std::remove(result.begin(), result.end(), '\n'),
                     result.end());
        return GLADIUS_SUCCESS;
    }

    /**
     * Returns whether or now a given file exists.
     */
    static bool
    fileExists(const std::string &file)
    {
        return (0 == access(file.c_str(), F_OK));
    }

    /**
     * Creates directories like: mkdir PATH.
     */
    static int
    mkDir(
        const std::string &path,
        int &errnoOnFailure
    ) {
        errnoOnFailure = 0;
        auto status = mkdir(path.c_str(),
                            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
                      );
        if (0 != status) {
            errnoOnFailure = errno;
            return GLADIUS_ERR;
        }
        return GLADIUS_SUCCESS;
    }

    /**
     * Returns the absolute path to the caller's executable.
     */
    static int
    getSelfPath(
        std::string &maybeRes,
        int &errnoIfNotSuccess
    ) {
        char buff[PATH_MAX];
        (void)memset(buff, '\0', sizeof(buff));
        auto len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
        if (len != -1) {
            buff[len] = '\0';
            maybeRes = std::string(buff);
            return GLADIUS_SUCCESS;
        }
        else {
            errnoIfNotSuccess = errno;
            return GLADIUS_ERR;
        }
    }
};

} // end core namespace
} // end gladius namespace

#endif
