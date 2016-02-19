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
#include <fstream>

#include <limits.h>
#include <unistd.h>
#include "mrnet/MRNet.h"
#include "xplat/Thread.h"
#include "mpi.h"

using namespace MRN;
using namespace std;

struct ThreadPersonality {
    int rank = 0;
    static constexpr int argc = 6;
    char *argv[argc];
};

class ToolContext {
private:
    ssize_t mUID = 0;
    size_t mNToolThreads = 0;
    std::vector<XPlat::Thread::Id> mTIDs;
    std::string mConnectionFile;

public:
    char mHostname[HOST_NAME_MAX];
    char parentHostname[HOST_NAME_MAX],
         parentPort[16],
         parentRank[16],
         toolRankString[16];
    static constexpr int sArgc = 6;
    char *argv[sArgc];

    ToolContext(
        ssize_t uid,
        size_t nToolThreads,
        std::string connectionFile
    ) : mUID(uid)
      , mNToolThreads(nToolThreads)
      , mConnectionFile(connectionFile) {
        assert(-1 != gethostname(mHostname, sizeof(mHostname)));
    }

    std::vector<XPlat::Thread::Id> &
    getThreadIDs(void) {
        return mTIDs;
    }

    ssize_t
    getUID(void) const { return mUID; }

    std::string
    getConnectionInfo(void) const { return mConnectionFile; }

    size_t
    getNToolThreads(void) { return mNToolThreads; }
};

int
getParentInfo(ToolContext &tc)
{
    char lineBuf[256];
    const char *file = tc.getConnectionInfo().c_str();
    ifstream ifs(file);
    assert(ifs.is_open());
    while (ifs.good()) {
        ifs.getline(lineBuf, sizeof(lineBuf));

        char pname[64];
        int tpport, tprank, trank;
        int matches = sscanf(
                          lineBuf, "%s %d %d %d",
                          pname, &tpport, &tprank, &trank
                      );
        if (4 != matches) {
            fprintf(stderr, "Error while scanning %s\n", file);
            ifs.close();
            return 1;
        }
        if (trank == tc.getUID()) {
            sprintf(tc.parentHostname, "%s", pname);
            sprintf(tc.parentPort, "%d", tpport);
            sprintf(tc.parentRank, "%d", tprank);
            ifs.close();
            return 0;
        }
    }
    ifs.close();
    // my rank not found :(
    return 1;
}

void *
toolThreadMain(void *arg)
{
    ThreadPersonality *tp = (ThreadPersonality *)arg;

    char rankStr[64];
    snprintf(rankStr, sizeof(rankStr), "%d", tp->rank);
    tp->argv[5] = rankStr;

    int32_t recv_int = 0;
    int tag;
    PacketPtr p;
    Stream *stream;
    Network *net = NULL;

    assert(6 == tp->argc);

    net = Network::CreateNetworkBE(tp->argc, tp->argv);
    assert(net);

    do {
        if (net->recv(&tag, p, &stream) != 1) {
            fprintf( stderr, "BE[%s]: net->recv() failure\n", rankStr);
            tag = PROTO_EXIT;
        }

        switch( tag ) {
            case PROTO_PING: {
                if (p->unpack( "%d", &recv_int) == -1 ) {
                    fprintf( stderr, "BE[%s]: stream::unpack(%%d) failure\n", rankStr);
                    return NULL;
                }

                fprintf( stdout, "BE[%s]: received int = %d\n", rankStr, recv_int);

                if( (stream->send(PROTO_PING, "%d", recv_int) == -1) ||
                        (stream->flush() == -1 ) ) {
                    fprintf( stderr, "BE[%s]: stream::send(%%d) failure\n", rankStr);
                    return NULL;
                }
                break;
             }

            case PROTO_EXIT:
                break;

            default:
                fprintf( stderr, "BE[%s]: Unknown Protocol %d\n", rankStr, tag);
                tag = PROTO_EXIT;
                break;
        }

    fflush(stdout);
    fflush(stderr);

    } while ( tag != PROTO_EXIT );

    // FE delete of the network will cause us to exit, wait for it
    net->waitfor_ShutDown();
    delete net;

    // TODO
    //delete arg;
    return NULL;
}

int
toolAttach(
    ToolContext &tc
) {
    if (0 == tc.getUID()) cout << "=== attaching tool..." << endl;

    // TODO add as class member
    assert(!getParentInfo(tc));

    std::vector<XPlat::Thread::Id> &tids = tc.getThreadIDs();
    for (size_t i = 0; i < tc.getNToolThreads(); ++i) {
        ThreadPersonality *tp = new ThreadPersonality();
        tp->rank = (10000 * (i + 1)) + tc.getUID();
        tp->argv[0] = (char *)"./toolBE";
        tp->argv[1] = tc.parentHostname;
        tp->argv[2] = tc.parentPort;
        tp->argv[3] = tc.parentRank;
        tp->argv[4] = tc.mHostname;
        XPlat::Thread::Id tid;
        XPlat::Thread::Create(toolThreadMain, (void *)tp, &tid);
        tids.push_back(tid);
    }
    return 0;
}

int
toolDetach(ToolContext &tc)
{
    if (0 == tc.getUID()) cout << "=== detaching tool..." << endl;
    std::vector<XPlat::Thread::Id> &tids = tc.getThreadIDs();
    size_t nThreads = tids.size();
    for (size_t t = 0; t < nThreads; ++t) {
        XPlat::Thread::Id tid = tids[t];
        void *retval = NULL;
        XPlat::Thread::Join(tid, &retval);
    }
    return 0;
}

int
doWork(int rank)
{
    if (0 == rank) cout << "=== application is working now..." << endl;
    sleep(5);
    return 0;
}

int
main(int argc, char **argv)
{
    assert(MPI_SUCCESS == MPI_Init(&argc, &argv));

    int cwRank = 0, cwSize = 0;

    assert(MPI_SUCCESS == MPI_Comm_size(MPI_COMM_WORLD, &cwSize));
    assert(MPI_SUCCESS == MPI_Comm_rank(MPI_COMM_WORLD, &cwRank));

    if (0 == cwRank) {
        printf("=== %d application processes started...\n", cwSize);
        printf("=== connecting to tool...\n");
        fflush(stdout);
    }

    ToolContext tc(cwRank, 1, "./attachBE_connections");
    assert(!toolAttach(tc));
    assert(!doWork(cwRank));
    assert(!toolDetach(tc));

    assert(MPI_SUCCESS == MPI_Finalize());

    return EXIT_SUCCESS;
}
