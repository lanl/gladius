/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "ui.h"

using namespace gladius::ui;

/**
 *
 */
UI::~UI(void) { ; }

/**
 *
 */
UI::UI(void) { ; }

/**
 *
 */
UI::UI(const core::Args &args)
{
    try {
        mArgs = args;
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
