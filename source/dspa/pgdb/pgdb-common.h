/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Common stuff (FE/BE) for the Parallel Step (pgdb) plugin.
 */

#ifndef GLADIUS_PLUGIN_PGDB_COMMON_H_INCLUDED
#define GLADIUS_PLUGIN_PGDB_COMMON_H_INCLUDED

#include "tool-common/tool-common.h"

// The plugin's name.
#define PLUGIN_NAME "pgdb"
// The plugin's version string.
#define PLUGIN_VERSION "0.0.1"

namespace pgdb {
//
enum PGDBProtoTags {
    // Notice where we start here. ALL plugins MUST start with this tag value.
    SetBreakPoint = gladius::toolcommon::FirstPluginTag,
    ExecCommand,
    Run,
    Step,
    Exit
};
} // end pgdb namesapce.

#endif
