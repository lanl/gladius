/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Parallel Step (pstep) plugin back-end.
 */

#include "dspa/pstep/pstep-common.h"

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
const std::string CNAME = "pstepbe";
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
class PStepBE : public DomainSpecificPlugin {
    //
    bool mBeVerbose = false;
    //
    DSPluginArgs mDSPluginArgs;
    //
    void
    mEnterMainLoop(void);
    //
    void
    mBEReady(void);

public:
    //
    PStepBE(void) { ; }
    //
    ~PStepBE(void) { ; }
    //
    virtual void
    pluginMain(
        DSPluginArgs &pluginArgs
    );
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(PStepBE, PLUGIN_NAME, PLUGIN_VERSION);

/**
 * Plugin Main.
 */
void
PStepBE::pluginMain(
    DSPluginArgs &pluginArgs
) {
    // Set our verbosity level.
    mBeVerbose = core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME);
    mBeVerbose = true; // TODO FIXME
    COMP_COUT << "::" << std::endl;
    COMP_COUT << ":: " PLUGIN_NAME " " PLUGIN_VERSION << std::endl;
    COMP_COUT << "::" << std::endl;
    // And so it begins...
    try {
        mDSPluginArgs = pluginArgs;
        VCOMP_COUT("Home: " << mDSPluginArgs.myHome << std::endl);
        if (mBeVerbose) {
            mDSPluginArgs.procTab.dumpTo(std::cout, "[" + CNAME + "] ", COMPC);
        }
        // Setup network.
        mEnterMainLoop();
        //
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    VCOMP_COUT("Exiting Plugin." << std::endl);
    sleep(1000); // TODO RM
}

/**
 *
 */
void
PStepBE::mEnterMainLoop(void)
{
    VCOMP_COUT("Entering Main Loop." << std::endl);
    //
    toolcommon::beReady(mDSPluginArgs.protoStream);
    //
    MRN::PacketPtr packet;
    const bool recvShouldBlock = true;
    int action = 0;
    // Convenience pointer to network.
    auto *network = mDSPluginArgs.network;
    MRN::Stream *protoStream = nullptr;
    int status = 0;
    // Do Until the FE Says So...
    do {
        // What action is next FE?
        status = network->recv(&action, packet, &protoStream, recvShouldBlock);
        if (1 != status) GLADIUS_THROW_CALL_FAILED("Network::Recv");
        //
        switch (action) {
            case pstep::SetBreakPoint: {
                VCOMP_COUT("Action: SetBreakPoint" << std::endl);
                break;
            }
            case pstep::Step: {
                VCOMP_COUT("Action: Step" << std::endl);
                break;
            }
            case pstep::Run: {
                VCOMP_COUT("Action: Run" << std::endl);
                break;
            }
            case pstep::Exit: {
                VCOMP_COUT("Action: Exit" << std::endl);
                break;
            }
            default:
                GLADIUS_CERR << "Received Invalid Action from Front-End!" << std::endl;
                action = pstep::Exit;
        }
    } while (action != pstep::Exit);

    VCOMP_COUT("Done with Main Loop." << std::endl);
}
