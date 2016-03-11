/**
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller
 * Detailed MRNet usage rights in "LICENSE" file in the MRNet distribution.
 *
 */

#pragma once

#include "tool-common/tool-common.h"

#include <vector>
#include <thread>

// Forward declarations
namespace MRN {
    class Network;
    class Stream;
}

namespace gladius {
namespace mrnetbe {
////////////////////////////////////////////////////////////////////////////////
// Container for tool thread personalities.
////////////////////////////////////////////////////////////////////////////////
struct ThreadPersonality {
    int rank = 0;
    static constexpr int argc = 6;
    const char *argv[argc];
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Implements the MRNet interface for a tool back-end.
 */
class MRNetBE {
private:
    //  Constant indicating that we don't yet have a unique ID.
    static constexpr int sNOUID = -1;
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
    // Our unique identifier in the parallel job.
    int mUID;
    // Total number of participants in this job.
    int mTargetCount;
    // Host's name
    std::string mHostName;
    // Absolute path to target application in which we are embedded.
    std::string mHostExecPath;
    // Host's local IP
    std::string mLocalIP;
    //
    std::string mSessionKey;
    // Pointer to connection information.
    toolcommon::ToolLeafInfoArrayT *mtli;
    // Buffers for stringified connection info.
    char mParentHostname[HOST_NAME_MAX];
    char mParentPort[16];
    char mParentRank[16];
    // Handle to tool network.
    MRN::Network *mNet;
    // Tool OOB protocol stream.
    MRN::Stream *mProtoStream = nullptr;
    // Pool of tool threads.
    std::vector<std::thread> mToolThreads;
    //
    int
    mGetConnectionInfo(void);
    //
    int
    mStartToolThreads(void);
    //
    int
    mToolThreadMain(
        ThreadPersonality *tp
    );
    //
    int
    mHandshake(void);
    //
    int
    mPluginInfoRecv(
        std::string &validPluginName,
        std::string &pathToValidPlugin
    );

public:
    //
    MRNetBE(void);
    //
    ~MRNetBE(void);
    //
    int
    init(bool beVerbose);
    //
    int
    create(int uid);
    //
    int
    connect(void);
};

} // end mrnetbe namespace
} // end gladius namespace
