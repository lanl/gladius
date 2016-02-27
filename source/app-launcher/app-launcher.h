/**
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the interface to our parallel application launcher functionality.
 * This interfaces to things like orte, aprun, srun, etc.
 */
#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "core/ret-codes.h"
#include "core/args.h"
#include "tool-common/tool-common.h"

#include <string>

namespace gladius {
namespace applauncher {

class AppLauncher {
public:
    /**
     * Supported launcher types.
     */
    enum AppLauncherPersonality {
        ORTE, /* orte */
        NONE  /* none specified */
    };

protected:
    // The launcher "personality", i.e. what kind of launcher.
    AppLauncherPersonality mPersonality;
    // All arguments supplied to launch request.
    core::Args mAppArgs;
    // Target hosts in parallel job.
    toolcommon::Hosts mHosts;

public:
    /**
     *
     */
    AppLauncher(
        void
    ) : mPersonality(NONE) { ; }

    /**
     *
     */
    virtual ~AppLauncher(void) { ; }

    /**
     *
     */
    AppLauncherPersonality
    getPersonality(void) { return mPersonality; }

    /**
     *
     */
    std::string
    getPersonalityName(void) {
        switch(mPersonality) {
            // TODO mpich v. open mpi's mpirun?
            case (ORTE): return "orte";
            case (NONE): return "none";
            default: return "???";
        }
    }

    /**
     *
     */
    void
    setPersonality(AppLauncherPersonality p) { mPersonality = p; }

    /**
     * Returns personality based on launcher name.
     */
    static AppLauncherPersonality
    getPersonalityByName(const std::string &name) {
        // TODO deal with all mpiruns
        if ("mpirun" == name) return ORTE;
        else return NONE;
    }
private:
};

} // end gladius applauncher
} // end gladius namespace
