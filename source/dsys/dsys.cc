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

#include "mpi.h"

using namespace std;

namespace {

static const char *prompt = "(dsys)";

static const int SUCCESS = 0;
static const int ERROR   = 1;

struct Proc {
    bool initialized;
    int cwRank;
    int cwSize;

    Proc(void)
        : initialized(false)
        , cwRank(0)
        , cwSize(0) { ; }
};

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
    return SUCCESS;
}

void
fini(const Proc &p)
{
    if (p.initialized) {
        MPI_Finalize();
    }
}

int
ready(const Proc &p)
{
    int mpiRC = MPI_Barrier(MPI_COMM_WORLD);
    if (MPI_SUCCESS != mpiRC) return ERROR;

    if (0 == p.cwRank) {
        cout << prompt << flush;
    }
    return SUCCESS;
}

int
interact(Proc &p)
{
    int rc = ERROR;

    if (SUCCESS != (rc = ready(p))) return rc;

    return rc;
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
out:
    fini(proc);
    return (SUCCESS == rc) ? EXIT_SUCCESS : EXIT_FAILURE;
}
