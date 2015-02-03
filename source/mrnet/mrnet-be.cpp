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
