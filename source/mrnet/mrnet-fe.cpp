/**
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
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

#include "mrnet/MRNet.h"

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

using namespace gladius;
using namespace gladius::mrnetfe;

namespace {
// This component's name.
const std::string CNAME = "mrnetfe";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::GREEN);
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
    const std::string &topoFilePath,
    TopologyType topoType,
    const std::string &feHostName,
    const core::ProcessLandscape &procLandscape
) : mTopoFilePath(topoFilePath)
  , mTopoType(topoType)
  , mFEHostName(feHostName)
  , mProcLandscape(procLandscape)
  , mCanRMFile(false)
{
    using namespace std;
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
    catch (const std::exception &e) {
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
std::string
MRNetTopology::mGenFlatTopo(void)
{
    using namespace std;
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
    static std::mutex mtx;
    if (MRN::Event::TOPOLOGY_EVENT == event->get_Class()
        && MRN::TopologyEvent::TOPOL_ADD_BE == event->get_Type()) {
        std::lock_guard<std::mutex> lock(mtx);
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
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    if (MRN::Event::TOPOLOGY_EVENT == event->get_Class()
        && MRN::TopologyEvent::TOPOL_REMOVE_NODE == event->get_Type()) {
        COMP_COUT << "A Node Loss Was Detected!" << std::endl << std::flush;
    }
}
} // end namespace

const std::string MRNetFE::sCommNodeName = "mrnet_commnode";
// If filter shared object names change, then update this.
const std::string MRNetFE::sCoreFiltersSO = "libGladiusMRNetCoreFilters.so";

/**
 * Constructor.
 */
MRNetFE::MRNetFE(
    void
) : mPrefixPath("")
  , mNThread(1)
{
    MRNetFEGlobals::numBEsReporting = 0;
}

/**
 * Destructor.
 */
MRNetFE::~MRNetFE(void)
{
    // TODO complete
    if (mNetwork) { }
}

/**
 * Sets some environment variables that impact the behavior of MRNetFE.
 */
int
MRNetFE::mSetEnvs(void)
{
    VCOMP_COUT("Setting important environment variables..." << std::endl);

    return core::utils::setEnv("MRNET_RSH", "/usr/bin/ssh");
}

/**
 * Initialization.
 */
int
MRNetFE::init(
    bool beVerbose
) {
    using namespace std;
    using namespace core;
    //
    try {
        mBeVerbose = beVerbose;
        //
        VCOMP_COUT("Initializing MRNet front-end..." << endl);
        //
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
                  + std::to_string(getpid()) + "-" + CNAME + ".topo";
        VCOMP_COUT("Topology specification file: " << mTopoFile << endl);
    }
    catch (const std::exception &e) {
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
    const std::string &whichString,
    std::string &prefix
) {
    std::string badness = "Could not determine MRNet's installation "
                          "prefix by inspecting the following path:"
                          "'" + whichString + "'";
    prefix = whichString;
    std::string last = "/bin/" + sCommNodeName;
    auto found = prefix.rfind(last);
    // Not found, so something is wrong.
    if (std::string::npos == found) {
        GLADIUS_CERR << badness << std::endl;
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
    using namespace std;
    //
    VCOMP_COUT("Determining and setting paths..." << std::endl);
    // Make sure that we can find mrnet_commnode. Really just to get the base
    // MRNet installation path. That way we can find the libraries that we need.
    string cnPath;
    int status = core::utils::which(sCommNodeName, cnPath);
    if (GLADIUS_SUCCESS != status) {
        GLADIUS_CERR << toolcommon::utils::genNotInPathErrString(sCommNodeName)
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
    using namespace std;
    try {
        VCOMP_COUT("Finalizing MRNet front-end." << endl);
    }
    catch (const std::exception &e) {
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
    VCOMP_COUT("Registering event callbacks..." << std::endl);
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
        static const std::string f = "MRN::register_EventCallback";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << std::endl;
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
    using namespace std;
    using namespace MRN;
    //
    VCOMP_COUT("Building network..." << std::endl);
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
    catch (const std::exception &e) {
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
    using namespace std;
    using namespace gladius::core;
    using namespace MRN;
    //
    VCOMP_COUT("Populating leaf info..." << std::endl);
    //
    mLeafInfo.networkTopology = mNetwork->get_NetworkTopology();
    if (!mLeafInfo.networkTopology) {
        static const std::string f = "MRN::get_NetworkTopology";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << std::endl;
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
    using namespace std;
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
    std::vector<toolcommon::ToolLeafInfoT> &cMap
) {
    VCOMP_COUT("Generating connection map..." << std::endl);
    //
    const auto numLeaves = mLeafInfo.leaves.size();
    //
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
    VCOMP_COUT("Creating and populating MRNet topology" << std::endl);
    // Stash the process landscape because we'll need this info later.
    mProcLandscape = procLandscape;
    // Build network
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mBuildNetwork())) {
        return rc;
    }
    //
    if (GLADIUS_SUCCESS != (rc = mRegisterEventCallbacks())) {
        return rc;
    }
    //
    if (GLADIUS_SUCCESS != (rc = mPopulateLeafInfo())) {
        return rc;
    }
    if (GLADIUS_SUCCESS != (rc = mEchoNetStats())) {
        return rc;
    }
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
    using namespace std;

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
    catch (const std::exception &e) {
        GLADIUS_THROW(e.what());
    }
    return GLADIUS_ERR;
}

/**
 *
 */
void
MRNetFE::mLoadCoreFilters(void)
{
    VCOMP_COUT("Loading Core Filters." << std::endl);
    //
    static const auto ps = core::utils::osPathSep;
    static const auto execPrefix = core::SessionFE::TheSession().execPrefix();
    static const auto soPrefix = execPrefix + ps + "lib";
    //
    VCOMP_COUT("Looking For Core Filters in: " << soPrefix << std::endl);
    //
    static const std::string coreFilterSOName = soPrefix + ps + sCoreFiltersSO;
    auto filterID = mNetwork->load_FilterFunc(
                        coreFilterSOName.c_str(),
                        "GladiusMRNetProtoFilter"
                    );
    if (-1 == filterID) {
        GLADIUS_THROW_CALL_FAILED("load_FilterFunc: GladiusMRNetProtoFilter");
    }
    //
    mProtoStream = mNetwork->new_Stream(
                       mBcastComm,
                       MRN::SFILTER_WAITFORALL,
                       filterID
                   );
    if (!mProtoStream) {
        GLADIUS_THROW_CALL_FAILED("new_Stream");
    }
    //
    VCOMP_COUT("Done Loading Core Filters." << std::endl);
}

/**
 *
 */
void
MRNetFE::networkInit(void)
{
    VCOMP_COUT("Initializing Network." << std::endl);

#if 0 // DEBUG
    mLeafInfo.networkTopology->print_TopologyFile(mTopoFile.c_str());
#endif
    mBcastComm = mNetwork->get_BroadcastCommunicator();
    if (!mBcastComm) {
        GLADIUS_THROW_CALL_FAILED("get_BroadcastCommunicator");
    }
    //
    mLoadCoreFilters();
    //
    VCOMP_COUT("Done Initializing Network." << std::endl);
}

#if 0
/**
 * Initial FE to BE handshake.
 */
void
MRNetFE::handshake(void)
{
    VCOMP_COUT("Starting Lash-Up Handshake." << std::endl);
    // Ping!
    auto status = mProtoStream->send(
                      toolcommon::MRNetCoreTags::InitHandshake,
                      "%d",
                      GladiusMRNetProtoFilterMagic
                  );
    if (-1 == status) {
        GLADIUS_THROW_CALL_FAILED("Stream::Send");
    }
    status = mProtoStream->flush();
    if (-1 == status) {
        GLADIUS_THROW_CALL_FAILED("Stream::Flush");
    }
    // Pong!
    MRN::PacketPtr packet;
    int tag = 0;
    status = mProtoStream->recv(&tag, packet);
    if (-1 == status) {
        GLADIUS_THROW_CALL_FAILED("Stream::Recv");
    }
    if (toolcommon::MRNetCoreTags::InitHandshake != tag) {
        GLADIUS_THROW("Received Invalid Tag From Tool Back-End");
    }
    int data = 0;
    status = packet->unpack("%d", &data);
    // Once we are here, then we know that all the back-ends have reported back.
    if (0 != status) {
        GLADIUS_THROW_CALL_FAILED("PacketPtr::unpack");
    }
    // Notice the negative here...
    if (data != -GladiusMRNetProtoFilterMagic) {
        GLADIUS_THROW("Received Invalid Data From Tool Back-End");
    }
    //
    VCOMP_COUT("Done with Lash-Up Handshake." << std::endl);
}

/**
 * Sends plugin name and path to BEs.
 */
void
MRNetFE::pluginInfoBCast(
    const std::string &validPluginName,
    const std::string &pathToValidPlugin
)
{
    VCOMP_COUT("Sending plugin info to back-ends." << std::endl);
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
        GLADIUS_THROW_CALL_FAILED("Stream::Send");
    }
    status = mProtoStream->flush();
    if (-1 == status) {
        GLADIUS_THROW_CALL_FAILED("Stream::Flush");
    }
    //
    VCOMP_COUT("Done sending plugin info to back-ends." << std::endl);
}
#endif
