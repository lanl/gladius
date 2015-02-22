/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Parallel Step (pstep) plugin front-end.
 */

#include "dspa/core/gladius-dspi.h"

#include "core/utils.h"
#include "core/colors.h"
#include "core/env.h"

#include <iostream>

using namespace gladius;
using namespace gladius::dspi;

namespace {
// This component's name.
const std::string CNAME = "pstepfe";
//
const auto COMPC = core::colors::MAGENTA;
// CNAME's color code.
const std::string NAMEC = core::colors::color().ansiBeginColor(COMPC);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)
} // end namespace

class PStepFE : public DomainSpecificPlugin {
    bool mBeVerbose = false;
public:
    //
    PStepFE(void) { ; }
    //
    ~PStepFE(void) { ; }
    //
    virtual void
    pluginMain(
        const core::Args &appArgs,
        const toolcommon::ProcessTable &procTab
    );
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(PStepFE, "pstep", "0.0.1");

/**
 * Plugin Main.
 */
void
PStepFE::pluginMain(
    const core::Args &appArgs,
    const toolcommon::ProcessTable &procTab
) {
    mBeVerbose = core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME);
    VCOMP_COUT("Entering Main" << std::endl);
    procTab.dumpTo(std::cout, "[" + CNAME + "] ", COMPC);
}
