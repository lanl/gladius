/*
 * Copyright (c) 2014-2016 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/utils.h"
#include "core/base64.h"

#include <type_traits>
#include <cassert>

#include <sys/stat.h>

using namespace gladius::core;

const std::string utils::osPathSep = "/";

/**
 * Takes an argv-like structure and returns a duplicate that needs to be
 * freed by the caller by calling freeDupArgv.
 */
char **
utils::dupArgv(
    int argc,
    const char **argv
) {
    if (!argv) {
        // Done!
        if (0 == argc) return nullptr;
        // Something is wrong.
        else GLADIUS_THROW_INVLD_ARG();
    }
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
void
utils::freeDupArgv(char **dup)
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
 * Returns a string containing the host name of the calling process.
 */
std::string
utils::getHostname(void)
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
std::string
utils::getEnv(const std::string &envVarName)
{
    char *cEnvP = getenv(envVarName.c_str());
    if (!cEnvP) {
        auto errStr = "getEnv. Why: Value for '" + envVarName
                    + "' requested, but not set.";
        GLADIUS_CERR << errStr << std::endl;
        return std::string("");
    }
    return std::string(cEnvP);
}

/**
 * Wrapper for setenv(3).
 */
int
utils::setEnv(
    const std::string &envVarName,
    const std::string &value,
    bool overwrite
) {
    const int overwriteI = overwrite ? 1 : 0;
    auto rc = setenv(envVarName.c_str(), value.c_str(), overwriteI);
    if (-1 == rc) {
        GLADIUS_CERR << utils::formatCallFailed("setenv(3): ", GLADIUS_WHERE)
                     << std::endl;
        return GLADIUS_ERR;
    }
    return GLADIUS_SUCCESS;
}

/**
 * Wrapper for unsetenv(3).
 */
int
utils::unsetEnv(
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
 * Replicates 'which'.
 * Searches the user's $PATH for a program file.
 */
int
utils::which(std::string execName,
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
    char *fgres = fgets(lineBuff, sizeof(lineBuff), fp);
    if (!fgres) return GLADIUS_ERR;
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
 * Returns the absolute path to the caller's executable.
 */
int
utils::getSelfPath(
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

/**
 * Creates directories like: mkdir PATH.
 */
int
utils::mkDir(
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
 *
 */
std::vector<std::string>
utils::strTok(
    const std::string &theString,
    const std::string &theDelimiters
) {
    using namespace std;

    vector<string> theResult;
    auto *cStr = (char *)theString.c_str();
    auto *cDel = theDelimiters.c_str();

    auto *pch = strtok(cStr, cDel);
    while (pch) {
        theResult.push_back(pch);
        pch = strtok(NULL, cDel);
    }

    return theResult;
}


/**
 * Returns encoded string from string, val.
 */
std::string
utils::base64Encode(
    const std::string &val
) {
    const int encBufLen = Base64encode_len(val.length());
    char *enc = (char *)calloc(encBufLen, sizeof(char));
    if (!enc) {
        GLADIUS_THROW_OOR();
    }
    const int resLen = Base64encode(enc, val.c_str(), val.length());
    std::string result(enc, resLen);
    free(enc);
    return result;
}

/**
 * Returns decoded string from base64 encoded string, val.
 */
std::string
utils::base64Decode(
    const std::string &val
) {
    const int decBufLen = Base64decode_len(val.data());
    char *dec = (char *)calloc(decBufLen, sizeof(char));
    if (!dec) {
        GLADIUS_THROW_OOR();
    }
    const int resLen = Base64decode(dec, val.data());
    std::string result(dec, resLen);
    free(dec);
    return result;
}

/**
 * Returns size (in bytes) of file.
 */
int
utils::getSizeOfFile(
    const std::string &file,
    size_t &outSize
) {
    using namespace std;
    //
    struct stat sbuf;
    if (0 != stat(file.c_str(), &sbuf)) {
        int err = errno;
        const string errs = "stat(2): " + getStrError(err);
        GLADIUS_CERR << formatCallFailed(errs , GLADIUS_WHERE)
                     << std::endl;
        return GLADIUS_ERR_IO;
    }
    outSize = (size_t)sbuf.st_size;
    //
    return GLADIUS_SUCCESS;
}
