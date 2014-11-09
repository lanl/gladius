/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_UTILS_H_INCLUDED
#define GLADIUS_CORE_UTILS_H_INCLUDED

#include "gladius-exception.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include <errno.h>

#define GLADIUS_UNUSED(x)                                                      \
do {                                                                           \
    (void)(x);                                                                 \
} while (0)

/**
 * Convenience macro for throwing an out of resource exception.
 */
#define GLADIUS_THROW_OOR()                                                    \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "Out of Resources"                                               \
          );                                                                   \
} while (0)

/**
 * Convenience macro for throwing an invalid argument exception.
 */
#define GLADIUS_THROW_INVLD_ARG()                                              \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "Invalid argument detected"                                      \
          );                                                                   \
} while (0)

/**
 * Convenience macro for throwing a call failure message.
 */
#define GLADIUS_THROW_CALL_FAILED(msg)                                         \
do {                                                                           \
    throw gladius::core::GladiusException(                                     \
              GLADIUS_WHERE,                                                   \
              "The following call failed: " + std::string(msg)                 \
          );                                                                   \
} while (0)

namespace gladius {
namespace core {

class Utils {
private:
    Utils(void) { ; }

    ~Utils(void) { ; }

public:
    /**
     * Takes an argv-like structure and returns a duplicate that needs to be
     * freed by the caller by calling freeDupArgv.
     */
    static char **
    dupArgv(int argc,
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
};

} // end core namespace
} // end gladius namespace

#endif
