/*
 * Copyright (c) 2014-2016 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

/**
 * Return codes.
 */

enum {
    GLADIUS_SUCCESS = 0,
    GLADIUS_ERR,
    GLADIUS_ERR_OOR,
    GLADIUS_ERR_IO,
    GLADIUS_ERR_SYS,
    GLADIUS_ERR_MRNET,
    GLADIUS_ENV_NOT_SET,
    GLADIUS_NOT_CONNECTED,
    GLADIUS_PLUGIN_NOT_FOUND
};
