/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "session.h"
#include "core/core.h"

using namespace gladius::core;

/**
 * Name of the dot directory where session data are stored.
 */
const std::string Session::sDotName = "." PACKAGE_NAME;

/**
 * Access to the session singleton.
 */
Session &
Session::TheSession(void)
{
    static Session *singleton = new Session();
    return *singleton;
}

/**
 *
 */
void
Session::open(void)
{
    try {
        auto home = core::utils::getEnv("HOME");
        mSessionDir = home + core::utils::osPathSep + sDotName;
        // if this doesn't exist, then create it.
        if (!core::utils::fileExists(mSessionDir)) {
            GLADIUS_COUT_STAT << "Creating " + mSessionDir << std::endl;
            int errNo = 0;
            auto status = core::utils::mkDir(mSessionDir, errNo);
            if (GLADIUS_SUCCESS != status) {
                GLADIUS_CERR << "mkDir Failed: "
                             << core::utils::getStrError(errNo)
                             << std::endl;
                return;
            }
        }
    }
    catch (const std::exception &e) {
        GLADIUS_CERR_WARN << "Session creation failed: "
                          << e.what() << std::endl;
    }
}
