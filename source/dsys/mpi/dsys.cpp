/**
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * MPI code that is launched over the target resources to:
 * 1. Gather system statistics relevant to tool setup.
 * 2. Deposit tool-related startup info across those resources.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "core/utils.h"
#include "tool-common/leaf-info.h"

#include <functional>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <map>
#include <vector>

#include <limits.h>
#include <unistd.h>

#include "mpi.h"

using namespace std;

namespace {

static const char *prompt   = "(dsys) ";
static const char *compName = "[***dsys]";

static const int SUCCESS = 0;
static const int ERROR   = 1;
//
static const int DONE    = 0;
static const int HOSTS   = 1;
static const int PUBCONN = 2;

/**
 *
 */
struct Proc {
    char hostname[HOST_NAME_MAX];
    bool leader;
    bool initialized;
    int cwRank;
    int cwSize;
    // Map between a hostname and the number of targets on it. There shall be no
    // duplicate hostnames in this table.
    map<string, int> hostTargetNumTab;

    Proc(void)
        : leader(false)
        , initialized(false)
        , cwRank(0)
        , cwSize(0) {
        int rc = gethostname(hostname, sizeof(hostname));
        if (0 != rc) exit(EXIT_FAILURE);
    }
};

/**
 *
 */
int
echoHosts(Proc &p)
{
    // I don't have it, so return.
    if (!p.leader) return SUCCESS;
    // I do, so here you go
    // Form: [hostname] [number of expected tasks]
    for (const auto ti : p.hostTargetNumTab) {
        std::cout << ti.first << " " << ti.second << std::endl;
    }
    std::cout << std::flush;
    return SUCCESS;
}

/**
 *
 */
int
hosts(Proc &p)
{
    static bool done = false;
    if (done) return echoHosts(p);
    //
    char *hostNames = NULL;
    //
    if (p.leader) {
        hostNames = new char[p.cwSize * HOST_NAME_MAX * sizeof(char)];
        if (!hostNames) return ERROR;
    }
    //
    int mpiRC = MPI_Gather(
                    p.hostname,
                    HOST_NAME_MAX,
                    MPI_CHAR,
                    hostNames,
                    HOST_NAME_MAX,
                    MPI_CHAR,
                    0,
                    MPI_COMM_WORLD
                );
    if (MPI_SUCCESS != mpiRC) return ERROR;
    // Populate the hostname/number of targets table
    if (p.leader) {
        for (int r = 0; r < p.cwSize; ++r) {
            // Grab the host name from the flat buffer.
            string hn(&(hostNames[r * HOST_NAME_MAX]));
            auto &tab = p.hostTargetNumTab;
            auto searchr = tab.find(hn);
            // Not found
            if (searchr == tab.end()) {
                tab.insert(make_pair(hn, 1));
            }
            // Found, increment by one
            else {
                searchr->second += 1;
            }
        }
    }
    if (hostNames) delete[] hostNames;
    done = true;
    return echoHosts(p);
}

/**
 * Publishes tool connection info in parallel across compute resources.
 * Protocol:
 * 1. Read number of expected targets, n
 * 2. Read and decode n base64-encoded infos.
 * 3. Push info to nodes.
 */
int
pubConn(Proc &p)
{
    using namespace gladius;
    using namespace std;
    //
    if (p.leader) {
        string line;
        std::getline(cin, line);
        int nTargets = std::stol(line, 0, 10);
        int nGot = 0;
        vector<toolcommon::ToolLeafInfoT> leafInfos;
        for ( ; nGot < nTargets; ++nGot) {
            std::getline(cin, line);
            if (line.empty()) break;
            // Decode the infos
            const string res = core::utils::base64Decode(line);
            toolcommon::ToolLeafInfoT li;
            memcpy(&li, res.data(), sizeof(li));
#if 1 // DEBUG
            cout << "ToolLeafInfoT " << nGot << endl
                 << "- Parent Host Name: " << li.parentHostName << endl
                 << "- Parent Rank     : " << li.parentRank     << endl
                 << "- Parent Port     : " << li.parentPort     << endl;
#endif
        }
        if (nGot != nTargets) {
            cerr << compName
                 << " Terminating due to unexpected number of infos..."
                 << endl;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    return SUCCESS;
}

/**
 *
 */
int
done(Proc &)
{
    return SUCCESS;
}

/**
 *
 */
int
init(
    int argc,
    char **argv,
    Proc &p
) {
    int mpiRC = MPI_Init(&argc, &argv);
    if (MPI_SUCCESS != mpiRC) return ERROR;
    else p.initialized = true;
    //
    mpiRC = MPI_Comm_size(MPI_COMM_WORLD, &p.cwSize);
    if (MPI_SUCCESS != mpiRC) return ERROR;
    //
    mpiRC = MPI_Comm_rank(MPI_COMM_WORLD, &p.cwRank);
    if (MPI_SUCCESS != mpiRC) return ERROR;
    //
    p.leader = (0 == p.cwRank);
    //
    return SUCCESS;
}

/**
 *
 */
int
fini(const Proc &p)
{
    if (p.initialized) {
        int mpiRC = MPI_Finalize();
        if (MPI_SUCCESS != mpiRC) return ERROR;
    }
    return SUCCESS;
}

/**
 * User command/protocol table.
 */
const map<char, int> cmdProtoTab = {
    {'q', DONE},
    {'h', HOSTS},
    {'c', PUBCONN}
};

/**
 * Protocol/function table.
 */
const map< int, function<int(Proc &p)> > protoFunTable = {
    {DONE,    done},
    {HOSTS,   hosts},
    {PUBCONN, pubConn}
};

/**
 * Interaction REPL.
 */
int
interact(Proc &p)
{
    int mpiRC = MPI_Barrier(MPI_COMM_WORLD);
    if (MPI_SUCCESS != mpiRC) return ERROR;

    int cmd;
    do {
        if (p.leader) {
            cout << prompt << endl << flush;
            string line;
            do {
                std::getline(cin, line);
            } while (line.empty());
            auto searchr = cmdProtoTab.find(line[0]);
            // Command not found
            if (searchr == cmdProtoTab.end()) {
                cerr << compName
                     << " Terminating due to unexpected command: '"
                     << line[0] << "'" << endl;
                cmd = DONE;
            }
            else {
                // Broadcast command to workers
                cmd = searchr->second;
            }
        }
        int mpiRC = MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (MPI_SUCCESS != mpiRC) return ERROR;
        //
        auto searchr = protoFunTable.find(cmd);
        if (searchr == protoFunTable.end()) {
            // Callback function for given command not found.
            cerr << "cannot satisfy protocol request!" << endl;
            return ERROR;
        }
        else {
            searchr->second(p);
        }
    } while (DONE != cmd);
    //
    return SUCCESS;
}

} // namespace

/**
 *
 */
int
main(
    int argc,
    char **argv
) {
    int rc = SUCCESS;
    //
    Proc proc;
    //
    if (SUCCESS != (rc = init(argc, argv, proc))) {
        goto out;
    }
    //
    if (SUCCESS != (rc = interact(proc))) {
        goto out;
    }
    // Don't finalize in out: (may hang). Just exit on error so that the MPI
    // runtime can just bail when a process exits with an error code.
    if (SUCCESS != (rc = fini(proc))) {
        goto out;
    }
out:
    int exitCode = (SUCCESS == rc) ? EXIT_SUCCESS : EXIT_FAILURE;
    exit(exitCode);
    // Never reached.
    return -127;
}
