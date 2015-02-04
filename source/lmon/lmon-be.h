/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Back-End (BE) API.
 */

#ifndef GLADIUS_TOOL_BE_LMON_H_INCLUDED
#define GLADIUS_TOOL_BE_LMON_H_INCLUDED

#include "core/core.h"
#include "core/args.h"
#include "tool-common/tool-common.h"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

#include "lmon_api/lmon_be.h"
#include "lmon_api/lmon_proctab.h"

namespace gladius {
namespace lmonbe {
// Type signature of the back-end unpacking routine.
typedef int (*FEToBEUnpackFnP)(void *, int, void *);

class LaunchMonBE {
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose = false;
    // A copy of the arguments passed during init.
    core::Args mArgs;
    // A handle to the FE to BE function.
    FEToBEUnpackFnP mFEToBEUnpackFn = nullptr;

    ////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ////////////////////////////////////////////////////////////////////////////
public:
    //
    LaunchMonBE(void);
    //
    ~LaunchMonBE(void);
    //
    void
    init(
        const core::Args &args,
        bool beVerbose = false
    );
    //
    void
    regUnpackForFEToBE(
        lmonbe::FEToBEUnpackFnP funp
    );
    //
    void
    handshake(void);

    /**
     * Sets whether or not LaunchMON operations will be verbose.
     */
    void
    verbose(bool bVerbose) {
        mBeVerbose = bVerbose;
    }
    //
    void
    createAndPopulateProcTab(
        toolcommon::ProcessTable &procTab
    );

};

} // end lmonbe namespace
} // end gladius namespace

#endif
