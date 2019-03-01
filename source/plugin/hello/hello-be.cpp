/*
 * Copyright (c) 2015-2016 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The "hello world" plugin back-end.
 */

#include "plugin/hello/hello-common.h"

#include "plugin/core/gladius-plugin.h"

#include "core/core.h"
#include "core/utils.h"
#include "core/colors.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <signal.h>

#include "mrnet/MRNet.h"

using namespace gladius;
using namespace gladius::gpi;

namespace {
// This component's name.
const std::string CNAME = "hellobe";
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
class HelloBE : public GladiusPlugin {
    //
    bool mBeVerbose = false;
    //
    GladiusPluginArgs mGladiusPluginArgs;
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
    HelloBE(void) { ; }
    //
    ~HelloBE(void) { ; }
    //
    virtual void
    pluginMain(
        GladiusPluginArgs &pluginArgs
    );
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(HelloBE, PLUGIN_NAME, PLUGIN_VERSION)

/**
 * Plugin Main.
 */
void
HelloBE::pluginMain(
    GladiusPluginArgs &pluginArgs
) {
    // Set our verbosity level.
    mBeVerbose = core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME);
    mBeVerbose = true; // TODO FIXME
    COMP_COUT << "::" << std::endl;
    COMP_COUT << ":: " PLUGIN_NAME " " PLUGIN_VERSION << std::endl;
    COMP_COUT << "::" << std::endl;
    // And so it begins...
    try {
        mGladiusPluginArgs = pluginArgs;
        VCOMP_COUT("Home: " << mGladiusPluginArgs.myHome << std::endl);
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
HelloBE::mEnterMainLoop(void)
{
    VCOMP_COUT("Entering Main Loop." << std::endl);
    // TODO
    //toolcommon::beReady(mGladiusPluginArgs.protoStream);
    //
    MRN::PacketPtr packet;
    const bool recvShouldBlock = true;
    // Convenience pointer to network.
    auto *network = mGladiusPluginArgs.network;
    MRN::Stream *protoStream = nullptr;
    int status = 0;
    int action = 0;
    // Do Until the FE Says So...
    do {
        // What action is next FE?
        status = network->recv(&action, packet, &protoStream, recvShouldBlock);
        if (1 != status) GLADIUS_THROW_CALL_FAILED("Network::Recv");
        switch (action) {
            case hello::SayHello:
            {
                const char *hello = "hello";
                status = protoStream->send(action, "%s", hello); 
                if (-1 == status) {
                    GLADIUS_THROW_CALL_FAILED("Stream::Send");
                }
                status = protoStream->flush();
                if (-1 == status) {
                    GLADIUS_THROW_CALL_FAILED("Stream::Flush");
                }
                break;
            }
            case hello::Shutdown: {
                break;
            }
        }
    } while (action != hello::Shutdown);
    VCOMP_COUT("Done with Main Loop." << std::endl);
}
