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
    PacketPtr &params
) {
    int data = -1;
    params->unpack("%d", &data);
    std::cout << "FILTER GOT: " << data << std::endl;
    outputPackets.push_back(inputPackets[0]);
}

}
