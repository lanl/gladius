/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Common FE/BE stuff for grok.
 */

#ifndef GLADIUS_PLUGIN_GROK_COMMON_H_INCLUDED
#define GLADIUS_PLUGIN_GROK_COMMON_H_INCLUDED

#include "tool-common/tool-common.h"

// The plugin's name.
#define PLUGIN_NAME "grok"
// The plugin's version string.
#define PLUGIN_VERSION "0.0.1"

namespace grok {
//
enum GrokProtoTags {
    // Notice where we start here. ALL plugins MUST start with this tag value.
    Shutdown = gladius::toolcommon::FirstPluginTag
};
} // end grok namesapce.

#endif
