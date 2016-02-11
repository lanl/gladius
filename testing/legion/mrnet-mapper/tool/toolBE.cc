/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "net-proto.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include "mrnet/MRNet.h"

using namespace MRN;
using namespace std;

int
main(int argc, char **argv)
{
    Network *net = MRN::Network::CreateNetworkBE( argc, argv );
    assert(net);

    int myRank = net->get_LocalRank();

    cout << "hi from mrnet rank " << myRank << endl;

    Stream *stream = NULL;
    MRN::PacketPtr packetPtr;
    int tag = 0;
    static const bool blocks = true;
    int rxrc = net->recv(&tag, packetPtr, &stream, blocks);
    assert(rxrc != -1);

    cout << "rank " << myRank << " got tag " << tag << endl;

    int fromFE = 0;
    packetPtr->unpack("%d", &fromFE);

    cout << "rank " << myRank << " got val " << fromFE << endl;

    assert(stream->send(tag, "%d", myRank) != -1);
    assert(stream->flush() != -1);

    assert(stream->recv(&tag, packetPtr) != -1);
    assert(tag == PROTO_EXIT);

    net->waitfor_ShutDown();
    delete net;

    return EXIT_SUCCESS;
}
