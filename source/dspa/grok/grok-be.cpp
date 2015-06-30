/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * grok plugin back-end.
 */

#include "dspa/grok/grok-common.h"

#include "dspa/core/gladius-dspi.h"

#include "core/gladius-exception.h"
#include "core/utils.h"
#include "core/colors.h"
#include "core/env.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <signal.h>

using namespace gladius;
using namespace gladius::dspi;

namespace {
// This component's name.
const std::string CNAME = "grokbe";
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
class GrokBE : public DomainSpecificPlugin {
    //
    bool mBeVerbose = false;
    //
    DSPluginArgs mDSPluginArgs;
    //
    std::map<size_t, std::string> mRankInferiorMap;
    //
    std::map<std::string, size_t> mInferiorRankMap;
    //
    void
    mEnterMainLoop(void);
    //
    void
    mBEReady(void);

public:
    //
    GrokBE(void) { ; }
    //
    ~GrokBE(void) { ; }
    //
    virtual void
    pluginMain(
        DSPluginArgs &pluginArgs
    );
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(GrokBE, PLUGIN_NAME, PLUGIN_VERSION);

/**
 * Plugin Main.
 */
void
GrokBE::pluginMain(
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
        // Enter FE-driven main loop.
        mEnterMainLoop();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    VCOMP_COUT("Exiting Plugin." << std::endl);
}

/**
 *
 */
void
GrokBE::mEnterMainLoop(void)
{
    VCOMP_COUT("Entering Main Loop." << std::endl);
    //
    toolcommon::beReady(mDSPluginArgs.protoStream);
    //
    MRN::PacketPtr packet;
    const bool recvShouldBlock = true;
    // Convenience pointer to network.
    auto *network = mDSPluginArgs.network;
    MRN::Stream *protoStream = nullptr;
    int status = 0;
    auto &pTab = mDSPluginArgs.procTab;
    int action = 0;
    // Do Until the FE Says So...
    do {
        // What action is next FE?
        status = network->recv(&action, packet, &protoStream, recvShouldBlock);
        if (1 != status) GLADIUS_THROW_CALL_FAILED("Network::Recv");
        switch (action) {
            case grok::Shutdown: {
                for (decltype(pTab.nEntries()) p = 0; p < pTab.nEntries(); ++p) {
                    core::utils::sendSignal(pTab.procTab()[p].pd.pid, SIGCONT);
                }
                break;
            }
        }
    } while (action != grok::Shutdown);
    VCOMP_COUT("Done with Main Loop." << std::endl);
}
