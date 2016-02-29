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
#include "core/utils.h"
#include "core/hosts.h"

#include <vector>
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
        NONE  /* none specified/unknown */
    };

private:
    // Name of the launcher, e.g. mpirun, srun, aprun
    std::string mName;
    // Absolute that to the launcher.
    std::string mAbsolutePath;
    // The launcher "personality", i.e. what kind of launcher.
    AppLauncherPersonality mPersonality;
    // All arguments supplied to launch request.
    core::Args mAppArgs;
    // Target hosts in parallel job.
    core::Hosts mHosts;

public:
    /**
     *
     */
    AppLauncher(
        void
    ) : mName("")
      , mAbsolutePath("")
      , mPersonality(NONE) { ; }

    /**
     *
     */
    void
    init(const core::Args &args) {
        mAppArgs = args;
        // First argument should be launcher name
        mName = mAppArgs.argv()[0];
        mPersonality = getPersonalityByName(mName);
        //
        if (applauncher::AppLauncher::NONE == mPersonality) {
            static const std::string errs =
                "Cannot determine launcher type by name: '" + mName + "'";
            GLADIUS_THROW(errs);
        }
        //
        auto status =  core::utils::which(mName, mAbsolutePath);
        if (GLADIUS_SUCCESS != status) {
            static const std::string errs =
                "It appears as if " + std::string(mName) + " is either "
                "not installed or not in your $PATH. "
                " Please fix this and try again.";
            GLADIUS_THROW(errs);
        }
    }

    /**
     *
     */
    ~AppLauncher(void) { ; }

    /**
     *
     */
    AppLauncherPersonality
    getPersonality(void) const { return mPersonality; }

    /**
     *
     */
    std::string
    getPersonalityName(void) const {
        switch(mPersonality) {
            // TODO mpich v. open mpi's mpirun? Add a more robust check here.
            case (ORTE): return "orte";
            case (NONE): return "none";
            default: return "???";
        }
    }

    /**
     *
     */
    std::string
    which(void) const { return mAbsolutePath; }

    /**
     * Returns the launch command for the given command given the structure of
     * the provided arguments in mAppArgs.
     *
     * Example:
     * Given mAppArgs: mpirun -n 2 foo --fooArg=3
     *        cmdArgv: baz --bazArg0 --bazArg1
     * Results: mpirun -n 2 baz --bazArg0 --bazArg1
     */
    // TODO FIXME
    core::Args
    getLaunchArgVFor(const std::vector<std::string> &cmdArgv) {
        char **argv = mAppArgs.argv();
        std::vector<std::string> newArgv;
        // Grab launch command
        newArgv.push_back(argv[0]);
        // Then grab the rest of the first bit (launcher arguments) until we
        // reach the application portion of the argument list.
        // The assumption is that all launcher-related arguments will be of the
        // form: -launcherArgName varValue ... app [app args]...
        for (int argi = 1; argi < mAppArgs.argc(); ) {
            if ('-' != argv[argi][0]) break;
            newArgv.push_back(std::string(argv[argi++]));
            newArgv.push_back(std::string(argv[argi++]));
        }
        for (const auto &c : cmdArgv) {
            newArgv.push_back(c);
        }
        return core::Args(newArgv);
    }

    /**
     * Returns personality based on launcher name.
     */
    static AppLauncherPersonality
    getPersonalityByName(const std::string &name) {
        // TODO deal with all mpiruns
        if ("mpirun" == name) return ORTE;
        else return NONE;
    }
};

} // end gladius applauncher
} // end gladius namespace
