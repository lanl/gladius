/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "mrnet-fe.h"
#include "core/core.h"
#include "core/utils.h"
#include "core/session.h"
#include "core/colors.h"

#include "mrnet/MRNet.h"

#include <string>

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

/**
 * Constructor.
 */
MRNetFE::MRNetFE(void) { ; }

/**
 * Destructor.
 */
MRNetFE::~MRNetFE(void) { ; }

/**
 * Initialization.
 */
void
MRNetFE::init(void)
{
    using namespace std;
    try {
        if (mBeVerbose) {
            COMP_COUT << "Initializing MRNet Front-End." << endl;
        }
        mSessionDir = core::Session::TheSession().sessionDir();
        mTopoFile = "/home/samuel/.gladiusrc/topo";
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
MRNetFE::createNetworkFE(void)
{
    const char *dummyArgv = NULL;
    mNetwork = MRN::Network::CreateNetworkFE(
                   mTopoFile.c_str(),
                   mBEExe.c_str(),
                   &dummyArgv
               );
}
