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
using namespace gladius::mrnetbe;

namespace {
// This component's name.
const std::string CNAME = "mrnetbe";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::NONE);
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
MRNetBE::~MRNetBE(void) = default;

/**
 *
 */
int
MRNetBE::init(
    bool beVerbose
) {
    using namespace std;
    using namespace gladius::core;
    //
    mBeVerbose = beVerbose;
    VCOMP_COUT("Initializing..." << endl);
    //
    mHostName = core::utils::getHostname();
    //
    struct sockaddr_in *sinp = NULL;
    struct addrinfo *addinf = NULL;
    auto rc = getaddrinfo(
                  mHostName.c_str(),
                  NULL,
                  NULL,
                  &addinf
              );
    if (rc) {
        const string rcs = to_string(rc);
        GLADIUS_CERR << utils::formatCallFailed(
                            "getaddrinfo(3): " + rcs,
                            GLADIUS_WHERE
                        )
                     << std::endl;
        return GLADIUS_ERR_SYS;
    }
    //
    sinp = (struct sockaddr_in *)addinf->ai_addr;
    if (!sinp) {
        GLADIUS_CERR << "Cannot Get addinf->ai_addr" << endl;
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
                     << std::endl;
        return GLADIUS_ERR_SYS;
    }
    mLocalIP = std::string(ntopRes);
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
                     << std::endl;
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
             << std::endl;
        // Warning only. Just return success...
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
    //
    // TODO FIXME when we want more than one thread per target.
    const size_t nThreads = 1;
    ToolLeafInfoArrayT *tli = (ToolLeafInfoArrayT *)mtli;
    // Not supported yet...
    if (tli->size != 1) {
        GLADIUS_CERR << "Multiple targets not supported..." << endl;
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
            std::thread(&MRNetBE::mToolThreadMain, this, tp)
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
MRNetBE::mToolThreadMain(
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

    mNet = Network::CreateNetworkBE(tp->argc, tp->argv);
    if (!mNet || mNet->has_Error()) {
        GLADIUS_CERR << core::utils::formatCallFailed(
                            "MRN::Network::CreateNetworkBE",
                            GLADIUS_WHERE
                        )
                     << std::endl;
        return GLADIUS_ERR_MRNET;
    }
    //
    return GLADIUS_SUCCESS;
}

#if 0
/**
 *
 */
void
MRNetBE::handshake(void)
{
    VCOMP_COUT("Starting Lash-Up Handshake." << std::endl);

    MRN::PacketPtr packet;
    const bool recvShouldBlock = true;
    int tag = 0;
    // This will setup the protocol stream.
    auto status = mNetwork->recv(&tag, packet, &mProtoStream, recvShouldBlock);
    if (1 != status) {
        GLADIUS_THROW_CALL_FAILED("Network::Recv");
    }
    int ping = -1;
    status = packet->unpack("%d", &ping);
    if (0 != status) {
        GLADIUS_THROW_CALL_FAILED("PacketPtr::unpack");
    }
    if (toolcommon::MRNetCoreTags::InitHandshake != tag) {
        GLADIUS_THROW("Received Invalid Tag From Tool Front-End");
    }
    if (ping != GladiusMRNetProtoFilterMagic) {
        GLADIUS_THROW("Received Invalid Data From Tool Front-End");
    }
    int pong = -ping;
    status = mProtoStream->send(tag, "%d", pong);
    if (-1 == status) {
        GLADIUS_THROW_CALL_FAILED("Stream::Send");
    }
    mProtoStream->flush();

    VCOMP_COUT("Done with Lash-Up Handshake." << std::endl);
}

/**
 * Receives valid plugin name and path from FE.
 */
void
MRNetBE::pluginInfoRecv(
    std::string &validPluginName,
    std::string &pathToValidPlugin
) {
    VCOMP_COUT("Receiving Plugin Info from Front-End." << std::endl);
    //
    MRN::PacketPtr packet;
    MRN::Stream *stream = nullptr;
    const bool recvShouldBlock = true;
    int tag = 0;
    auto status = mNetwork->recv(&tag, packet, &stream, recvShouldBlock);
    if (1 != status) {
        GLADIUS_THROW_CALL_FAILED("Network::Recv");
    }
    // Make sure that we are dealing with a tag that we are expecting...
    if (toolcommon::MRNetCoreTags::PluginNameInfo != tag) {
        GLADIUS_THROW("Received Invalid Tag From Tool Front-End");
    }
    char *pluginName = nullptr;
    char *pluginPath = nullptr;
    status = packet->unpack("%s %s", &pluginName, &pluginPath);
    if (0 != status) {
        GLADIUS_THROW_CALL_FAILED("PacketPtr::unpack");
    }
    // Set returns.
    validPluginName = std::string(pluginName);
    pathToValidPlugin = std::string(pluginPath);
    //
    free(pluginName);
    free(pluginPath);
    //
    VCOMP_COUT("Front-End Plugin Info:" << std::endl);
    VCOMP_COUT("*Name: " << validPluginName << std::endl);
    VCOMP_COUT("*Path: " << pathToValidPlugin << std::endl);
    //
    VCOMP_COUT("Done Receiving Plugin Info from Front-End." << std::endl);
}
#endif
