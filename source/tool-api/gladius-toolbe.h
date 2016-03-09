/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <cstring>

#include <limits.h>

namespace gladius {
namespace toolbe {

// Forward declarations.
struct ToolConnectionInfo;
struct ToolNetwork;
struct ThreadPersonality;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Tool {
    static constexpr int sNOUID = -1;
    // Our unique identifier in the parallel job.
    int mUID;
    // Total number of participants in this job.
    int mTargetCount;
    // Host's name
    std::string mHostName;
    //
    std::string mSessionKey;
    // Opaque handle to connection information.
    ToolConnectionInfo *mtli;
    // Buffers for stringified connection info.
    char mParentHostname[HOST_NAME_MAX];
    char mParentPort[16];
    char mParentRank[16];
    // Opaque handle to tool network.
    ToolNetwork *mNet;
    // Pool of tool threads.
    std::vector<std::thread> mToolThreads;
    //
    int
    mConnect(void);
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
public:
    /**
     *
     */
    Tool(
        void
    ) : mUID(sNOUID)
      , mTargetCount(0)
      , mtli(nullptr)
      , mNet(nullptr)
    {
        memset(mParentHostname, 0, sizeof(mParentHostname));
        memset(mParentPort,     0, sizeof(mParentPort));
        memset(mParentRank,     0, sizeof(mParentRank));
    }
    /**
     *
     */
    ~Tool(void) {
        if (mtli) free(mtli);
    }
    //
    int
    create(int uid);
    /**
     *
     */
    int
    connect(void) { return mConnect(); }
};

} // namespace toolbe
} // gladius
