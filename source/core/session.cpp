/*
 * Copyright (c) 2014-2015 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "core/session.h"
#include "core/core.h"
#include "core/utils.h"

using namespace gladius::core;

/**
 * Name of the dot directory where session data are stored.
 */
const std::string SessionFE::sDotName = "." PACKAGE_NAME;

/**
 * Access to the session singleton.
 */
SessionFE &
SessionFE::TheSession(void)
{
    static bool opened = false;
    static SessionFE singleton;
    // Open the session once.
    if (!opened) {
        // Set this BEFORE and instance actions. Badness will happen if you move
        // this. Don't do it.
        opened = true;
        singleton.mOpen();
        singleton.init();
    }
    return singleton;
}

/**
 * Just return the singleton.
 */
SessionFE &
SessionFE::operator=(const SessionFE &other) {
    GLADIUS_UNUSED(other);
    return SessionFE::TheSession();
}

/**
 * NOTE: Be careful about using the session singleton from within a tool daemon!
 * A storm of stats will rain down... XXX Maybe we should protect against this.
 */
void
SessionFE::mOpen(void)
{
    try {
        auto home = core::utils::getEnv("HOME");
        mSessionFEDir = home + core::utils::osPathSep + sDotName;
        // if this doesn't exist, then create it.
        if (!core::utils::fileExists(mSessionFEDir)) {
            GLADIUS_COUT_STAT << "Creating " + mSessionFEDir << std::endl;
            int errNo = 0;
            auto status = core::utils::mkDir(mSessionFEDir, errNo);
            if (GLADIUS_SUCCESS != status) {
                GLADIUS_CERR << "mkDir Failed: "
                             << core::utils::getStrError(errNo)
                             << std::endl;
                return;
            }
        }
    }
    catch (const std::exception &e) {
        auto errs = "Session Creation Failed: " + std::string(e.what());
        GLADIUS_THROW_CALL_FAILED(errs);
    }
}

/**
 *
 */
void
SessionFE::mSetExecPrefix(void)
{
    std::string selfPath;
    int errNo = 0;
    auto rc = core::utils::getSelfPath(selfPath, errNo);
    if (GLADIUS_SUCCESS != rc) {
        const auto errs = "getSelfPath: " + core::utils::getStrError(errNo);
        GLADIUS_THROW_CALL_FAILED_RC(errs, rc);
    }
    // Looking for something like: [prefix]/bin/gladius. All we want is to know
    // what [prefix] is.
    std::string badness = "Could not determine " PACKAGE_NAME "'s installation "
                          "prefix by inspecting the following path:"
                          "'" + selfPath + "'";
    std::string last = "/bin/" PACKAGE_NAME;
    auto found = selfPath.rfind(last);
    // Not found, so something is wrong.
    if (std::string::npos == found) {
        GLADIUS_THROW(badness);
    }
    mExecPrefix = selfPath.substr(0, found);
}

/**
 *
 */
void
SessionFE::init(void)
{
    mSetExecPrefix();
}
