/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef EVNTS_H_INCLUDED
#define EVNTS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* In case you need this... */
#define LIBEVNTS 1

enum {
    EVNTS_SUCCESS = 0,
    EVNTS_FAILURE
};

/**
 *
 */
int
EVNTSInit(void);

/**
 *
 */
int
EVNTSFinalize(void);

#ifdef __cplusplus
}
#endif

#endif
