/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2003-2012 Dorian C. Arnold, Philip C. Roth, Barton P. Miller
 * Detailed MRNet usage rights in "LICENSE" file in the MRNet distribution.
 *
 */

/**
 * Implements the MRNet interface.
 */

#ifndef GLADIUS_MRNET_MRNET_FE_H_INCLUDED
#define GLADIUS_MRNET_MRNET_FE_H_INCLUDED

#include "mrnet/MRNet.h"

#include <string>

namespace gladius {
namespace mrnet {

class MRNetFE {
private:
    // Path to MRNet topology file that has already been populated
    std::string topoFile;
    // Name of the backend executable
    std::string beExe;
    MRN::Network *network = nullptr;

public:
    MRNetFE(void);

    ~MRNetFE(void);
};

} // end mrnet namespace
} // end gladius namespace

#endif
