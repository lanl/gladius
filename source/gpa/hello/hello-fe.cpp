/*
 * Copyright (c) 2015-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The "hello world" plugin front-end.
 */

#include "gpa/hello/hello-common.h"

#include "gpa/core/gpi.h"

#include "core/core.h"
#include "core/utils.h"
#include "core/colors.h"

#include <iostream>

using namespace gladius;
using namespace gladius::gpi;

namespace {
// This component's name.
const std::string CNAME = "hello";
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
class HelloFE : public GladiusPlugin {
    //
    bool mBeVerbose = false;
    //
    GladiusPluginArgs mGladiusPluginArgs;
    //
    void
    mEnterMainLoop(void);
    //
    void
    mWaitForBEs(void);
    // TODO rename
    MRN::Stream *mStream = nullptr;

public:
    //
    HelloFE(void) { ; }
    //
    ~HelloFE(void) { ; }
    //
    virtual void
    pluginMain(
        GladiusPluginArgs &pluginArgs
    );
    //
    void
    mLoadFilters(void);
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(HelloFE, PLUGIN_NAME, PLUGIN_VERSION)

/**
 *
 */
void
HelloFE::mLoadFilters(void)
{
    VCOMP_COUT("Loading Filters From: " << mGladiusPluginArgs.myHome << std::endl);
    // This is the absolute path where this plugin was found.
    static const auto home = mGladiusPluginArgs.myHome;
    // Path separator.
    static const auto ps = core::utils::osPathSep;
    // FIXME
    static const std::string filterSOName = home + ps + "PluginFilters.so";
    auto *network = mGladiusPluginArgs.network;
    auto filterID = network->load_FilterFunc(
                        filterSOName.c_str(),
                        "HelloStringsFilter"
                    );
    if (-1 == filterID) {
        GLADIUS_THROW_CALL_FAILED("load_FilterFunc: " + filterSOName);
    }
    //
    mStream = network->new_Stream(
                  mGladiusPluginArgs.network->get_BroadcastCommunicator(),
                  MRN::SFILTER_WAITFORALL,
                  filterID
              );
    if (!mStream) {
        GLADIUS_THROW_CALL_FAILED("new_Stream");
    }
    //
    VCOMP_COUT("Done Loading Filters." << std::endl);
}

/**
 * Plugin Main.
 */
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void
HelloFE::pluginMain(
    GladiusPluginArgs &pluginArgs
) {
    // Set our verbosity level.
    mBeVerbose = core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME);
    COMP_COUT << "::" << std::endl;
    COMP_COUT << ":: " PLUGIN_NAME " " PLUGIN_VERSION << std::endl;
    COMP_COUT << "::" << std::endl;
    // And so it begins...
    try {
        mGladiusPluginArgs = pluginArgs;
        VCOMP_COUT("Home: " << mGladiusPluginArgs.myHome << std::endl);
        // Load Simple Filter
        mLoadFilters();
        // Setup network.
        mEnterMainLoop();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    VCOMP_COUT("Exiting Plugin." << std::endl);
}

/**
 * The front-end REPL that drives the back-end actions.
 */
void
HelloFE::mEnterMainLoop(void)
{
    VCOMP_COUT("Waiting for Back-Ends..." << std::endl);
    toolcommon::feWaitForBEs(mGladiusPluginArgs.protoStream);
    
    //
    // At this point all our back-ends have reported that they are ready to go.
    // At this point, all the back-ends are in their main loop and ready to
    // accept commands from us.
    //
    int status = 0;
    try {
        std::cout << "(" + CNAME + ") say hello, back-ends!" << std::endl;
        status = mStream->send(hello::SayHello, "");
        if (-1 == status) {
            GLADIUS_THROW_CALL_FAILED("Stream::Send");
        }
        status = mStream->flush();
        if (-1 == status) {
            GLADIUS_THROW_CALL_FAILED("Stream::Flush");
        }
        //
        int tag;
        MRN::PacketPtr packet;
        status = mStream->recv(&tag, packet);
        if (-1 == status) {
            GLADIUS_THROW_CALL_FAILED("Stream::Recv");
        }
        char *out = nullptr;
        status = packet->unpack("%s", &out);
        std::cout << out << std::endl;
        free(out);
        //
        std::cout << "(" + CNAME + ") say goodbye, back-ends!" << std::endl;
        status = mStream->send(hello::Shutdown, "");
        if (-1 == status) {
            GLADIUS_THROW_CALL_FAILED("Stream::Send");
        }
        status = mStream->flush();
        if (-1 == status) {
            GLADIUS_THROW_CALL_FAILED("Stream::Flush");
        }
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    VCOMP_COUT("Done with Main Loop." << std::endl);
}
