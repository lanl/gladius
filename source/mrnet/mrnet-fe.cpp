/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
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
    const toolcommon::Hosts &hosts
) : mTopoFilePath(topoFilePath)
  , mTopoType(topoType)
  , mFEHostName(feHostName)
  , mHosts(hosts)
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
 * Generates a 1xN (where N is the number of remote hosts) topology.
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
    for (const auto &hostName : mHosts.hostNames()) {
        resTopoStr += "  " + hostName + ":" + to_string(id++) + "\n";
    }
    resTopoStr += ";";
    return resTopoStr;
}

////////////////////////////////////////////////////////////////////////////////
// MRNetFE
////////////////////////////////////////////////////////////////////////////////
namespace {
/**
 *
 */
int
feToBEPack(
    void *data,
    void *buf,
    int bufMax,
    int *bufLen
) {
    using namespace std;
    using namespace MRN;
    using namespace toolcommon;

    LeafInfo *leafInfo = (LeafInfo *)data;
    NetworkTopology *networkTopology = leafInfo->networkTopology;
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
    cout      << endl;

    char *ptr = (char *)buf;
    char *daemonCountPtr = ptr;

    // Reserve space for the number of daemons (to be calculated later).
    int total = sizeof(int);
    ptr += sizeof(int);

    // Pack up the number of parent nodes.
    int nLeaves = leafInfo->leafCps.size();
    (void)memcpy(ptr, (void *)&nLeaves, sizeof(int));
    ptr += sizeof(int);
    total += sizeof(int);

    std::set<std::string>::iterator daemonIter = leafInfo->daemons.begin();
    NetworkTopology::Node *node = nullptr;
    char *childCountPtr = nullptr;
    int len = 0, port = 0, rank = 0, daemonRank = 0, daemonCount = 0;
    unsigned long i = 0, j;
    // Write the data one parent at a time.
    for (i = 0; i < (unsigned long)nLeaves; ++i) {
        // Get the parent info.
        node = leafInfo->leafCps[i];
        port = node->get_Port();
        rank = node->get_Rank();
        std::string currentHost = node->get_HostName();

        // Calculate the amount of data.
        len = strlen(currentHost.c_str()) + 1;
        // 3x for node, port, and rank.
        total += (3 * sizeof(int)) + len;
        if (total > bufMax) {
            GLADIUS_CERR << "Exceeded Maximum Packing Buffer" << std::endl;
            return -1;
        }

        // Write the parent host name, port, rank and child count.
        (void)memcpy(ptr, (void *)currentHost.c_str(), len);
        ptr += len;
        (void)memcpy(ptr, (void *)&port, sizeof(int));
        ptr += sizeof(int);
        (void)memcpy(ptr, (void *)&rank, sizeof(int));
        ptr += sizeof(int);
        childCountPtr = ptr;
        ptr += sizeof(int);

        for (j = 0;
             j < (leafInfo->daemons.size() / nLeaves)
                 + (leafInfo->daemons.size() % nLeaves > i ? 1 : 0);
             ++j, daemonIter++, daemonCount++) {
            if (daemonIter == leafInfo->daemons.end()) break;
            len = strlen(daemonIter->c_str()) + 1;
            total += sizeof(int) + len;

            if (total > bufMax) {
                GLADIUS_CERR << "Exceeded Maximum Packing Buffer" << std::endl;
                return -1;
            }

            // Copy the daemon host name.
            (void)memcpy(ptr, (void *)(daemonIter->c_str()), len);
            ptr += len;

            // Copy the daemon rank.
            daemonRank = daemonCount + nNodes;
            (void)memcpy(ptr, (void *)(&daemonRank), sizeof(int));
            ptr += sizeof(int);
        }
        (void)memcpy(childCountPtr, (void *)&j, sizeof(int));
    }

    // Write the daemon count to the appropriate location */
    (void)memcpy(daemonCountPtr, (void *)&daemonCount, sizeof(int));

    *bufLen = total;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
namespace MRNetFEGlobals {
// The number of back-ends that have reported back to us.
int numBEsReporting = 0;
}

/**
 * Connection event callback.
 */
void
beConnectCbFn(
    MRN::Event *event,
    void *
) {
    std::mutex mtx;
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
    std::mutex mtx;
    if (MRN::Event::TOPOLOGY_EVENT == event->get_Class()
        && MRN::TopologyEvent::TOPOL_REMOVE_NODE == event->get_Type()) {
        std::lock_guard<std::mutex> lock(mtx);
        COMP_COUT << "A Node Loss Was Detected! Implement Recovery."
                  << std::endl << std::flush;
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
 * Returns a function pointer that is responsible for packing data for front-end
 * to back-end transfers.
 */

toolcommon::FEToBePackFnP
MRNetFE::getFEToBePackFun(void)
{
    return feToBEPack;
}

/**
 * Sets some environment variables that impact the behavior of MRNetFE.
 */
void
MRNetFE::mSetEnvs(void)
{
    core::utils::setEnv("MRNET_RSH", "/usr/bin/ssh");
}

/**
 * Initialization.
 */
void
MRNetFE::init(
    bool beVerbose
) {
    using namespace std;
    using namespace core;
    string whatsWrong;
    try {
        mBeVerbose = beVerbose;
        VCOMP_COUT("Initializing MRNet Front-End." << endl);
        if (!mDetermineAndSetPaths(whatsWrong)) {
            GLADIUS_THROW(whatsWrong);
        }
        mSetEnvs();
        mSessionDir = core::SessionFE::TheSession().sessionDir();
        // Create a unique name for the file.
        mTopoFile = mSessionDir + utils::osPathSep
                  + utils::getHostname() + "-"
                  + std::to_string(getpid()) + "-" + CNAME + ".topo";
        VCOMP_COUT("Topology Specification File: " << mTopoFile << endl);
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
std::string
MRNetFE::mGetPrefixFromCommNode(
    const std::string &whichString
) {
    std::string badness = "Could not determine MRNet's installation "
                          "prefix by inspecting the following path:"
                          "'" + whichString + "'";
    std::string prefix = whichString;
    std::string last = "/bin/" + sCommNodeName;
    auto found = prefix.rfind(last);
    // Not found, so something is wrong.
    if (std::string::npos == found) {
        GLADIUS_THROW(badness);
    }
    prefix = prefix.substr(0, found);
    return prefix;
}

/**
 *
 */
bool
MRNetFE::mDetermineAndSetPaths(
    std::string &whatsWrong
) {
    using namespace std;

    whatsWrong = "";
    VCOMP_COUT("Determining and Setting Paths." << std::endl);
    // Make sure that we can find mrnet_commnode. Really just to get the base
    // MRNet installation path. That way we can find the libraries that we need.
    string cnPath;
    auto status = core::utils::which(sCommNodeName, cnPath);
    if (GLADIUS_SUCCESS != status) {
        whatsWrong = toolcommon::utils::genNotInPathErrString(sCommNodeName);
        return false;
    }
    // Now get MRNet's installation prefix.
    mPrefixPath = mGetPrefixFromCommNode(cnPath);
    return true;
}

/**
 *
 */
void
MRNetFE::finalize(void)
{
    using namespace std;
    try {
        VCOMP_COUT("Finalizing MRNet Front-End." << endl);
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * Registers network event callbacks.
 */
void
MRNetFE::mRegisterEventCallbacks(void)
{
    using namespace MRN;

    bool rc = mNetwork->register_EventCallback(
                  Event::TOPOLOGY_EVENT,
                  TopologyEvent::TOPOL_ADD_BE,
                  beConnectCbFn,
                  NULL
              );
    if (!rc) GLADIUS_THROW_CALL_FAILED("register_EventCallback");
    //
    rc = mNetwork->register_EventCallback(
             Event::TOPOLOGY_EVENT,
             TopologyEvent::TOPOL_REMOVE_NODE,
             nodeLostCbFn,
             NULL
         );
}

/**
 *
 */
void
MRNetFE::createNetworkFE(
    const toolcommon::ProcessTable &procTab
) {
    // First, create and populate MRNet network topology file.
    // TODO dynamic TopologyType based on job characteristics.
    VCOMP_COUT("Creating and Populating MRNet Topology" << std::endl);
    // Stash the process table because we'll need this info later.
    mProcTab = procTab;
    //
    const auto &hosts = toolcommon::Hosts(mProcTab);
    // Set the number of target hosts
    mNumAppNodes = hosts.nHosts();
    // Create the topology file.
    MRNetTopology topo(
        mTopoFile,
        MRNetTopology::TopologyType::FLAT,
        core::utils::getHostname(),
        hosts
    );
    // Both NULL because MRNet is NOT going to be launching the tool daemons.
    const char *dummyBackendExe = NULL;
    const char *dummyArgv = NULL;
    mNetwork = MRN::Network::CreateNetworkFE(
                   mTopoFile.c_str(),
                   dummyBackendExe,
                   &dummyArgv
               );
    if (!mNetwork) {
        GLADIUS_THROW_CALL_FAILED("MRN::Network::CreateNetworkFE");
    }
    else if (mNetwork->has_Error()) {
        auto netErr = mNetwork->get_ErrorStr(mNetwork->get_Error());
        GLADIUS_THROW_CALL_FAILED(netErr);
    }
    //
    mRegisterEventCallbacks();
    //
    mLeafInfo.networkTopology = mNetwork->get_NetworkTopology();
    if (!mLeafInfo.networkTopology) {
        GLADIUS_THROW_CALL_FAILED("MRN::Network::get_NetworkTopology");
    }
    //
    mNTreeNodes = mLeafInfo.networkTopology->get_NumNodes();
    //
    mLeafInfo.daemons.insert(hosts.hostNames().cbegin(),
                             hosts.hostNames().cend());
    //
    mCreateDaemonTIDMap();
    //
    mLeafInfo.networkTopology->get_Leaves(mLeafInfo.leafCps);
}

/**
 *
 */
void
MRNetFE::mCreateDaemonTIDMap(void)
{
    using namespace std;
    using namespace toolcommon;

    // A map between host names and the task IDs on them.
    const auto *procTab = mProcTab.procTab();
    map< string, vector<decltype(procTab->mpirank)> > hostTIDMap;
    auto npte = mProcTab.nEntries();
    for (decltype(npte) tid = 0; tid < npte; ++tid) {
        hostTIDMap[procTab[tid].pd.host_name].push_back(procTab[tid].mpirank);
    }
    if (mBeVerbose) {
        for (const auto &host : hostTIDMap) {
            COMP_COUT << "Host " << host.first
                      << " Responsible for: "
                      << host.second.size()
                      << " Processes." << endl;
        }
    }
    // A call to get_BackEndNodes will populate this thing...
    set<MRN::NetworkTopology::Node *> backendNodes;
    // A mapping between hostnames and MRNet ranks.
    map<string, int> hostToMrnetRankMap;
    // Populate backendNodes.
    mLeafInfo.networkTopology->get_BackEndNodes(backendNodes);
    // Populate hostname to MRNet ranks.
    for (const auto &beNode : backendNodes) {
        const auto nodeRank = beNode->get_Rank();
        const auto chn = Hosts::canonicalForm(beNode->get_HostName());
        hostToMrnetRankMap[chn] = nodeRank;
    }
    // Now prepare info to be transfered to the remote daemons.
    decltype(mNTreeNodes) loopi = 0;
    for (auto &host : hostTIDMap) {
        // Convenience TID vector reference.
        auto &tvf = host.second;
        auto tid = procTab->mpirank;
        TxList<decltype(tid)> daemonTIDs;
        // Stash the number of TIDs
        daemonTIDs.nElems = tvf.size();
        // Make room for the list of TIDs.
        daemonTIDs.elems = (decltype(tid) *)
            calloc(daemonTIDs.nElems, sizeof(tid));
        if (!daemonTIDs.elems) GLADIUS_THROW_OOR();
        // Sort the TID vector.
        sort(tvf.begin(), tvf.end());
        // And then copy over all that good TID info.
        for (auto id = 0UL; id < daemonTIDs.nElems; ++id) {
            daemonTIDs.elems[id] = tvf[id];
        }
        // The MRNet BEs are NOT connected.
        if (0 == hostToMrnetRankMap.size()) {
            // XXX Why like this?
            mMRNetRankToTIDsMap[mNTreeNodes + loopi] = daemonTIDs;
        }
        else {
            // XXX See: STAT_FrontEnd::createDaemonRankMap for more info.
            assert(false && "Oops! Guess we need to deal with this...");
        }
    }
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
        if (mNumAppNodes == (size_t)MRNetFEGlobals::numBEsReporting) {
            return GLADIUS_SUCCESS;
        }
        else {
            VCOMP_COUT("Sill Waiting for All Daemons to Report Back..."
                       << endl);
            VCOMP_COUT("*** "
                       << setw(6) << setfill('0')
                       << MRNetFEGlobals::numBEsReporting << " Out of "
                       << setw(6) << setfill('0')
                       << mNumAppNodes << " Daemons Reporting."
                       << endl);
            return GLADIUS_NOT_CONNECTED;
        }
    }
    catch (const std::exception &e) {
        // If we are being verbose, then show the error.
        if (mBeVerbose) {
            GLADIUS_CERR << e.what() << std::endl;
        }
        return GLADIUS_NOT_CONNECTED;
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

    // NOTE: Can print topology file from:
    // mLeafInfo.networkTopology->print_TopologyFile();
    mBcastComm = mNetwork->get_BroadcastCommunicator();
    if (!mBcastComm) {
        GLADIUS_THROW_CALL_FAILED("get_BroadcastCommunicator");
    }
    //
    mLoadCoreFilters();

    VCOMP_COUT("Done Initializing Network." << std::endl);
}

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
    VCOMP_COUT("Sending Plugin Info to Back-Ends." << std::endl);
    //
    const char *pluginName = validPluginName.c_str();
    const char *pluginPath = pathToValidPlugin.c_str();
    // NOTE: The filters aren't setup to receive string data, so don't do
    // that with this particular stream and filter combo.
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
    VCOMP_COUT("Done Sending Plugin Info to Back-Ends." << std::endl);
}
