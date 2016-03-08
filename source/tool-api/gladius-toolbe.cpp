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
    cout << mSessionKey << endl;
    //
    char *tmpDir = getenv("TMPDIR");
    if (!tmpDir) {
        tmpDir = (char *)"/tmp";
    }
    ////////////////////////////////////////////////////////////////////////////
    // NOTE: this is to be kept in sync with dsys.cpp
    ////////////////////////////////////////////////////////////////////////////
    string infoFile = string(tmpDir) + "/"
                    + string(mSessionKey) + "-"
                    + to_string(mUID);
    //
    size_t fileSize = 0;
    int rc = core::utils::getSizeOfFile(infoFile, fileSize);
    if (GLADIUS_SUCCESS != rc) return rc;
    cout << "FileSize: " << fileSize << endl;
    // Sanity
    if (0 != fileSize % sizeof(ToolLeafInfoT)) {
        CERRLN("Connection info inconsistency!");
        return GLADIUS_ERR_IO;
    }
    // Determine number of targets.
    mTargetCount = fileSize / sizeof(ToolLeafInfoT);
    cout << "N Targets: " << mTargetCount << endl;
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
#if 1 // DEBUG
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
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
Tool::create(
    int uid
) {
    mUID = uid;
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
    //
    return GLADIUS_SUCCESS;
}
