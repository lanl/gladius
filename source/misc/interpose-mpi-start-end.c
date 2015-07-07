/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * To Build
 * mpicc -o interpose-mpi-start-end.so -shared -fPIC interpose-mpi-start-end.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

typedef struct csy_state_t {
    int numpe;
    int rank;
    char hostbuf[MPI_MAX_PROCESSOR_NAME];
} csy_state_t;

static csy_state_t my_context;

/* ////////////////////////////////////////////////////////////////////////// */
int
MPI_Init(int *argc, char ***argv)
{
    int rc = MPI_ERR_OTHER, len = 0;

    if (MPI_SUCCESS != (rc = PMPI_Init(argc, argv))) {
        perror("PMPI_Init failed");
    }
    if (MPI_SUCCESS != (rc = MPI_Comm_size(MPI_COMM_WORLD,
                                           &my_context.numpe))) {
        perror("MPI_Comm_size failed");
    }
    if (MPI_SUCCESS != (rc = MPI_Comm_rank(MPI_COMM_WORLD,
                                           &my_context.rank))) {
        perror("MPI_Comm_rank failed");
    }
    if (MPI_SUCCESS != (rc = MPI_Get_processor_name(my_context.hostbuf,
                                                    &len))) {
        perror("MPI_Get_processor_name failed");
    }
    if (0 == my_context.rank) {
        fprintf(stdout, "\n@@@ %s @@@\n", __func__);
    }
    return MPI_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
MPI_Abort(MPI_Comm comm, int errorcode)
{
    fprintf(stdout, "\n\n @@@ rank %d on host %s called MPI_Abort @@@\n\n",
            my_context.rank, my_context.hostbuf);
    fflush(stdout);

    return PMPI_Abort(comm, errorcode);
}

/* ////////////////////////////////////////////////////////////////////////// */
int
MPI_Finalize(void)
{
    if (0 == my_context.rank) {
        fprintf(stdout, "\n@@@ %s @@@\n", __func__);
    }
    return PMPI_Finalize();
}
