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
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        std::cout << streamInsertions;                                         \
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
) {
    ;
}

/**
 * Destructor.
 */
MRNetBE::~MRNetBE(void)
{
}

/**
 *
 */
void
MRNetBE::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    //
    struct sockaddr_in *sinp = NULL;
    struct addrinfo *addinf = NULL;
    auto rc = getaddrinfo(
                  core::utils::getHostname().c_str(),
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
    const char *ntopRes = inet_ntop(AF_INET,
                                    &sinp->sin_addr,
                                    abuf,
                                    INET_ADDRSTRLEN
                          );
    if (!ntopRes) {
        GLADIUS_THROW_CALL_FAILED("inet_ntop");
    }
    snprintf(mLocalIPBuff, sizeof(mLocalIPBuff), "%s", ntopRes);
}

/**
 *
 */
void
MRNetBE::setPersonality(
    const toolbecommon::ToolLeafInfoArrayT &tlia
) {
    VCOMP_COUT("Finding My MRNet Personality" << std::endl);
    std::string localHost = core::utils::getHostname();
    std::string prettyHost;
    XPlat::NetUtils::GetHostName(localHost, prettyHost);

    bool found = false;
    for (decltype(tlia.size) i = 0; i < tlia.size; i++) {
        std::string leafPrettyHost;
        XPlat::NetUtils::GetHostName(
            std::string(tlia.leaves[i].hostName),
            leafPrettyHost
        );
        if (prettyHost == leafPrettyHost
            || strcmp(leafPrettyHost.c_str(), mLocalIPBuff) == 0) {
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
}
