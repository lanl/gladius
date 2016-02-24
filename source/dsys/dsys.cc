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

/**
 *
 */
int
main(
    int argc,
    char **argv
) {
    int mpiRC = MPI_Init(&argc, &argv);
    //
    int cwSize = 0;
    mpiRC = MPI_Comm_size(MPI_COMM_WORLD, &cwSize);
    //
    int cwRank = 0;
    mpiRC = MPI_Comm_rank(MPI_COMM_WORLD, &cwRank);

    if (0 == cwRank) {
        std::cout << "hi from: " << cwRank << std::endl;
    }
    //
    mpiRC = MPI_Finalize();

    return EXIT_SUCCESS;
}
