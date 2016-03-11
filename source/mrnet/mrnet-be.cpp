/**
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "mrnet/mrnet-be.h"
#include "mrnet/filters/core-filters.h"

#include "core/core.h"
#include "core/utils.h"
#include "tool-common/tool-common.h"

#include <cstdlib>

#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "mrnet/MRNet.h"
#include "xplat/NetUtils.h"

using namespace std;
using namespace gladius;
using namespace gladius::core;
using namespace gladius::mrnetbe;

namespace {
// This component's name.
const string CNAME = "mrnetbe";
// CNAME's color code.
const string NAMEC = core::colors::color().ansiBeginColor(core::colors::NONE);
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
// MRNetBE
////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
MRNetBE::MRNetBE(
    void
) : mBeVerbose(false)
  , mUID(sNOUID)
  , mTargetCount(0)
  , mtli(nullptr)
  , mNet(nullptr)
{
    memset(mParentHostname, 0, sizeof(mParentHostname));
    memset(mParentPort,     0, sizeof(mParentPort));
    memset(mParentRank,     0, sizeof(mParentRank));
}

/**
 * Destructor.
 */
MRNetBE::~MRNetBE(void) {
    // FIXME
    mToolThreads[0].join();
    if (mtli) {
        if (mtli->leaves) free(mtli->leaves);
        mtli->leaves = nullptr;
        free(mtli);
        mtli = nullptr;
    }
}

/**
 *
 */
int
MRNetBE::init(
    bool beVerbose
) {
    using namespace gladius::core;
    //
    mBeVerbose = beVerbose;
    VCOMP_COUT("Initializing..." << endl);
    //
    mHostName = core::utils::getHostname();
    //
    int rc = mSetLocalIP();
    if (GLADIUS_SUCCESS != rc) return rc;
    //
    if (GLADIUS_SUCCESS != (rc = mSetSelfPath())) return rc;
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::mSetSelfPath(void)
{
    int err = 0;
    int rc = core::utils::getSelfPath(mHostExecPath, err);
    if (GLADIUS_SUCCESS != rc) {
        GLADIUS_CERR << core::utils::formatCallFailed(
                            "getSelfPath: " + core::utils::getStrError(err),
                            GLADIUS_WHERE
                        )
                     << endl;
    }
    return rc;
}

/**
 *
 */
int
MRNetBE::mSetLocalIP(void)
{
    struct sockaddr_in *sinp = NULL;
    struct addrinfo *addinf = NULL;
    //
    int rc = getaddrinfo(mHostName.c_str(), NULL, NULL, &addinf);
    if (rc) {
        const string rcs = to_string(rc);
        GLADIUS_CERR << utils::formatCallFailed(
                            "getaddrinfo(3): " + rcs,
                            GLADIUS_WHERE
                        )
                     << endl;
        return GLADIUS_ERR_SYS;
    }
    //
    sinp = (struct sockaddr_in *)addinf->ai_addr;
    if (!sinp) {
        GLADIUS_CERR << "Cannot get addinf->ai_addr" << endl;
        return GLADIUS_ERR_SYS;
    }
    //
    char abuf[INET_ADDRSTRLEN];
    const char *ntopRes = inet_ntop(
                              AF_INET,
                              &sinp->sin_addr,
                              abuf,
                              INET_ADDRSTRLEN
                          );
    if (!ntopRes) {
        int err = errno;
        const string errs = utils::getStrError(err);
        GLADIUS_CERR << utils::formatCallFailed(
                            "inet_ntop(3): " + errs,
                            GLADIUS_WHERE
                        )
                     << endl;
        return GLADIUS_ERR_SYS;
    }
    mLocalIP = string(ntopRes);
    if (addinf) freeaddrinfo(addinf);
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::create(int uid)
{
    mUID = uid;
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::connect(void)
{
    VCOMP_COUT("Connecting..." << endl);
    //
    int rc = mGetConnectionInfo();
    if (GLADIUS_SUCCESS != rc) return rc;
    if (GLADIUS_SUCCESS != (rc = mStartToolThreads())) return rc;
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::mGetConnectionInfo(void)
{
    using namespace gladius::toolcommon;
    //
    const char *sKey = getenv(GLADIUS_ENV_GLADIUS_SESSION_KEY);
    if (!sKey) {
        GLADIUS_CERR << "Cannot connect: Session key not found!" << endl;
        return GLADIUS_ERR;
    }
    mSessionKey = string(sKey);
    //
    const auto tmpDir = core::utils::getTmpDir();
    ////////////////////////////////////////////////////////////////////////////
    // NOTE: this naming scheme is to be kept in sync with dsys.cpp
    ////////////////////////////////////////////////////////////////////////////
    string infoFile = tmpDir + core::utils::osPathSep
                    + string(mSessionKey) + "-"
                    + to_string(mUID);
    //
    size_t fileSize = 0;
    int rc = core::utils::getSizeOfFile(infoFile, fileSize);
    if (GLADIUS_SUCCESS != rc) return rc;
    // Sanity
    if (0 != fileSize % sizeof(ToolLeafInfoT)) {
        GLADIUS_CERR << "Connection info inconsistency!" << endl;
        return GLADIUS_ERR_IO;
    }
    // Determine number of targets.
    mTargetCount = fileSize / sizeof(ToolLeafInfoT);
    // Sanity
    if (mTargetCount <= 0) {
        GLADIUS_CERR << "Error determining number of targets... Got: "
                     << mTargetCount << endl;
        return GLADIUS_ERR;
    }
    // Not supported (yet)
    if (mTargetCount > 1) {
        GLADIUS_CERR << "Multiple targets not supported..." << endl;
        return GLADIUS_ERR;
    }
    //
    mtli = (ToolLeafInfoArrayT *)calloc(1, sizeof(ToolLeafInfoArrayT));
    if (!mtli) {
        GLADIUS_CERR << "Out of resources!" << endl;
        return GLADIUS_ERR_OOR;
    }
    //
    mtli->size = mTargetCount;
    mtli->leaves = (ToolLeafInfoT *)calloc(mTargetCount, sizeof(ToolLeafInfoT));
    if (!mtli->leaves) {
        GLADIUS_CERR << "Out of resources!" << endl;
        return GLADIUS_ERR_OOR;
    }
    //
    FILE *connectionInfo = fopen(infoFile.c_str(), "rb");
    if (!connectionInfo) {
        int err = errno;
        const string errs = core::utils::getStrError(err);
        GLADIUS_CERR << core::utils::formatCallFailed(
                            "fopen(3): " + errs, GLADIUS_WHERE
                        ) << endl;
        return GLADIUS_ERR_IO;
    }
    const int nItemsRead = fread(mtli->leaves,
                                 sizeof(ToolLeafInfoT),
                                 mTargetCount,
                                 connectionInfo
                           );
    if (nItemsRead != mTargetCount) {
        GLADIUS_CERR << core::utils::formatCallFailed(
                            "fread(3): ",
                            GLADIUS_WHERE
                        )
                     << endl;
        return GLADIUS_ERR_IO;
    }
#if 0 // DEBUG
    for (int i = 0; i < mTargetCount; ++i) {
        cout << "ToolLeafInfoT "       << i                              << endl
             << "- Parent Host Name: " << mtli->leaves[i].parentHostName << endl
             << "- Parent Rank     : " << mtli->leaves[i].parentRank    << endl
             << "- Parent Port     : " << mtli->leaves[i].parentPort    << endl;
    }
#endif
    if (0 != fclose(connectionInfo)) {
        cerr << core::utils::formatCallFailed("fclose(3): ", GLADIUS_WHERE)
             << endl;
        // Warning only. Don't return error.
    }
    // Now stash the string version of the info
    snprintf(
        mParentHostname,
        sizeof(mParentHostname),
        "%s",
        mtli->leaves[0].parentHostName
    );
    snprintf(
        mParentRank,
        sizeof(mParentRank),
        "%d",
        mtli->leaves[0].parentRank
    );
    snprintf(
        mParentPort,
        sizeof(mParentPort),
        "%d",
        mtli->leaves[0].parentPort
    );
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::mStartToolThreads(void)
{
    using namespace gladius::toolcommon;
    // Get name of host executable.
    // TODO FIXME when we want more than one thread per target.
    const size_t nThreads = 1;
    // Not supported yet...
    if (mtli->size != 1) {
        GLADIUS_CERR << "Multiple targets not supported..." << endl;
        return GLADIUS_ERR;
    }
    for (size_t i = 0; i < nThreads; ++i) {
        ThreadPersonality *tp = new ThreadPersonality();
        // TODO FIXME: calculate proper rank.
        tp->rank = (10000 * (i + 1)) + mUID;
        tp->argv[0] = mHostExecPath.c_str();
        tp->argv[1] = mParentHostname;
        tp->argv[2] = mParentPort;
        tp->argv[3] = mParentRank;
        tp->argv[4] = mHostName.c_str();
        mToolThreads.push_back(
            thread(&MRNetBE::mToolThreadMain, this, tp)
        );
    }
    // TODO RM
    mToolThreads[0].join();
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::mToolThreadMain(
    ThreadPersonality *tp
) {
    using namespace MRN;
    // Finish populating argv (thread local)
    char rankStr[64];
    memset(rankStr, 0, sizeof(rankStr));
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
    //
    mNet = Network::CreateNetworkBE(tp->argc, (char **)tp->argv);
    if (!mNet || mNet->has_Error()) {
        GLADIUS_CERR << core::utils::formatCallFailed(
                            "MRN::Network::CreateNetworkBE",
                            GLADIUS_WHERE
                        )
                     << endl;
        return GLADIUS_ERR_MRNET;
    }
    //
    int rc = mHandshake();
    if (GLADIUS_SUCCESS != rc) return rc;
    string p1, p2;
    rc = mPluginInfoRecv(p1, p2);
    if (GLADIUS_SUCCESS != rc) return rc;
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
MRNetBE::mHandshake(void)
{
    VCOMP_COUT("Starting lash-up handshake..." << endl);
    //
    MRN::PacketPtr packet;
    const bool recvShouldBlock = true;
    int tag = 0;
    // This will setup the protocol stream.
    auto status = mNet->recv(&tag, packet, &mProtoStream, recvShouldBlock);
    if (1 != status) {
        static const string f = "Stream::Recv";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    int ping = -1;
    status = packet->unpack("%d", &ping);
    if (0 != status) {
        static const string f = "PacketPtr::unpack";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    if (toolcommon::MRNetCoreTags::InitHandshake != tag) {
        static const string errs = "Received Invalid Tag From Tool Front-End";
        GLADIUS_CERR << errs << endl;
        return GLADIUS_ERR;
    }
    if (ping != GladiusMRNetProtoFilterMagic) {
        static const string errs = "Received Invalid Data From Tool Front-End";
        GLADIUS_CERR << errs << endl;
        return GLADIUS_ERR;
    }
    int pong = -ping;
    status = mProtoStream->send(tag, "%d", pong);
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
    //
    return GLADIUS_SUCCESS;
}

/**
 * Receives valid plugin name and path from FE.
 */
int
MRNetBE::mPluginInfoRecv(
    string &validPluginName,
    string &pathToValidPlugin
) {
    VCOMP_COUT("Receiving plugin info from front-end..." << endl);
    //
    MRN::PacketPtr packet;
    MRN::Stream *stream = nullptr;
    const bool recvShouldBlock = true;
    int tag = 0;
    auto status = mNet->recv(&tag, packet, &stream, recvShouldBlock);
    if (1 != status) {
        static const string f = "Network::Recv";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    // Make sure that we are dealing with a tag that we are expecting...
    if (toolcommon::MRNetCoreTags::PluginNameInfo != tag) {
        static const string errs = "Received Invalid Tag From Tool Front-End";
        GLADIUS_CERR << errs << endl;
        return GLADIUS_ERR;
    }
    char *pluginName = nullptr;
    char *pluginPath = nullptr;
    status = packet->unpack("%s %s", &pluginName, &pluginPath);
    if (0 != status) {
        static const string f = "PacketPtr::unpack";
        GLADIUS_CERR << utils::formatCallFailed(f, GLADIUS_WHERE) << endl;
        return GLADIUS_ERR_MRNET;
    }
    // Set returns.
    validPluginName = string(pluginName);
    pathToValidPlugin = string(pluginPath);
    //
    free(pluginName);
    free(pluginPath);
    //
    VCOMP_COUT("Front-End Plugin Info:" << endl);
    VCOMP_COUT("*Name: " << validPluginName << endl);
    VCOMP_COUT("*Path: " << pathToValidPlugin << endl);
    //
    return GLADIUS_SUCCESS;
}
