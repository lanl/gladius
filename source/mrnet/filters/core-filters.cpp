/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "mrnet/Packet.h"
#include "mrnet/NetworkTopology.h"

#include <vector>

using namespace std;
using namespace MRN;

/**
 *
 */
extern "C" {

////////////////////////////////////////////////////////////////////////////////
// Filter Init
////////////////////////////////////////////////////////////////////////////////
const char *GladiusMRNetFilterInit_format_string = "%ac";

/**
 *
 */
void
GladiusMRNetFilterInit(
    vector<PacketPtr> &inputPackets,
    vector<PacketPtr> &outputPackets,
    vector<PacketPtr> &outputPacketsReverse,
    void **filterState,
    PacketPtr &params,
    const TopologyLocalInfo &topology
) {
    std::cout << "FILTER!!!" << std::endl;
}

}
