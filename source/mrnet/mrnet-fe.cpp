/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "mrnet/mrnet-fe.h"

#include "core/core.h"
#include "core/utils.h"
#include "core/session.h"
#include "core/colors.h"
#include "tool-common/tool-common.h"

#include "mrnet/MRNet.h"

#include <string>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <unistd.h>

using namespace gladius;
using namespace gladius::mrnet;

namespace {
// This component's name.
static const std::string CNAME = "mrnetfe";
// CNAME's color code.
static const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::DGRAY);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
MRNetTopology::MRNetTopology(
    const std::string &topoFilePath,
    TopologyType topoType,
    const std::string &feHostName,
    const toolcommon::Hosts &hosts
) : mTopoFilePath(topoFilePath)
  , mTopoType(topoType)
  , mFEHostName(feHostName)
  , mHosts(hosts)
{
    using namespace std;
    ofstream theFile;
    try {
        // Create topology file.
        theFile.open(mTopoFilePath, ios::out | ios::trunc);
        string topoString;
        switch (mTopoType) {
            case FLAT:
                topoString = mGenFlatTopo();
                break;
            default:
                GLADIUS_THROW_INVLD_ARG();
        }
        // Populate the topology with the generated string.
        theFile << topoString;
        theFile.close();
    }
    catch (const std::exception &e) {
        if (theFile.is_open()) theFile.close();
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * Generates a 1xN (where N is the number of remote hosts) topology.
 * host:0 =>
 *   host:1
 *   host:2
 *   host:n ;
 */
// FIXME host names
std::string
MRNetTopology::mGenFlatTopo(void)
{
    using namespace std;
    auto id = 0;
    // The "host:0 =>" bit.
    string resTopoStr = "localhost: " + to_string(id++) + " =>\n";
    for (const auto &hostName : mHosts.hostNames()) {
        COMP_COUT << "HOST: " << hostName << std::endl;
        resTopoStr += "  " + hostName + ":" + to_string(id++) + "\n";
    }
    resTopoStr += ";";
    return resTopoStr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
const std::string MRNetFE::sCommNodeName = "mrnet_commnode";

/**
 * Constructor.
 */
MRNetFE::MRNetFE(
    void
) : mPrefixPath("") { ; }

/**
 * Destructor.
 */
MRNetFE::~MRNetFE(void) { ; }

/**
 * Sets some environment variables that impact the behavior of MRNetFE.
 */
void
MRNetFE::mSetEnvs(void)
{
    core::utils::setEnv("MRNET_RSH", "/usr/bin/ssh");
}

/**
 * Initialization.
 */
void
MRNetFE::init(
    bool beVerbose
) {
    using namespace std;
    using namespace core;
    string whatsWrong;
    try {
        mBeVerbose = beVerbose;
        if (mBeVerbose) {
            COMP_COUT << "Initializing MRNet Front-End." << endl;
        }
        if (!mDetermineAndSetPaths(whatsWrong)) {
            GLADIUS_THROW(whatsWrong);
        }
        mSetEnvs();
        mSessionDir = core::Session::TheSession().sessionDir();
        mTopoFile = mSessionDir + utils::osPathSep
                  + std::to_string(getpid()) + "-" + CNAME + ".topo";
        if (mBeVerbose) {
            COMP_COUT << "Topology Specification File: "
                      << mTopoFile << endl;
        }
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
std::string
MRNetFE::mGetPrefixFromCommNode(
    const std::string &whichString
) {
    std::string badness = "Could not determine MRNet's installation "
                          "prefix by inspecting the following path:"
                          "'" + whichString + "'";
    std::string prefix = whichString;
    std::string last = "/bin/" + sCommNodeName;
    auto found = prefix.rfind(last);
    // Not found, so something is wrong.
    if (std::string::npos == found) {
        GLADIUS_THROW(badness);
    }
    prefix = prefix.substr(0, found);
    return prefix;
}

/**
 *
 */
bool
MRNetFE::mDetermineAndSetPaths(
    std::string &whatsWrong
) {
    using namespace std;

    whatsWrong = "";
    if (mBeVerbose) {
        COMP_COUT << "Determining and Setting Paths." << std::endl;
    }
    // Make sure that we can find mrnet_commnode. Really just to get the base
    // MRNet installation path. That way we can find the libraries that we need.
    string cnPath;
    auto status = core::utils::which(sCommNodeName, cnPath);
    if (GLADIUS_SUCCESS != status) {
        whatsWrong = toolcommon::utils::genNotInPathErrString(sCommNodeName);
        return false;
    }
    // Now get MRNet's installation prefix.
    mPrefixPath = mGetPrefixFromCommNode(cnPath);
    return true;
}

/**
 *
 */
void
MRNetFE::finalize(void)
{
    using namespace std;
    try {
        if (mBeVerbose) {
            COMP_COUT << "Finalizing MRNet Front-End." << endl;
        }
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
MRNetFE::createNetworkFE(
    const toolcommon::Hosts &hosts
) {
    // First, create and populate MRNet network topology file.
    // TODO dynamic TopologyType based on job characteristics.
    if (mBeVerbose) {
        COMP_COUT << "Creating and Populating MRNet Topology" << std::endl;
    }
    MRNetTopology topo(
        mTopoFile,
        MRNetTopology::TopologyType::FLAT,
        core::utils::getHostname(),
        hosts
    );
    // Both NULL because MRNet is NOT going to be launching the tool daemons.
    const char *dummyBackendExe = NULL;
    const char *dummyArgv = NULL;
    mNetwork = MRN::Network::CreateNetworkFE(
                   mTopoFile.c_str(),
                   dummyBackendExe,
                   &dummyArgv
               );
    if (!mNetwork || mNetwork->has_Error()) {
        GLADIUS_THROW_CALL_FAILED("MRN::Network::CreateNetworkFE");
    }
}
