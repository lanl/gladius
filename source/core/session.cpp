/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "session.h"
#include "core/core.h"
#include "core/utils.h"

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
    static bool opened = false;
    static Session singleton;
    // Open the session once.
    if (!opened) {
        // Set this BEFORE and instance actions. Badness will happen if you move
        // this. Don't do it.
        opened = true;
        singleton.mOpen();
    }
    return singleton;
}

/**
 * Just return the singleton.
 */
Session &
Session::operator=(const Session &other) {
    GLADIUS_UNUSED(other);
    return Session::TheSession();
}

/**
 *
 */
void
Session::mOpen(void)
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
        // Is colored output enabled?
        mColorizeSession = !utils::envVarSet(GLADIUS_NO_TERM_COLORS_STR);
    }
    catch (const std::exception &e) {
        GLADIUS_CERR_WARN << "Session creation failed: "
                          << e.what() << std::endl;
    }
}
