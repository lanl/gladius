/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
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

#include "mrnet/MRNet.h"
#include "xplat/NetUtils.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


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
MRNetBE::MRNetBE(void)
{
    ;
}

/**
 * Destructor.
 */
MRNetBE::~MRNetBE(void)
{
    ;
}

/**
 *
 */
void
MRNetBE::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
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
        GLADIUS_THROW_CALL_FAILED_RC("getaddrinfo", rc);
    }
    sinp = (struct sockaddr_in *)addinf->ai_addr;
    if (!sinp) {
        GLADIUS_THROW("Cannot Get addinf->ai_addr");
    }
    char abuf[INET_ADDRSTRLEN];
    const char *ntopRes = inet_ntop(
                              AF_INET,
                              &sinp->sin_addr,
                              abuf,
                              INET_ADDRSTRLEN
                          );
    if (!ntopRes) {
        GLADIUS_THROW_CALL_FAILED("inet_ntop");
    }
    mLocalIP = std::string(ntopRes);
    if (addinf) freeaddrinfo(addinf);
}

/**
 *
 */
void
MRNetBE::setPersonality(
    const toolbecommon::ToolLeafInfoArrayT &tlia
) {
    VCOMP_COUT("Finding My MRNet Personality." << std::endl);

    std::string prettyHost;
    XPlat::NetUtils::GetHostName(mHostName, prettyHost);

    bool found = false;
    for (decltype(tlia.size) i = 0; i < tlia.size; i++) {
        std::string leafPrettyHost;
        XPlat::NetUtils::GetHostName(
            std::string(tlia.leaves[i].hostName),
            leafPrettyHost
        );
        if (prettyHost == leafPrettyHost
            || leafPrettyHost == mLocalIP) {
            found = true;
            mParentHostname = std::string(tlia.leaves[i].parentHostName);
            mParentPort = tlia.leaves[i].parentPort;
            mParentRank = tlia.leaves[i].parentRank;
            mRank = tlia.leaves[i].rank;
            break;
        }
    }
    if (!found) {
        GLADIUS_THROW("Failed to Find MRNet Parent Info");
    }

    VCOMP_COUT("Done Finding My MRNet Personality." << std::endl);
}

/**
 *
 */
void
MRNetBE::connect(void)
{
    VCOMP_COUT("Connecting to MRNet Network." << std::endl);

    static const int bufSize = 1024;
    char parentPort[bufSize];
    memset(parentPort, 0, sizeof(parentPort));
    snprintf(parentPort, sizeof(parentPort), "%d", mParentPort);
    char parentRank[bufSize];
    memset(parentRank, 0, sizeof(parentRank));
    snprintf(parentRank, sizeof(parentRank), "%d", mParentRank);
    char rank[bufSize];
    memset(rank, 0, sizeof(rank));
    snprintf(rank, sizeof(rank), "%d", mRank);
    const int argc = 6;
    char *argv[argc];
    argv[0] = NULL;
    argv[1] = (char *)mParentHostname.c_str();
    argv[2] = parentPort;
    argv[3] = parentRank;
    argv[4] = (char *)mHostName.c_str();
    argv[5] = rank;
    //
    mNetwork = MRN::Network::CreateNetworkBE(argc, argv);
    if (!mNetwork) {
        GLADIUS_THROW_CALL_FAILED("MRN::Network::CreateNetworkBE");
    }
}

/**
 *
 */
void
MRNetBE::handshake(void)
{
    VCOMP_COUT("Starting Lash-Up Handshake." << std::endl);

    MRN::PacketPtr packet;
    MRN::Stream *stream = nullptr;
    const bool recvShouldBlock = true;
    int tag = 0;
    auto status = mNetwork->recv(&tag, packet, &stream, recvShouldBlock);
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
    status = stream->send(tag, "%d", pong);
    if (-1 == status) {
        GLADIUS_THROW_CALL_FAILED("Stream::Send");
    }
    stream->flush();

    VCOMP_COUT("Done with Lash-Up Handshake." << std::endl);
}

/**
 * Receives valid plugin name and path from FE.
 */
void
MRNetBE::pluginInfoRecv(
    std::string &validPluginName,
    std::string &pathToValidPlugin
)
{
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
    pathToValidPlugin= std::string(pluginPath);
    VCOMP_COUT(
        "Loading the Following Plugin:" << std::endl <<
        " * Name: " << validPluginName << std::endl <<
        " * Path: " << pathToValidPlugin << std::endl
    );
    //
    VCOMP_COUT("Done Receiving Plugin Info from Front-End." << std::endl);
}
