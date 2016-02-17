/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-proto.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include "mrnet/MRNet.h"

using namespace MRN;
using namespace std;

namespace {

void
usage(void)
{
    cout << "usage: toolFE toolBE" << endl;
}

string
getTopology(void)
{
    return "localhost:0 => localhost:1 ;";
}

Network *
buildNetwork(string beExe)
{
    Network *net = NULL;
    string topology = getTopology();
    map<string, string> attributes;

    const char *argv[3] = {
        "arg0", "arg1", NULL
    };

    net = Network::CreateNetworkFE(
              topology.c_str(), // topology
              beExe.c_str(), // path to back-end exe
              argv, // back-end argv
              &attributes, // Network attributes
              true, // rank back-ends (start from 0)
              true // topology in memory buffer, not a file
          );
    assert(net);

    return net;
}

}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }

    cout << "*** starting ***" << endl;

    Network *net = buildNetwork(string(argv[1]));
    assert(net);

    Communicator *commWorld = net->get_BroadcastCommunicator();
    assert(commWorld);

    size_t nBackends = commWorld->get_EndPoints().size();
    cout << "*** number of back-ends: " << nBackends << endl;

    Stream *stream = net->new_Stream(
                         commWorld,
                         TFILTER_SUM,
                         SFILTER_WAITFORALL,
                         TFILTER_NULL
                     );
    assert(stream);
    assert(stream->send(PROTO_PING, "%d", 7) != -1);
    assert(stream->flush() != -1);

    int tag = 0;
    PacketPtr packet;
    assert(stream->recv(&tag, packet) != -1);
    assert(tag == PROTO_PING);
    int res = 0;
    packet->unpack("%d", &res);

    cout << "sum of all back-end ranks is: " << res << endl;

    cout << "shutting down" << endl;

    assert(stream->send(PROTO_EXIT, "") != -1);
    assert(stream->flush() != -1);

    sleep(1);
    delete net;

    return EXIT_SUCCESS;
}
