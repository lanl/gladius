/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to connection information types.
 */

#pragma once

#include <limits.h>

namespace gladius {
namespace toolcommon {

extern "C" {
/**
 *
 */
typedef struct ToolLeafInfoT {
    // TODO Remove
    char hostName[HOST_NAME_MAX];
    char parentHostName[HOST_NAME_MAX];
    int rank;
    int parentPort;
    int parentRank;
} ToolLeafInfoT;

/**
 *
 */
typedef struct ToolLeafInfoArrayT {
    int size;
    ToolLeafInfoT *leaves;
} ToolLeafInfoArrayT;

} // extern "C"

} // end namespace
} // end namespace
