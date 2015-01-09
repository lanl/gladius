/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "mrnet-fe.h"
#include "core/core.h"

#include "mrnet/MRNet.h"

#include <string>

using namespace gladius;
using namespace gladius::mrnet;

/**
 * Constructor.
 */
MRNetFE::MRNetFE(void)
{
    try {
        // FIXME. We can't have this sort of thing becuse of multiple
        // allocations on a cluster. mktmp or something.
        mTopoFile = "/home/samuel/.gladiusrc/topo";
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
MRNetFE::~MRNetFE(void)
{
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
