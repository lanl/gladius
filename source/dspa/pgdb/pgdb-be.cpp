/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Parallel GDB (pgdb) plugin back-end.
 */

#include "dspa/pgdb/pgdb-common.h"

#include "dspa/core/gladius-dspi.h"

#include "core/gladius-exception.h"
#include "core/utils.h"
#include "core/colors.h"
#include "core/env.h"

#include "dmi/dmi.h"

#include <iostream>
#include <cstdlib>

#include <signal.h>

using namespace gladius;
using namespace gladius::dspi;

namespace {
// This component's name.
const std::string CNAME = "*pgdbbe";
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
class PGDBBE : public DomainSpecificPlugin {
    //
    bool mBeVerbose = false;
    //
    dmi::DMI debugger;
    //
    DSPluginArgs mDSPluginArgs;
    //
    void
    mAttachToTarget(void);
    //
    void
    mEnterMainLoop(void);
    //
    void
    mBEReady(void);

public:
    //
    PGDBBE(void) { ; }
    //
    ~PGDBBE(void) { ; }
    //
    virtual void
    pluginMain(
        DSPluginArgs &pluginArgs
    );
};

/**
 * Plugin registration.
 */
GLADIUS_PLUGIN(PGDBBE, PLUGIN_NAME, PLUGIN_VERSION);

/**
 * Plugin Main.
 */
void
PGDBBE::pluginMain(
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
        //
        mAttachToTarget();
        // Enter FE-driven main loop.
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
PGDBBE::mAttachToTarget(void)
{
    VCOMP_COUT("Attaching." << std::endl);
    //
    debugger.init(mBeVerbose);
    //
    auto &pTab = mDSPluginArgs.procTab;
    auto *pt = pTab.procTab();
    // TODO Deal with gdb of multiple processes! Do we care? Legion will only
    // have one? But MPI apps will care. PGDB has an example (I think).
    for (decltype(pTab.nEntries()) p = 0; p < pTab.nEntries(); ++p) {
        // Attach to PID
        debugger.attach(pt[p].pd.pid);
        //
        //kill(pt[p].pd.pid, SIGCONT);
        //
    }
    //
    VCOMP_COUT("Done Attaching." << std::endl);
}

/**
 *
 */
void
PGDBBE::mEnterMainLoop(void)
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
    // TODO FIXME Show. Just throw away for now...
    { std::string tmp; debugger.recvResp(tmp); }
    // Do Until the FE Says So...
    do {
        // What action is next FE?
        status = network->recv(&action, packet, &protoStream, recvShouldBlock);
        if (1 != status) GLADIUS_THROW_CALL_FAILED("Network::Recv");
        //
        switch (action) {
            case pgdb::ExecCommand: {
                VCOMP_COUT("Action: ExecCommand" << std::endl);
                char *cmd = nullptr;
                status = packet->unpack("%s", &cmd);
                std::cout << cmd << std::endl;
                debugger.sendCommand(cmd);
                free(cmd);
                //
                std::string out;
                debugger.recvResp(out);
                status = protoStream->send(action, "%s", out.c_str());
                if (-1 == status) {
                    GLADIUS_THROW_CALL_FAILED("Stream::Send");
                }
                status = protoStream->flush();
                if (-1 == status) {
                    GLADIUS_THROW_CALL_FAILED("Stream::Flush");
                }
                break;
            }
            case pgdb::Exit: {
                VCOMP_COUT("Action: Exit" << std::endl);
                break;
            }
            default:
                GLADIUS_CERR << "Received Invalid Action from Front-End!" << std::endl;
                action = pgdb::Exit;
        }
    } while (action != pgdb::Exit);

    VCOMP_COUT("Done with Main Loop." << std::endl);
}
