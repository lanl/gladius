/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements LaunchMON actions shim.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lmon/lmon-be.h"

#include "core/core.h"
#include "tool-common/tool-common.h"

#include <iomanip>

using namespace gladius;
using namespace gladius::toolbe;

namespace {
// This component's name.
const std::string CNAME = "lmon-be";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::GREEN);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)
/// What to use for remote login
const std::string REMOTE_LOGIN = "/usr/bin/ssh";
}

////////////////////////////////////////////////////////////////////////////////
namespace LaunchMonFEGlobals {
// Global variable that indicates whether or not this component will be verbose.
bool beVerbose = false;
// Global variable holding latest LaunchMON state (set by statusFuncCallback).
int lmonState = 0;
}
