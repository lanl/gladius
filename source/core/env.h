/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to environment variables that impact gladius' behavior.
 */

#ifndef GLADIUS_CORE_ENV_H_INCLUDED
#define GLADIUS_CORE_ENV_H_INCLUDED

/**
 * If this environment variable is set, then the tool front-end will be verbose
 * about its actions.
 */
#define GLADIUS_TOOL_FE_VERBOSE_STR "GLADIUS_TOOL_FE_VERBOSE"

/**
 * If this environment variable is set, then the tool will not colorize its
 * terminal output.
 */
#define GLADIUS_NO_TERM_COLORS_STR "GLADIUS_NO_TERM_COLORS"

#endif
