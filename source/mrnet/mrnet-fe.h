/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller
 * Detailed MRNet usage rights in "LICENSE" file in the MRNet distribution.
 *
 */

#ifndef GLADIUS_MRNET_MRNET_FE_H_INCLUDED
#define GLADIUS_MRNET_MRNET_FE_H_INCLUDED

#include "mrnet/MRNet.h"

#include "core/hosts.h"
#include "tool-common/tool-common.h"

#include <string>
#include <cstdint>

namespace gladius {
namespace mrnetfe {
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MRNetTopology {
public:
    //
    enum TopologyType {
        FLAT = 0
    };
private:
    //
    std::string mTopoFilePath;
    //
    TopologyType mTopoType;
    // Hostname of the tool front-end.
    std::string mFEHostName;
    //
    core::Hosts mHosts;
    //
    std::string
    mGenFlatTopo(void);
    //
    bool mCanRMFile = false;
public:
    //
    MRNetTopology(void) { ; }
    //
    ~MRNetTopology(void) {
        if (mCanRMFile) remove(mTopoFilePath.c_str());
    }
    //
    MRNetTopology(
        const std::string &topoFilePath,
        TopologyType topoType,
        const std::string &feHostName,
        const core::Hosts &hosts
    );
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Implements the MRNet interface for a tool front-end.
 */
class MRNetFE {
private:
    //
    static const std::string sCommNodeName;
    //
    static const std::string sCoreFiltersSO;
    // Be verbose or not.
    bool mBeVerbose = false;
    // Base session directory.
    std::string mSessionDir;
    // Path to MRNet topology file.
    std::string mTopoFile;
    // Name of the backend executable
    std::string mBEExe;
    // Absolute path to MRNet installation.
    std::string mPrefixPath;
    // The process table of our job.
    toolcommon::ProcessTable mProcTab;
    // The number of nodes in our job.
    size_t mNumAppNodes = 0;
    // The MRNet network instance.
    MRN::Network *mNetwork = nullptr;
    //
    MRN::Communicator *mBcastComm = nullptr;
    //
    MRN::Stream *mProtoStream = nullptr;
    // The number of tree nodes in our topology.
    unsigned int mNTreeNodes = 0;
    //
    toolcommon::LeafInfo mLeafInfo;
    // A mapping between MRNet ranks and target task IDs.
    std::map< int, toolcommon::TxList<int> > mMRNetRankToTIDsMap;
    // Registers MRNet even callbacks.
    void
    mRegisterEventCallbacks(void);
    //
    bool
    mDetermineAndSetPaths(std::string &whatsWrong);
    //
    std::string
    mGetPrefixFromCommNode(
        const std::string &whichString
    );
    //
    void
    mCreateDaemonTIDMap(void);
    //
    void
    mLoadCoreFilters(void);

public:
    MRNetFE(void);
    //
    ~MRNetFE(void);
    //
    void
    mSetEnvs(void);
    //
    void
    createNetworkFE(
        const toolcommon::ProcessTable &procTab
    );
    //
    void
    init(bool beVerbose = false);
    //
    void
    finalize(void);

    /**
     * Sets MRNetFE verbosity.
     */
    void
    verbose(bool b) {
        mBeVerbose = b;
    }
    //
    static toolcommon::FEToBePackFnP
    getFEToBePackFun(void);
    //
    const toolcommon::LeafInfo &
    getLeafInfo(void) const {
        return mLeafInfo;
    }

    /**
     *
     */
    MRN::Network *
    getNetwork(void) {
        return mNetwork;
    }
    /**
     *
     */
    MRN::Stream *
    getProtoStream(void) {
        return mProtoStream;
    }
    //
    int
    connect(void);
    //
    void
    networkInit(void);
    //
    void
    handshake(void);
    //
    void
    pluginInfoBCast(
        const std::string &validPluginName,
        const std::string &pathToValidPlugin
    );
};

} // end mrnetfe namespace
} // end gladius namespace

#endif
