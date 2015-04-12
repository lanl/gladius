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
// Filter Init & Tool FE <--> Tool BE Protocol Filter
////////////////////////////////////////////////////////////////////////////////
const char *GladiusMRNetProtoFilter_format_string = "%d";

/**
 *
 */
void
GladiusMRNetProtoFilter(
    vector<PacketPtr> &inputPackets,
    vector<PacketPtr> &outputPackets,
    vector<PacketPtr> &,
    void **,
    PacketPtr &
) {
    //
    outputPackets.push_back(inputPackets[0]);
}

}
