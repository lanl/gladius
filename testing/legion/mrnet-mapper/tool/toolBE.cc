/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-proto.h"
#include "tooli.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include <limits.h>
#include <unistd.h>
#include "mpi.h"

using namespace MRN;
using namespace std;

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
