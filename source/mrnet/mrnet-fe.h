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
#include "tool-common/tool-common.h"

#include <string>

namespace gladius {
namespace mrnet {
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
    toolcommon::Hosts mHosts;
    //
    std::string
    mGenFlatTopo(void);
public:
    //
    MRNetTopology(void) { ; }
    //
    ~MRNetTopology(void) { ; }
    //
    MRNetTopology(
        const std::string &topoFilePath,
        TopologyType topoType,
        const std::string &feHostName,
        const toolcommon::Hosts &hosts
    );
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Implements the MRNet interface for a tool front-end.
 */
class MRNetFE {
private:
    // Be verbose or not.
    bool mBeVerbose = false;
    // Base session directory.
    std::string mSessionDir;
    // Absolute path to MRNet installation.
    static std::string sInstallPrefix;
    // Path to MRNet topology file.
    std::string mTopoFile;
    // Name of the backend executable
    std::string mBEExe;
    // The MRNet network instance.
    MRN::Network *mNetwork = nullptr;

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
        const toolcommon::Hosts &hosts
    );
    //
    void
    init(void);
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
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MRNetBE {
};

} // end mrnet namespace
} // end gladius namespace

#endif
