/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"

#include "core/core.h"
#include "ui/ui-factory.h"

#include <string>

using namespace gladius;

/**
 * Name of the dot directory where session data are stored.
 */
const std::string Session::sDotName = "." PACKAGE_NAME;

/**
 *
 */
Session::Session(void)
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

/**
 *
 */
Session::~Session(void)
{
}

/**
 *
 */
Gladius::Gladius(const core::Args &args) {
    try {
        Session currentSession;
        mArgs = args;
        // TODO add parsing and real UI instantiation.
        mUI = ui::UIFactory::getNewUI(mArgs, ui::UIFactory::UI_TERM);
        if (!mUI) GLADIUS_THROW_CALL_FAILED("getNewUI");
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
Gladius::~Gladius(void)
{
    if (mUI) delete mUI;
}

/**
 *
 */
void
Gladius::run(void)
{
    try {
        mUI->interact();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
