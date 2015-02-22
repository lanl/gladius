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

#include "core/gladius-exception.h"
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

/**
 *
 */
class PStepFE : public DomainSpecificPlugin {
    // Where I was found. The base of my plugin pack.
    std::string mHome;
    // The process table of this job.
    toolcommon::ProcessTable mProcTab;
    //
    bool mBeVerbose = false;
    // MRNet network instance pointer.
    MRN::Network *mNetwork = nullptr;
    //
    MRN::Communicator *mBcastComm = nullptr;
    //
    MRN::Stream *mBcastStream = nullptr;
    //
    void
    mNetworkSetup(void);

public:
    //
    PStepFE(void) { ; }
    //
    ~PStepFE(void) { ; }
    //
    virtual void
    pluginMain(
        const std::string &myHome,
        const core::Args &appArgs,
        const toolcommon::ProcessTable &procTab,
        MRN::Network &mrnetNetwork
    );
};

// The plugin's name.
#define PLUGIN_NAME "pstep"
// The plugin's version string.
#define PLUGIN_VERSION "0.0.1"

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(PStepFE, PLUGIN_NAME, PLUGIN_VERSION);

/**
 * Plugin Main.
 */
void
PStepFE::pluginMain(
    const std::string &myHome,
    const core::Args & /* Application Args */,
    const toolcommon::ProcessTable &procTab,
    MRN::Network &mrnetNetwork
) {
    // Set our verbosity level.
    mBeVerbose = core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME);
    COMP_COUT << "::" << std::endl;
    COMP_COUT << ":: " PLUGIN_NAME " " PLUGIN_VERSION << std::endl;
    COMP_COUT << "::" << std::endl;
    // And so it begins...
    try {
        mHome = myHome;
        mProcTab = procTab;
        mNetwork = &mrnetNetwork;
        VCOMP_COUT("Home: " << mHome << std::endl);
        if (mBeVerbose) {
            procTab.dumpTo(std::cout, "[" + CNAME + "] ", COMPC);
        }
        // Setup network.
        mNetworkSetup();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    VCOMP_COUT("Exiting Plugin." << std::endl);
}

/**
 *
 */
void
PStepFE::mNetworkSetup(void)
{
    VCOMP_COUT("Starting Network Setup." << std::endl);

    mBcastComm = mNetwork->get_BroadcastCommunicator();
    if (!mBcastComm) {
        GLADIUS_THROW_CALL_FAILED("get_BroadcastCommunicator");
    }

    mBcastStream = mNetwork->new_Stream(
                       mBcastComm,
                       MRN::SFILTER_WAITFORALL,
                       0 /* TODO */
                   );
    if (!mBcastStream) {
        GLADIUS_THROW_CALL_FAILED("new_Stream");
    }

    VCOMP_COUT("Done With Network Setup." << std::endl);
}
