/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#if 1

namespace gladius {
namespace toolbe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ToolContext {
};

} // namespace toolbe
} // gladius

#else

#include "tool-proto.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <limits.h>
#include <unistd.h>

#include "mrnet/MRNet.h"
#include "xplat/Monitor.h"

// TODO move
using namespace MRN;
using namespace std;

struct ThreadPersonality {
    int rank = 0;
    static constexpr int argc = 6;
    char *argv[argc];
};

class ToolContext {
public:
    enum ToolConditions {
        MAP = 0
    };
private:
    ssize_t mUID = 0;
    size_t mNToolThreads = 0;
    size_t mNToolThreadsAttached = 0;
    std::vector<std::thread> mThreads;
    std::string mConnectionFile;

public:
    std::condition_variable allThreadsAttached;
    std::mutex allThreadsAttachedMutex;

    XPlat::Monitor *inMapper = NULL;
    // TODO make thread-safe and have many.
    MRN::Network *net = NULL;

    char mHostname[HOST_NAME_MAX];
    char parentHostname[HOST_NAME_MAX],
         parentPort[16],
         parentRank[16];

    ToolContext(
        ssize_t uid,
        size_t nToolThreads,
        std::string connectionFile
    ) : mUID(uid)
      , mNToolThreads(nToolThreads)
      , mConnectionFile(connectionFile) {
        // TODO delete
        inMapper = new XPlat::Monitor();
        inMapper->RegisterCondition(MAP);
        assert(-1 != gethostname(mHostname, sizeof(mHostname)));
    }

    std::vector<std::thread> &
    getThreads(void) {
        return mThreads;
    }

    ssize_t
    getUID(void) const { return mUID; }

    std::string
    getConnectionInfo(void) const { return mConnectionFile; }

    size_t
    getNToolThreads(void) { return mNToolThreads; }

    void
    threadAttached(void) {
        std::lock_guard<std::mutex> lock(allThreadsAttachedMutex);
        ++mNToolThreadsAttached;
        allThreadsAttached.notify_one();
    }

    void
    waitForAttach(void) {
        // TODO FIXME
        const auto timeout = std::chrono::seconds(100000);
        std::unique_lock<std::mutex> lock(allThreadsAttachedMutex);
        auto status = allThreadsAttached.wait_for(
            lock,
            timeout,
            [&](){ return mNToolThreads == mNToolThreadsAttached; }
        );
        // TODO make sure all are fine and report to caller.
        if (status) {
            cout << "success!!!!!" << endl;
            fflush(stdout);
        }
        else {
            cout << "timed out!!!!!!!!!!!!" << endl;
            fflush(stdout);
        }
    }
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
toolThreadMain(
    ToolContext *tc,
    ThreadPersonality *tp
) {
    char rankStr[64];
    snprintf(rankStr, sizeof(rankStr), "%d", tp->rank);
    tp->argv[5] = rankStr;

    int32_t recv_int = 0;
    int tag;
    PacketPtr p;
    Stream *stream;

    assert(6 == tp->argc);

    tc->net = Network::CreateNetworkBE(tp->argc, tp->argv);
    assert(tc->net);

    if (tc->net->recv(&tag, p, &stream) != 1) {
        fprintf( stderr, "BE[%s]: net->recv() failure\n", rankStr);
        tag = PROTO_EXIT;
    }
    switch (tag) {
        case PROTO_CONN: {
            if (p->unpack( "%d", &recv_int) == -1) {
                fprintf( stderr, "BE[%s]: stream::unpack(%%d) failure\n", rankStr);
                return NULL;
            }
            // Ack
            if ((stream->send(PROTO_CONN, "%d", recv_int) == -1) ||
                (stream->flush() == -1 )) {
                fprintf( stderr, "BE[%s]: stream::send(%%d) failure\n", rankStr);
                assert(false);
            }
            printf("connected!\n");
            tc->threadAttached();
            break;
        }
        default:
            fprintf( stderr, "BE[%s]: Unknown Protocol %d\n", rankStr, tag);
            tag = PROTO_EXIT;
            break;
    }


    fflush(stdout);
    fflush(stderr);

#if 0
    // FE delete of the network will cause us to exit, wait for it
    net->waitfor_ShutDown();
    delete net;
#endif
    delete tp;
    return NULL;
}

int
toolAttach(
    ToolContext &tc
) {
    if (0 == tc.getUID()) cout << "=== attaching tool..." << endl;

    // TODO add as class member
    assert(!getParentInfo(tc));

    std::vector<std::thread> &threads = tc.getThreads();
    const auto nThreads = tc.getNToolThreads();
    for (size_t i = 0; i < nThreads; ++i) {
        ThreadPersonality *tp = new ThreadPersonality();
        tp->rank = (10000 * (i + 1)) + tc.getUID();
        tp->argv[0] = (char *)"./toolBE";
        tp->argv[1] = tc.parentHostname;
        tp->argv[2] = tc.parentPort;
        tp->argv[3] = tc.parentRank;
        tp->argv[4] = tc.mHostname;
        threads.push_back(std::thread(toolThreadMain, &tc, tp));
    }
    tc.waitForAttach();
    tc.inMapper->WaitOnCondition(ToolContext::ToolConditions::MAP);
    // wait for all the threads to start
    return 0;
}

int
toolDetach(ToolContext &tc)
{
    if (0 == tc.getUID()) cout << "=== detaching tool..." << endl;
    std::vector<std::thread> &threads = tc.getThreads();
    const auto nThreads = threads.size();
    for (size_t t = 0; t < nThreads; ++t) {
        threads[t].join();
    }
    return 0;
}
#endif
