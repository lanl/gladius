/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-api/gladius-toolbe.h"

#include "core/utils.h"
#include "core/gladius-rc.h"
#include "tool-common/gladius-tli.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#include "mrnet/MRNet.h"

using namespace std;
using namespace gladius;
using namespace gladius::core;
using namespace gladius::toolbe;
using namespace gladius::toolcommon;

namespace {
static const char *sCompName = "[gladius-toolbe]";
static const char *sSessionKeyEnvName = "GLADIUS_SESSION_KEY";

#define CERRLN(streamInsertions)                                               \
do {                                                                           \
    std::cerr << sCompName << " " << streamInsertions << std::endl;            \
} while (0)

} // namespace

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct gladius::toolbe::ThreadPersonality {
    int rank = 0;
    static constexpr int argc = 6;
    char *argv[argc];
};

/**
 *
 */
int
Tool::mGetConnectionInfo(void)
{
    const char *sKey = getenv(sSessionKeyEnvName);
    if (!sKey) {
        CERRLN("Cannot connect: Session key not found!");
        return GLADIUS_ERR;
    }
    mSessionKey = string(sKey);
    //
    char *tmpDir = getenv("TMPDIR");
    if (!tmpDir) {
        tmpDir = (char *)"/tmp";
    }
    ////////////////////////////////////////////////////////////////////////////
    // NOTE: this naming scheme is to be kept in sync with dsys.cpp
    ////////////////////////////////////////////////////////////////////////////
    string infoFile = string(tmpDir) + "/"
                    + string(mSessionKey) + "-"
                    + to_string(mUID);
    //
    size_t fileSize = 0;
    int rc = core::utils::getSizeOfFile(infoFile, fileSize);
    if (GLADIUS_SUCCESS != rc) return rc;
    // Sanity
    if (0 != fileSize % sizeof(ToolLeafInfoT)) {
        CERRLN("Connection info inconsistency!");
        return GLADIUS_ERR_IO;
    }
    // Determine number of targets.
    mTargetCount = fileSize / sizeof(ToolLeafInfoT);
    // Sanity
    if (mTargetCount <= 0) {
        CERRLN("Error determining number of targets... Got: " << mTargetCount);
        return GLADIUS_ERR;
    }
    // Not supported (yet)
    if (mTargetCount > 1) {
        CERRLN("Multiple targets not supported...");
        return GLADIUS_ERR;
    }
    //
    mtli = (ToolConnectionInfo *)calloc(1, sizeof(ToolLeafInfoArrayT));
    if (!mtli) {
        CERRLN("Out of resources!");
        return GLADIUS_ERR_OOR;
    }
    //
    ToolLeafInfoArrayT *tli = (ToolLeafInfoArrayT *)mtli;
    tli->size = mTargetCount;
    tli->leaves = (ToolLeafInfoT *)calloc(mTargetCount, sizeof(ToolLeafInfoT));
    if (!tli->leaves) {
        CERRLN("Out of resources!");
        return GLADIUS_ERR_OOR;
    }
    //
    FILE *connectionInfo = fopen(infoFile.c_str(), "rb");
    if (!connectionInfo) {
        int err = errno;
        const string errs = utils::getStrError(err);
        CERRLN(utils::formatCallFailed("fopen(3): " + errs, GLADIUS_WHERE));
        return GLADIUS_ERR_IO;
    }
    const int nItemsRead = fread(tli->leaves,
                                 sizeof(ToolLeafInfoT),
                                 mTargetCount,
                                 connectionInfo
                           );
    if (nItemsRead != mTargetCount) {
        cerr << utils::formatCallFailed("fread(3): ", GLADIUS_WHERE)
             << std::endl;
        return GLADIUS_ERR_IO;
    }
#if 0 // DEBUG
    for (int i = 0; i < mTargetCount; ++i) {
        cout << "ToolLeafInfoT "       << i                             << endl
             << "- Parent Host Name: " << tli->leaves[i].parentHostName << endl
             << "- Parent Rank     : " << tli->leaves[i].parentRank     << endl
             << "- Parent Port     : " << tli->leaves[i].parentPort     << endl;
    }
#endif
    if (0 != fclose(connectionInfo)) {
        cerr << utils::formatCallFailed("fclose(3): ", GLADIUS_WHERE)
             << std::endl;
        // Warning only. Just return success...
    }
    // Now stash the string version of the info
    snprintf(
        mParentHostname,
        sizeof(mParentHostname),
        "%s",
        tli->leaves[0].parentHostName
    );
    snprintf(
        mParentRank,
        sizeof(mParentRank),
        "%d",
        tli->leaves[0].parentRank
    );
    snprintf(
        mParentPort,
        sizeof(mParentPort),
        "%d",
        tli->leaves[0].parentPort
    );
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
Tool::mStartToolThreads(void)
{
    // TODO FIXME when we want more than one thread per target.
    const size_t nThreads = 1;
    ToolLeafInfoArrayT *tli = (ToolLeafInfoArrayT *)mtli;
    // Not supported yet...
    if (tli->size != 1) {
        CERRLN("Multiple targets not supported...");
        return GLADIUS_ERR;
    }
    for (size_t i = 0; i < nThreads; ++i) {
        ThreadPersonality *tp = new ThreadPersonality();
        // TODO FIXME: calculate proper rank.
        tp->rank = (10000 * (i + 1)) + mUID;
        // TODO FIXME: get real exec
        tp->argv[0] = (char *)"./toolBE";
        tp->argv[1] = mParentHostname; 
        tp->argv[2] = mParentPort;
        tp->argv[3] = mParentRank; 
        tp->argv[4] = (char *)mHostName.c_str();
        mToolThreads.push_back(
            std::thread(&Tool::mToolThreadMain, this, tp)
        );
    }
    mToolThreads[0].join();
#if 0
    tc.waitForAttach();
    tc.inMapper->WaitOnCondition(ToolContext::ToolConditions::MAP);
#endif
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
Tool::mToolThreadMain(
    ThreadPersonality *tp
) {
    using namespace MRN;
    // Finish populating argv (thread local)
    char rankStr[64];
    snprintf(rankStr, sizeof(rankStr), "%d", tp->rank);
    tp->argv[5] = rankStr;
#if 0 // DEBUG
    cerr << "Thread " << tp->rank << endl
         << "-- Parent Host Name: " << tp->argv[1] << endl
         << "-- Parent Port     : " << tp->argv[2] << endl
         << "-- Parent Rank     : " << tp->argv[3] << endl
         << "-- Host Name       : " << tp->argv[4] << endl
         << "-- Rank            : " << tp->argv[5] << endl
         << flush;
#endif
    // Sanity
    assert(6 == tp->argc);

    Network *net = Network::CreateNetworkBE(tp->argc, tp->argv);
    //mNet = (ToolNetwork *)net;
    //
    assert(net);
    assert(!net->has_Error());
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
Tool::mConnect(void)
{
    int rc = mGetConnectionInfo();
    if (GLADIUS_SUCCESS != rc) return rc;
    if (GLADIUS_SUCCESS != (rc = mStartToolThreads())) return rc;
    //
    return GLADIUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Public API
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
int
Tool::create(
    int uid
) {
    mUID = uid;
    mHostName = utils::getHostname();
    return GLADIUS_SUCCESS;
}
