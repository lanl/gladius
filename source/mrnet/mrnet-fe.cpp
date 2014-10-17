/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * University of Illinois/NCSA Open Source License
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 *
 * For more details, please see LICENSE.TXT in the LLDB distirubtion.
 */

#include "mrnet-fe.h"
#include "core/core-includes.h"

#include "mrnet/MRNet.h"

#include <string>

using namespace gladius;
using namespace gladius::mrnet;

/**
 *
 */
MRNetFE::MRNetFE(void)
{
    try {
        // FIXME. We can't have this sort of thing becuse of multiple
        // allocations on a cluster. mktmp or something.
        topoFile = "/home/samuel/.gladiusrc/topo";
        const char *dummyArgv = NULL;
        network = MRN::Network::CreateNetworkFE(
                      topoFile.c_str(),
                      beExe.c_str(),
                      &dummyArgv
                  );

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
