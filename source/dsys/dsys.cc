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

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <functional>

#include <cstdio>
#include <unistd.h>
#include "mpi.h"

using namespace std;

namespace {

static const char *prompt = "(dsys)";

static const int SUCCESS = 0;
static const int ERROR   = 1;
//
static const int DONE    = 0;
static const int STEP    = 1;
static const int HOSTS   = 2;

const map<char, int> cmdProtoTab = {
    {'q', DONE},
    {'s', STEP},
    {'h', HOSTS}
};


struct Proc {
    bool leader;
    bool initialized;
    int cwRank;
    int cwSize;

    Proc(void)
        : leader(false)
        , initialized(false)
        , cwRank(0)
        , cwSize(0) { ; }
};

int
hosts(Proc &p)
{
    cout << "hi from: " << p.cwRank << endl;
    return SUCCESS;
}

int
done(Proc &)
{
    return SUCCESS;
}

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

int
fini(const Proc &p)
{
    if (p.initialized) {
        int mpiRC = MPI_Finalize();
        if (MPI_SUCCESS != mpiRC) return ERROR;
    }
    return SUCCESS;
}

const map< int, function<int(Proc &p)> > protoFunTable = {
    {DONE,  done},
    {HOSTS, hosts}
};

int
interact(Proc &p)
{
    int mpiRC = MPI_Barrier(MPI_COMM_WORLD);
    if (MPI_SUCCESS != mpiRC) return ERROR;

    int cmd;
    do {
        if (p.leader) {
            cout << prompt << flush;
            string line;
            getline(cin, line);
            auto i = cmdProtoTab.find(line[0]);
            // Command not found
            if (i == cmdProtoTab.end()) {
                cerr << "unknown command: '" << line[0] << "'" << endl;
                return ERROR;
            }
            // Broadcast command to workers
            cmd = i->second;
        }
        int mpiRC = MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (MPI_SUCCESS != mpiRC) return ERROR;
        //
        auto searchr = protoFunTable.find(cmd);
        if (searchr == protoFunTable.end()) {
            cerr << "unknown protocol: '" << cmd << "'" << endl;
            return ERROR;
        }
        else {
            searchr->second(p);
        }
    } while (DONE != cmd);
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
