/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Parallel GDB (pgdb) plugin front-end.
 */

#include "dspa/pgdb/pgdb-common.h"

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
const std::string CNAME = "***pgdb";
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
class PGDBFE : public DomainSpecificPlugin {
    //
    bool mBeVerbose = false;
    //
    DSPluginArgs mDSPluginArgs;
    //
    void
    mEnterMainLoop(void);
    //
    void
    mWaitForBEs(void);
    //
    void
    mLoadFilters(void);
    // TODO rename
    MRN::Stream *mStream = nullptr;

public:
    //
    PGDBFE(void) { ; }
    //
    ~PGDBFE(void) { ; }
    //
    virtual void
    pluginMain(
        DSPluginArgs &pluginArgs
    );
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(PGDBFE, PLUGIN_NAME, PLUGIN_VERSION);

/**
 * Plugin Main.
 */
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void
PGDBFE::pluginMain(
    DSPluginArgs &pluginArgs
) {
    // Set our verbosity level.
    mBeVerbose = core::utils::envVarSet(GLADIUS_ENV_TOOL_FE_VERBOSE_NAME);
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
        // Load our filters.
        mLoadFilters();
        // Setup network.
        mEnterMainLoop();
        //
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
PGDBFE::mLoadFilters(void)
{
    VCOMP_COUT("Loading Filters From: " << mDSPluginArgs.myHome << std::endl);
    // This is the absolute path where this plugin was found.
    static const auto home = mDSPluginArgs.myHome;
    // Path separator.
    static const auto ps = core::utils::osPathSep;
    // FIXME
    static const std::string filterSOName = home + ps + "PluginFilters.so";
    auto *network = mDSPluginArgs.network;
    auto filterID = network->load_FilterFunc(
                        filterSOName.c_str(),
                        "PGDBGDBStringsFilter"
                    );
    if (-1 == filterID) {
        GLADIUS_THROW_CALL_FAILED("load_FilterFunc: " + filterSOName);
    }
    //
    mStream = network->new_Stream(
                  mDSPluginArgs.network->get_BroadcastCommunicator(),
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
 * TODO
 * Make a "proper REPL" with help and all that jive.
 */

/**
 * The front-end REPL that drives the back-end actions.
 */
void
PGDBFE::mEnterMainLoop(void)
{
    VCOMP_COUT("Entering Main Loop." << std::endl);
    // TODO add timeout?
    VCOMP_COUT("Waiting for Back-Ends..." << std::endl);
    toolcommon::feWaitForBEs(mDSPluginArgs.protoStream);
    VCOMP_COUT("Done Waiting for Back-Ends..." << std::endl);
    //
    // At this point all our back-ends have reported that they are ready to go.
    // At this point, all the back-ends are in their main loop and ready to
    // accept commands from us.
    //
    int status = 0;
    std::cout << "(" + CNAME + ") " << std::flush;
    for (std::string line; std::getline(std::cin, line) ; ) {
        if ("Q" == line || "q" == line) {
            std::cout << "Shutting Down..." << std::endl;
            status = mStream->send(pgdb::Exit, "");
            if (-1 == status) {
                GLADIUS_THROW_CALL_FAILED("Stream::Send");
            }
            status = mStream->flush();
            if (-1 == status) {
                GLADIUS_THROW_CALL_FAILED("Stream::Flush");
            }
            break;
        }
        else {
            status = mStream->send(pgdb::ExecCommand, "%s", line.c_str());
            if (-1 == status) {
                GLADIUS_THROW_CALL_FAILED("Stream::Send");
            }
            status = mStream->flush();
            if (-1 == status) {
                GLADIUS_THROW_CALL_FAILED("Stream::Flush");
            }
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
        }
        std::cout << "(" + CNAME + ") " << std::flush;
    }
    //
    VCOMP_COUT("Done with Main Loop." << std::endl);
}
