/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to environment variable things that impact gladius' behavior.
 */

#ifndef GLADIUS_CORE_ENV_H_INCLUDED
#define GLADIUS_CORE_ENV_H_INCLUDED

#define GLADIUS_TOOL_FE_VERBOSE_STR "GLADIUS_TOOL_FE_VERBOSE"

/**
 * GLADIUS_APP_LAUNCHER:
 * Supported Values:
 * - orte
 * - srun
 * - aprun
 */
#define GLADIUS_APP_LAUNCHER_STR    "GLADIUS_APP_LAUNCHER"

#endif
