/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 */

#include "tool-be/tool-be.h"

#include "core/core.h"
#include "core/colors.h"
#include "tool-common/tool-common.h"
#include "dmi/dmi.h"

#include "lmon_api/lmon_be.h"

#include <cstdlib>
#include <string>
#include <limits.h>
#include <signal.h>

using namespace gladius;
using namespace gladius::toolbe;

////////////////////////////////////////////////////////////////////////////////
// ToolBE
////////////////////////////////////////////////////////////////////////////////
namespace {
// This component's name.
const std::string CNAME = "tool-be";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::NONE);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)

/**
 *
 */
int
feToBeUnpack(
    void *buf,
    int bufLen,
    void *data
) {
    using namespace toolbecommon;

    GLADIUS_UNUSED(bufLen);

    COMP_COUT << "###############################################" << std::endl;
    COMP_COUT << "# Unpacking FE to BE Data #####################" << std::endl;
    COMP_COUT << "###############################################" << std::endl;

    try {
        int currentParentPort, currentParentRank;

        char currentParent[HOST_NAME_MAX], *ptr = (char *)buf;
        auto *leafInfoArray = (ToolLeafInfoArrayT *)data;
        // Get the number of daemons and set up the leaf info array.
        (void)memcpy((void *)&(leafInfoArray->size), ptr, sizeof(int));
        ptr += sizeof(int);
        //
        leafInfoArray->leaves = (ToolLeafInfoT *)
            calloc(leafInfoArray->size, sizeof(ToolLeafInfoT));
        if (!leafInfoArray->leaves) {
            GLADIUS_THROW_OOR();
        }
        //
        // Get MRNet parent node info for each daemon.
        int nParents = 0;
        memcpy((void *)&nParents, ptr, sizeof(int));
        ptr += sizeof(int);
        //
        int daemon = -1;
        for (decltype(nParents) parent = 0; parent < nParents; ++parent) {
            int nChildren = 0;
            COMP_COUT << "Parent: " << parent << std::endl;
            // Get the parent host name, port, rank and child count.
            strncpy(currentParent, ptr, HOST_NAME_MAX);
            COMP_COUT << "*** Host Name: " << currentParent << std::endl;
            ptr += strlen(currentParent) + 1;
            //
            (void)memcpy(&currentParentPort, ptr, sizeof(int));
            COMP_COUT << "*** Port: " << currentParentPort << std::endl;
            ptr += sizeof(int);
            //
            memcpy(&currentParentRank, ptr, sizeof(int));
            COMP_COUT << "*** Rank: " << currentParentRank << std::endl;
            ptr += sizeof(int);
            //
            (void)memcpy(&nChildren, ptr, sizeof(int));
            COMP_COUT << "*** Number of Children: " << nChildren << std::endl;
            ptr += sizeof(int);
            // Iterate over this parent's children.
            for (decltype(nChildren) child = 0; child < nChildren; child++) {
                daemon++;
                if (daemon >= leafInfoArray->size) {
                    auto errStr = "Failed to Unpack Info From "
                                  "the Front-End. Expecting "
                                + std::to_string(leafInfoArray->size)
                                + "Daemons, but Received "
                                + std::to_string(daemon) + ".";
                    GLADIUS_THROW(errStr);
                    // Never reached.
                    return -1;
                }
                // Fill in the parent information.
                (void)strncpy((leafInfoArray->leaves)[daemon].parentHostName,
                               currentParent,
                               HOST_NAME_MAX);
                (leafInfoArray->leaves)[daemon].parentRank = currentParentRank;
                (leafInfoArray->leaves)[daemon].parentPort = currentParentPort;
                // Get the daemon host name.
                (void)strncpy((leafInfoArray->leaves)[daemon].hostName,
                              ptr, HOST_NAME_MAX);
                ptr += strlen((leafInfoArray->leaves)[daemon].hostName) + 1;
                // Get the daemon rank.
                (void)memcpy(&((leafInfoArray->leaves)[daemon].rank),
                             ptr, sizeof(int));
                ptr += sizeof(int);
            }
        }
    }
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
        return -1;
    }

    COMP_COUT << "###############################################" << std::endl;
    COMP_COUT << "# Done Unpacking FE to BE Data ################" << std::endl;
    COMP_COUT << "###############################################" << std::endl;
    return 0;
}
}

/**
 * TODO FIXME
 */
/* (static) */ void
ToolBE::redirectOutputTo(
    const std::string &base
) {
    GLADIUS_UNUSED(base);
    std::string fName = "/tmp/BE-" + std::to_string(getpid()) + ".txt";
    FILE *outRedirectFile = freopen(fName.c_str(), "w", stdout);
    if (!outRedirectFile) GLADIUS_THROW_CALL_FAILED("freopen stdout");
    outRedirectFile = freopen(fName.c_str(), "w", stderr);
    if (!outRedirectFile) GLADIUS_THROW_CALL_FAILED("freopen stderr");
}

/**
 * Constructor.
 */
ToolBE::ToolBE(
    void
) : mBeVerbose(false) { }

/**
 * Destructor.
 */
ToolBE::~ToolBE(void) { }

/**
 *
 */
void
ToolBE::mInitLMON(
    const core::Args &args,
    bool beVerbose
) {
    mLMONBE.init(args, beVerbose);
    // We know how to do this, so let LMON know what to call.
    mLMONBE.regUnpackForFEToBE(feToBeUnpack);
    //
    mLMONBE.handshake();
    // Let LMON populate our process table.
    mLMONBE.createAndPopulateProcTab(mProcTab);
}

/**
 *
 */
void
ToolBE::init(
    const core::Args &args,
    bool beVerbose
) {
    VCOMP_COUT("Initializing Tool Back-End..." << std::endl);
    try {
        mBeVerbose = beVerbose;
        mArgs = args;
        //
        mInitLMON(mArgs, mBeVerbose);
        //
        mMRNBE.init(mBeVerbose);
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
ToolBE::connect(void)
{
    VCOMP_COUT("Connecting..." << std::endl);
    VCOMP_COUT("Receiving Tool Leaf Information." << std::endl);
    toolbecommon::ToolLeafInfoArrayT lia;
    mLMONBE.recvConnectionInfo(lia);
    //
    mLMONBE.broadcast((void *)&lia.size, sizeof(int));
    // Non-masters allocate space for the MRNet connection info.
    if (!mLMONBE.amMaster()) {
        lia.leaves = (toolbecommon::ToolLeafInfoT *)
            calloc(lia.size, sizeof(toolbecommon::ToolLeafInfoT));
        if (!lia.leaves) GLADIUS_THROW_OOR();
    }
    VCOMP_COUT(
        "Broadcasting Connection Information to All Daemons." << std::endl
    );
    mLMONBE.broadcast(
        (void *)lia.leaves,
        lia.size * sizeof(toolbecommon::ToolLeafInfoT)
    );
    //
    mMRNBE.setPersonality(lia);
    //
    mMRNBE.connect();
    //
    mMRNBE.handshake();
    //
    mLoadPlugins();
    //
    free(lia.leaves);
}

/**
 *
 */
void
ToolBE::mLoadPlugins(void)
{
    using namespace std;

    VCOMP_COUT("Loading Plugins." << std::endl);
    // We need to first get the info from the FE. The FE is responsible for
    // making sure that the requested plugin is valid, so we minimize the amount
    // of validation that we have to do on our end. We also rely on the FE to
    // find the path to the plugin pack. We don't want N (N = # of daemons) to
    // be statting around looking for plugins.
    mMRNBE.pluginInfoRecv(mPluginName, mPathToPluginPack);
    // Okay, now we know what to load. So, do that now.
    VCOMP_COUT("Loading Plugins." << std::endl);
    // Get the front-end plugin pack.
    mPluginPack = mDSPManager.getPluginPackFrom(
                      dspa::DSPluginPack::PluginBE,
                      mPathToPluginPack
                  );
    auto *fePluginInfo = mPluginPack.pluginInfo;
    GLADIUS_COUT_STAT << "Back-End Plugin Info:" << endl;
    GLADIUS_COUT_STAT << "*Name      : " << fePluginInfo->pluginName << endl;
    GLADIUS_COUT_STAT << "*Version   : " << fePluginInfo->pluginVersion << endl;
    GLADIUS_COUT_STAT << "*Plugin ABI: " << fePluginInfo->pluginABI << endl;
    mBEPlugin = fePluginInfo->pluginConstruct();
    //
    VCOMP_COUT("Done Loading Plugins." << std::endl);
}

/**
 *
 */
void
ToolBE::enterPluginMain(void)
{
    VCOMP_COUT("Entering Plugin Main." << std::endl);
    //
    try {
        const char *dummyArgv[] = {"dummy-arg-0", nullptr};
        int dummyArgc = 1;
        gladius::core::Args dummyAppArgs(dummyArgc, dummyArgv, dummyArgv);
        dspi::DSPluginArgs pluginArgs(
            mPathToPluginPack,
            dummyAppArgs,
            mProcTab,
            mMRNBE.getProtoStream(),
            mMRNBE.getNetwork()
        );
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Back-end Plugin Entry Point.
        mBEPlugin->pluginMain(pluginArgs);
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    VCOMP_COUT("Exited Plugin Main." << std::endl);
}

#if 0
/**
 *
 */
void
ToolBE::enterDomain(void)
{
    VCOMP_COUT("Entering Domain." << std::endl);
    // FIXME
    // Since we started our tool daemons under debugger control, we must send a
    // continue signal to them.
    // TODO the FE and BE need to sync.
    auto *pt = mProcTab.procTab();
    for (decltype(mProcTab.nEntries()) p = 0; p < mProcTab.nEntries(); ++p) {
        kill(pt[p].pd.pid, SIGCONT);
    }
    sleep(1000);
}
#endif

/**
 *
 */
void
ToolBE::finalize(void)
{
    mLMONBE.finalize();
}
