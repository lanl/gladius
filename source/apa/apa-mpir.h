/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * MPIR for APA
 *
 * @see: http://www.mpi-forum.org/docs/mpir-specification-10-11-2010.pdf
 */

#ifndef GLADIUS_APA_MPIR_H_INCLUDED
#define GLADIUS_APA_MPIR_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// Defined by the debugger interface
typedef struct MPIR_PROCDESC {
    const char *host_name;
    const char *executable_name;
    int pid;
} MPIR_PROCDESC;

// Set by the debugger when it attaches/launches application
extern volatile int MPIR_being_debugged;

// The number of entries in MPIR_proctable.
extern int MPIR_proctable_size;

// An array of processes
extern struct MPIR_PROCDESC *MPIR_proctable;

#ifdef __cplusplus
}
#endif

#endif
