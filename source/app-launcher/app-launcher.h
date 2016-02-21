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

namespace gladius {
namespace applauncher {

class AppLauncher {
protected:
    // All arguments supplied to launch request.
    core::Args mAppArgs;
    // Target hosts in parallel job.
    toolcommon::Hosts mHosts;

public:
    /**
     *
     */
    AppLauncher(void) { ; }

    /**
     *
     */
    virtual ~AppLauncher(void) { ; }

};

} // end gladius applauncher
} // end gladius namespace
