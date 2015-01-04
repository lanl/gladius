/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_APPL_APPL_FACTORY_H_INCLUDED
#define GLADIUS_APPL_APPL_FACTORY_H_INCLUDED

#include "appl.h"
#include "appl-orte.h"
#include "core/core.h"

#include <string>

namespace gladius {
namespace appl {

/**
 * Application Launcher (AppL) factory class.
 */
class AppLFactory {
private:
    // Names of supported application launchers.
    static const std::string sORTEName;
    static const std::string sSRUNName;
    static const std::string sAPRUNName;

public:

    /**
     *
     */
    static AppL *
    makeNewAppL(void)
    {
        try {
            auto applEnvStr = core::utils::getEnv(GLADIUS_APP_LAUNCHER_STR);
            if (sORTEName == applEnvStr) {
                return new AppLORTE();
            }
            // Don't know what this is...
            else {
                auto errStr = GLADIUS_APP_LAUNCHER_STR
                            " set to '" + applEnvStr
                            + "' and '" + applEnvStr
                            + "' is not a supported launcher type.";
                GLADIUS_CERR << errStr << std::endl;
                return NULL;
            }
        }
        catch(const std::exception &e) {
            throw core::GladiusException(GLADIUS_WHERE, e.what());
        }
    }

    /**
     *
     */
};

} // end appl namespace
} // end gladius namespace

#endif
