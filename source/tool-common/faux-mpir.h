/*
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 */

/**
 * Home to convenient structures in MPIR land that are mimicked here.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *host_name;       /* Something we can pass to inet_addr */
  char *executable_name; /* The name of the image */
  int   pid;             /* The pid of the process */
} MPIR_PROCDESC;

typedef struct {
  MPIR_PROCDESC pd;
  int mpirank;
  int cnodeid;
} MPIR_PROCDESC_EXT;

#ifdef __cplusplus
}
#endif
