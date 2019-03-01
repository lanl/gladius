/*
 * Copyright (c) 2016      Triad National Security, LLC
 *                         All rights reserved.
 *
 * Copyright (c) 2003-2015 Dorian C. Arnold, Philip C. Roth, Barton P. Miller
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-proto.h"

#include <iostream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <atomic>

#include "mrnet/MRNet.h"

using namespace std;
using namespace MRN;

namespace {

atomic<unsigned> gAtomicNumAttached{0};

void
usage(void)
{
    cout << "usage: toolFE" << endl;
}

string
getTopologyString(void)
{
    return "localhost:0 => localhost:1 ;";

#if 0
    return "localhost:0 =>"
           "  localhost:1"
           "  localhost:2 ;"
           ""
           "localhost:1 =>"
           "  localhost:3"
           "  localhost:4 ;"
           ""
           "localhost:2 =>"
           "  localhost:5"
           "  localhost:6 ;";
#endif
}

void
getNetworkTopology(
    Network *net,
    NetworkTopology **outTopo,
    vector<NetworkTopology::Node *> &outInternalLeaves
) {
    NetworkTopology *topology = net->get_NetworkTopology();
    assert(topology);
    *outTopo = topology;
    // TODO checks
    topology->get_Leaves(outInternalLeaves);
    // TODO add printout of stats, etc.
}

Network *
buildNetwork(void)
{
    Network *net = NULL;
    string topology = getTopologyString();

    net = Network::CreateNetworkFE(
              topology.c_str(), // topology
              NULL, // path to back-end exe
              NULL, // back-end argv
              NULL, // Network attributes
              true, // rank back-ends (start from 0)
              true  // topology in memory buffer, not a file
          );
    assert(net);

    return net;
}

void
beAddCallback(
    Event *event,
    void *evt_data
) {
    if ((event->get_Class() == Event::TOPOLOGY_EVENT) &&
        (event->get_Type() == TopologyEvent::TOPOL_ADD_BE)) {
        gAtomicNumAttached++;
        TopologyEvent::TopolEventData *ted = (TopologyEvent::TopolEventData *)evt_data;
        delete ted;
    }
}

void
publishBackendConnectionInfo(
    vector<NetworkTopology::Node *>& leaves,
    unsigned numBEs
) {
   FILE *fp = NULL;
   const char *connfile = "./attachBE_connections";
   assert(NULL != (fp = fopen(connfile, "w+")));

   unsigned numLeaves = unsigned(leaves.size());
   unsigned be_per_leaf = numBEs / numLeaves;
   unsigned currLeaf = 0;
   for (unsigned i = 0; (i < numBEs) && (currLeaf < numLeaves); ++i) {
       if( i && (i % be_per_leaf == 0) ) {
           // select next parent
           currLeaf++;
           if (currLeaf == numLeaves) {
               // except when there is no "next"
               currLeaf--;
           }
       }
       fprintf(stdout, "BE %d will connect to %s:%d:%d\n",
               i,
               leaves[currLeaf]->get_HostName().c_str(),
               leaves[currLeaf]->get_Port(),
               leaves[currLeaf]->get_Rank() );

       fprintf(fp, "%s %d %d %d\n",
               leaves[currLeaf]->get_HostName().c_str(),
               leaves[currLeaf]->get_Port(),
               leaves[currLeaf]->get_Rank(),
               i);
   }
   fclose(fp);
}

void
registerEventCallbacks(Network *net)
{
    bool cbrc = net->register_EventCallback(
                    Event::TOPOLOGY_EVENT,
                    TopologyEvent::TOPOL_ADD_BE,
                    beAddCallback, NULL
                );
    assert(cbrc);
}

void
waitForBackendConnections(unsigned nConnections)
{
    unsigned currCount = 0;
    do {
        sleep(1);
        currCount = gAtomicNumAttached;
        printf("*** %u connected out of %u...\n", currCount, nConnections);
    } while(currCount != nConnections);

    printf("All %u backends have attached!\n", nConnections);
}

}

int
main(int argc, char **)
{
    if (argc != 1) {
        usage();
        return EXIT_FAILURE;
    }

    cout << "*** starting tool front-end ***" << endl;

    Network *net = buildNetwork();
    assert(net);

    registerEventCallbacks(net);

    NetworkTopology *topology = NULL;
    vector<NetworkTopology::Node *> internalLeaves;
    getNetworkTopology(net, &topology, internalLeaves);

    unsigned nBeThreads = 1;
    unsigned nExpectedBEs = internalLeaves.size() * nBeThreads;

    publishBackendConnectionInfo(internalLeaves, nExpectedBEs);

    waitForBackendConnections(nExpectedBEs);

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
    assert(stream->send(PROTO_CONN, "%d", 1) != -1);
    assert(stream->flush() != -1);

    int tag = 0;
    PacketPtr packet;
    assert(stream->recv(&tag, packet) != -1);
    assert(tag == PROTO_CONN);
    int res = 0;
    packet->unpack("%d", &res);

    cout << "sum of all back-end ranks is: " << res << endl;

    cout << "q to kill back-ends..." << endl;

    char cmd = ' ';
    do {
        cmd = getchar();
        if ('s' == cmd) {
            std::cout << "stepping..." << std::endl;
            assert(stream->send(PROTO_STEP, "%d", 1) != -1);
            assert(stream->flush() != -1);
        }
    } while (cmd != 'q');

    cout << "shutting down" << endl;
    assert(stream->send(PROTO_EXIT, "") != -1);
    assert(stream->flush() != -1);

    sleep(1);
    delete net;

    return EXIT_SUCCESS;
}
