/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the DMI.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "core/core.h"
#include "core/utils.h"

#include "dmi/dmi.h"

#include <cassert>
#include <iostream>

using namespace gladius;
using namespace gladius::dmi;

namespace {
// This component's name.
const std::string CNAME = "****dmi";
//
const auto COMPC = core::colors::NONE;
// CNAME's color code.
const std::string NAMEC = core::colors::color().ansiBeginColor(COMPC);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)
} // end namespace

/**
 *
 */
DMI::DMI(void) { }

/**
 *
 */
DMI::~DMI(void) { }

/**
 *
 */
void
DMI::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    //
    VCOMP_COUT("Initializing the DMI..." << std::endl);
    mMI = mi_connect_local();
    if (!mMI) GLADIUS_THROW_CALL_FAILED("mi_connect_local");
    //
    VCOMP_COUT("Done Initializing the DMI..." << std::endl);
}
