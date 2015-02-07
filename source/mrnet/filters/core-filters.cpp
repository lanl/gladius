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
const char *GladiusMRNetFilterInit_format_string = "%d";

// FIXME Add const TopologyLocalInfo &topology to end of sig.
/**
 *
 */
void
GladiusMRNetFilterInit(
    vector<PacketPtr> &inputPackets,
    vector<PacketPtr> &outputPackets,
    vector<PacketPtr> &,
    void **,
    PacketPtr &
) {
    std::cout << "FILTER!!!" << std::endl;
    std::cout << "FILTER DONE!!!" << std::endl;
    outputPackets.push_back(inputPackets[0]);
}

}
