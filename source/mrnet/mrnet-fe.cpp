/**
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 *
 * Copyright (c) 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller
 * Detailed MRNet usage rights in "LICENSE" file in the MRNet distribution.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "mrnet/mrnet-fe.h"
#include "mrnet/filters/core-filters.h"

#include "core/core.h"
#include "core/utils.h"
#include "core/session.h"
#include "core/colors.h"
#include "tool-common/tool-common.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <mutex>
#include <iomanip>

#include <sys/types.h>
#include <unistd.h>

#include "mrnet/MRNet.h"

using namespace std;
using namespace gladius;
using namespace gladius::core;
using namespace gladius::mrnetfe;

namespace {
// This component's name.
const string CNAME = "mrnetfe";
// CNAME's color code.
const string NAMEC = core::colors::color().ansiBeginColor(core::colors::GREEN);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
MRNetTopology::MRNetTopology(
    const string &topoFilePath,
    TopologyType topoType,
    const string &feHostName,
    const core::ProcessLandscape &procLandscape
) : mTopoFilePath(topoFilePath)
  , mTopoType(topoType)
  , mFEHostName(feHostName)
  , mProcLandscape(procLandscape)
  , mCanRMFile(false)
{
    ofstream theFile;
    try {
        // Create topology file.
        theFile.open(mTopoFilePath, ios::out | ios::trunc);
        string topoString;
        switch (mTopoType) {
            case FLAT:
                topoString = mGenFlatTopo();
                break;
            default:
                GLADIUS_THROW_INVLD_ARG();
        }
#if 0 // DEBUG
        GLADIUS_COUT_STAT << "Network Topology:" << endl << topoString << endl;
#endif
        // Populate the topology with the generated string.
        theFile << topoString;
        theFile.close();
        mCanRMFile = true;
    }
    catch (const exception &e) {
        if (theFile.is_open()) theFile.close();
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * Generates a 1xN (where N is the number of remote target processes) topology.
 * localhost:0 =>
 *   host:1
 *   host:2
 *   host:n-1 ;
 */
string
MRNetTopology::mGenFlatTopo(void)
{
    auto id = 0;
    // The "host:0 =>" bit.
    // The top of the tree is always going to be localhost.
    string resTopoStr = "localhost: " + to_string(id++) + " =>\n";
    for (const auto &l : mProcLandscape.landscape()) {
        const string hostName = l.first;
        const int nOnHost = l.second;
        for (int targetID = 0; targetID < nOnHost; ++targetID) {
            resTopoStr += "  " + hostName + ":" + to_string(id++) + "\n";
        }
    }
    resTopoStr += ";";
    return resTopoStr;
}

////////////////////////////////////////////////////////////////////////////////
// MRNetFE
////////////////////////////////////////////////////////////////////////////////
namespace {

////////////////////////////////////////////////////////////////////////////////
namespace MRNetFEGlobals {
// The number of back-ends that have reported back to us.
unsigned numBEsReporting = 0;
}

/**
 * Connection event callback.
 */
void
beConnectCbFn(
    MRN::Event *event,
    void *
) {
    static mutex mtx;
    lock_guard<mutex> lock(mtx);
    if (MRN::Event::TOPOLOGY_EVENT == event->get_Class() &&
        MRN::TopologyEvent::TOPOL_ADD_BE == event->get_Type()) {
        MRNetFEGlobals::numBEsReporting++;
    }
}

/**
 * Node lost callback.
 */
void
nodeLostCbFn(
    MRN::Event *event,
    void *
) {
    static mutex mtx;
    lock_guard<mutex> lock(mtx);
    if (MRN::Event::TOPOLOGY_EVENT == event->get_Class() &&
        MRN::TopologyEvent::TOPOL_REMOVE_NODE == event->get_Type()) {
        COMP_COUT << "A Node Loss Was Detected!" << endl << flush;
    }
}
} // end namespace

const string MRNetFE::sCommNodeName = "mrnet_commnode";
// If filter shared object names change, then update this.
const string MRNetFE::sCoreFiltersSO = "libGladiusMRNetCoreFilters.so";

/**
 * Constructor.
 */
MRNetFE::MRNetFE(
    void
) : mBeVerbose(false)
  , mPrefixPath("")
  , mNThread(1)
{
    // Make sure that constants determined at configure time are still valid.
    assert(toolcommon::GladiusFirstApplicationTag == FirstApplicationTag &&
           "Sorry, some MRNet constants changed since this was built... "
           "Please reconfigure and rebuild to fix this problem.");
    MRNetFEGlobals::numBEsReporting = 0;
}

/**
 * Destructor.
 */
MRNetFE::~MRNetFE(void)
{
    // TODO complete
    if (mNetwork) {
        delete mNetwork;
        mNetwork = nullptr;
    }
}

/**
 * Sets some environment variables that impact the behavior of MRNetFE.
 */
int
MRNetFE::mSetEnvs(void)
{
    VCOMP_COUT("Setting important environment variables..." << endl);
    //
    return core::utils::setEnv("MRNET_RSH", "/usr/bin/ssh");
}

/**
 * Initialization.
 */
int
MRNetFE::init(
    bool beVerbose
) {
    using namespace core;
    //
    mBeVerbose = beVerbose;
    VCOMP_COUT("Initializing MRNet front-end..." << endl);
    //
    try {
        int rc = GLADIUS_SUCCESS;
        string whatsWrong;
        if (GLADIUS_SUCCESS != (rc = mDetermineAndSetPaths())) {
            return rc;
        }
        if (GLADIUS_SUCCESS != (rc = mSetEnvs())) {
            return GLADIUS_ERR;
        }
        //
        mSessionDir = core::SessionFE::TheSession().sessionDir();
        // Create a unique name for the file.
        mTopoFile = mSessionDir + utils::osPathSep
                  + utils::getHostname() + "-"
                  + to_string(getpid()) + "-" + CNAME + ".topo";
        VCOMP_COUT("Topology specification file: " << mTopoFile << endl);
    }
    catch (const exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetFE::mGetPrefixFromCommNode(
    const string &whichString,
    string &prefix
) {
    const string badness = "Could not determine MRNet's installation "
                                "prefix by inspecting the following path: '"
                              + whichString + "'";
    prefix = whichString;
    string last = "/bin/" + sCommNodeName;
    auto found = prefix.rfind(last);
    // Not found, so something is wrong.
    if (string::npos == found) {
        GLADIUS_CERR << badness << endl;
        return GLADIUS_ERR;
    }
    prefix = prefix.substr(0, found);
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetFE::mDetermineAndSetPaths(void)
{
    VCOMP_COUT("Determining and setting paths..." << endl);
    // Make sure that we can find mrnet_commnode. Really just to get the base
    // MRNet installation path. That way we can find the libraries that we need.
    string cnPath;
    int status = core::utils::which(sCommNodeName, cnPath);
    if (GLADIUS_SUCCESS != status) {
        GLADIUS_CERR << toolcommon::genNotInPathErrString(sCommNodeName)
                     << endl;
        return GLADIUS_ERR;
    }
    // Now get MRNet's installation prefix.
    return mGetPrefixFromCommNode(cnPath, mPrefixPath);
}

/**
 *
 */
void
MRNetFE::finalize(void)
{
    try {
        VCOMP_COUT("Finalizing MRNet front-end." << endl);
    }
    catch (const exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * Registers network event callbacks.
 */
int
MRNetFE::mRegisterEventCallbacks(void)
{
    using namespace gladius::core;
    using namespace MRN;
    //
    VCOMP_COUT("Registering event callbacks..." << endl);
    //
    int nErrs = 0;
    bool rc = mNetwork->register_EventCallback(
                  Event::TOPOLOGY_EVENT,
                  TopologyEvent::TOPOL_ADD_BE,
                  beConnectCbFn,
                  NULL
              );
    if (!rc) ++nErrs;
    //
    rc = mNetwork->register_EventCallback(
             Event::TOPOLOGY_EVENT,
             TopologyEvent::TOPOL_REMOVE_NODE,
             nodeLostCbFn,
             NULL
         );
    if (!rc) ++nErrs;
    //
    if (0 != nErrs) {
        static const string f = "MRN::register_EventCallback";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR;
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetFE::mBuildNetwork(void)
{
    using namespace MRN;
    //
    VCOMP_COUT("Building network..." << endl);
    //
    try {
        // Create the topology file.
        MRNetTopology topo(
            mTopoFile,
            MRNetTopology::TopologyType::FLAT,
            core::utils::getHostname(),
            mProcLandscape
        );
        mNetwork = Network::CreateNetworkFE(
                       mTopoFile.c_str(), // path to topology file
                       NULL,              // path to back-end exe
                       NULL,              // back-end argv
                       NULL,              // Network attributes
                       true,              // rank back-ends (start from 0)
                       false              // topology in memory buffer, not file
                   );
        if (!mNetwork) {
            static const string f = "MRN::Network::CreateNetworkFE";
            GLADIUS_CERR << core::utils::formatCallFailed(f, GLADIUS_WHERE)
                         << endl;
            return GLADIUS_ERR;
        }
        else if (mNetwork->has_Error()) {
            const string netErr = mNetwork->get_ErrorStr(mNetwork->get_Error());
            GLADIUS_CERR << "Network errors detected: " << netErr << endl;
            return GLADIUS_ERR;
        }
    }
    catch (const exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetFE::mPopulateLeafInfo(void)
{
    using namespace gladius::core;
    using namespace MRN;
    //
    VCOMP_COUT("Populating leaf info..." << endl);
    //
    mLeafInfo.networkTopology = mNetwork->get_NetworkTopology();
    if (!mLeafInfo.networkTopology) {
        static const string f = "MRN::get_NetworkTopology";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR;
    }
    //
    mNTreeNodes = mLeafInfo.networkTopology->get_NumNodes();
    //
    mLeafInfo.networkTopology->get_Leaves(mLeafInfo.leaves);
    //
    return GLADIUS_SUCCESS;
}

/**
 * Displays network statistics.
 */
int
MRNetFE::mEchoNetStats(void)
{
    using namespace MRN;
    using namespace toolcommon;

    NetworkTopology *networkTopology = mLeafInfo.networkTopology;
    //
    unsigned int nNodes = 0, depth = 0, minFanout = 0, maxFanout = 0;
    double averageFanout = 0.0, stdDevFanout = 0.0;
    // Grab some tree stats.
    networkTopology->get_TreeStatistics(
        nNodes,
        depth,
        minFanout,
        maxFanout,
        averageFanout,
        stdDevFanout
    );
    // Show off the fresh stats.
    GLADIUS_COUT_STAT << "::: MRNet Tree Statistics ::::::::::::::::::::::::::";
    cout      << endl;
    GLADIUS_COUT_STAT << "Number of Nodes : " << nNodes << endl;
    GLADIUS_COUT_STAT << "Depth           : " << depth << endl;
    GLADIUS_COUT_STAT << "Minimum Fanout  : " << minFanout << endl;
    GLADIUS_COUT_STAT << "Maximum Fanout  : " << maxFanout << endl;
    GLADIUS_COUT_STAT << "Average Fanout  : " << averageFanout << endl;
    GLADIUS_COUT_STAT << "Sigma Fanout    : " << stdDevFanout << endl;
    GLADIUS_COUT_STAT << "::::::::::::::::::::::::::::::::::::::::::::::::::::";
    cout      << endl << flush;
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetFE::generateConnectionMap(
    vector<toolcommon::ToolLeafInfoT> &cMap
) {
    VCOMP_COUT("Generating connection map..." << endl);
    //
    const auto numLeaves = mLeafInfo.leaves.size();
    mNExpectedBEs = numLeaves * mNThread;
    //
    const unsigned besPerLeaf = mNExpectedBEs / numLeaves;
    unsigned currLeaf = 0;
    for (unsigned i = 0; (i < mNExpectedBEs) && (currLeaf < numLeaves); ++i) {
        if( i && (i % besPerLeaf == 0) ) {
            // select next parent
            currLeaf++;
            if (currLeaf == numLeaves) {
                // except when there is no "next"
                currLeaf--;
            }
        }
        auto &leaves = mLeafInfo.leaves;
#if 0 // DEBUG
        fprintf(stdout, "ToolBE %u will connect to %s:%d:%d\n",
                i,
                leaves[currLeaf]->get_HostName().c_str(),
                leaves[currLeaf]->get_Port(),
                leaves[currLeaf]->get_Rank()
        );
#endif
        // Build the info
        toolcommon::ToolLeafInfoT mi;
        memset(&mi, 0, sizeof(mi));
        const char *hn = leaves[currLeaf]->get_HostName().c_str();
        memmove(mi.parentHostName, hn, strlen(hn) + 1);
        mi.parentRank = leaves[currLeaf]->get_Rank();
        mi.parentPort = leaves[currLeaf]->get_Port();
        mi.rank       = i;
        cMap.push_back(mi);
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetFE::createNetworkFE(
    const core::ProcessLandscape &procLandscape
) {
    // First, create and populate MRNet network topology file.
    // TODO dynamic TopologyType based on job characteristics.
    VCOMP_COUT("Creating and populating MRNet topology" << endl);
    // Stash the process landscape because we'll need this info later.
    mProcLandscape = procLandscape;
    // Build network
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mBuildNetwork())) return rc;
    //
    if (GLADIUS_SUCCESS != (rc = mRegisterEventCallbacks())) return rc;
    //
    if (GLADIUS_SUCCESS != (rc = mPopulateLeafInfo())) return rc;
    if (GLADIUS_SUCCESS != (rc = mEchoNetStats())) return rc;
    //
    return GLADIUS_SUCCESS;
}

/**
 * Returns GLADIUS_SUCCESS if all expected participants have connected
 * successfully.
 */
int
MRNetFE::connect(void)
{
    VCOMP_COUT("Trying to Connect..." << endl);
    try {
        if (mNExpectedBEs == MRNetFEGlobals::numBEsReporting) {
            return GLADIUS_SUCCESS;
        }
        else {
            VCOMP_COUT("Sill waiting for all back-ends to report back..."
                       << endl);
            VCOMP_COUT("*** "
                       << setw(6) << setfill('0')
                       << MRNetFEGlobals::numBEsReporting << " Out of "
                       << setw(6) << setfill('0')
                       << mNExpectedBEs << " Reporting."
                       << endl);
            return GLADIUS_NOT_CONNECTED;
        }
    }
    catch (const exception &e) {
        GLADIUS_THROW(e.what());
    }
    return GLADIUS_ERR;
}

/**
 *
 */
int
MRNetFE::networkInit(void)
{
    using namespace gladius::core;
    //
    VCOMP_COUT("Initializing network..." << endl);
    //
    mBcastComm = mNetwork->get_BroadcastCommunicator();
    if (!mBcastComm) {
        static const string f = "MRN::get_BroadcastCommunicator";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    //
    return mLoadCoreFilters();
}

/**
 *
 */
int
MRNetFE::mLoadCoreFilters(void)
{
    VCOMP_COUT("Loading core filters..." << endl);
    //
    static const auto ps = core::utils::osPathSep;
    static const auto execPrefix = core::SessionFE::TheSession().execPrefix();
    static const auto soPrefix = execPrefix + ps + "lib";
    //
    VCOMP_COUT("Looking for core filters in: " << soPrefix << endl);
    static const string coreFilterSOName = soPrefix + ps + sCoreFiltersSO;
    auto filterID = mNetwork->load_FilterFunc(
                        coreFilterSOName.c_str(),
                        "GladiusMRNetProtoFilter"
                    );
    if (-1 == filterID) {
        static const string f = "load_FilterFunc: GladiusMRNetProtoFilter";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    //
    mProtoStream = mNetwork->new_Stream(
                       mBcastComm,
                       MRN::SFILTER_WAITFORALL,
                       filterID
                   );
    if (!mProtoStream) {
        static const string f = "MRN::Network->new_Stream";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 * Initial FE to BE handshake.
 */
int
MRNetFE::handshake(void)
{
    using namespace gladius::toolcommon;
    VCOMP_COUT("Starting lash-up handshake..." << endl);
    // Ping!
    auto status = mProtoStream->send(
                      MRNetCoreTags::InitHandshake,
                      "%d",
                      GladiusMRNetProtoFilterMagic
                  );
    if (-1 == status) {
        static const string f = "Stream::Send";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    status = mProtoStream->flush();
    if (-1 == status) {
        static const string f = "Stream::Flush";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    // Pong!
    MRN::PacketPtr packet;
    int tag = 0;
    status = mProtoStream->recv(&tag, packet);
    if (-1 == status) {
        static const string f = "Stream::Recv";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    if (MRNetCoreTags::InitHandshake != tag) {
        static const string errs = "Received Invalid Tag From Tool Back-End";
        GLADIUS_CERR << errs << endl;
        return GLADIUS_ERR;
    }
    int data = 0;
    status = packet->unpack("%d", &data);
    // Once we are here, then we know that all the back-ends have reported back.
    if (0 != status) {
        static const string f = "PacketPtr::unpack";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    // Notice the negative here...
    if (data != -GladiusMRNetProtoFilterMagic) {
        static const string errs = "Received Invalid Data From Tool Back-End";
        GLADIUS_CERR << errs << endl;
        return GLADIUS_ERR;
    }
    //
    return GLADIUS_SUCCESS;
}

/**
 * Sends plugin name and path to BEs.
 */
int
MRNetFE::pluginInfoBCast(
    const string &validPluginName,
    const string &pathToValidPlugin
) {
    VCOMP_COUT("Sending plugin info to back-ends..." << endl);
    //
    const char *pluginName = validPluginName.c_str();
    const char *pluginPath = pathToValidPlugin.c_str();
    // NOTE: The filters aren't setup to receive string data, so don't do
    // that with this particular stream and filter combo. Sending is fine.
    auto status = mProtoStream->send(
                      toolcommon::MRNetCoreTags::PluginNameInfo,
                      "%s %s",
                      pluginName,
                      pluginPath
                  );
    if (-1 == status) {
        static const string f = "Stream::Send";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR;
    }
    status = mProtoStream->flush();
    if (-1 == status) {
        static const string f = "Stream::Flush";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR;
    }
    //
    return GLADIUS_SUCCESS;
}
