/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"

#include "core/core.h"
#include "ui/ui-factory.h"

#include <string>

using namespace gladius;

/**
 *
 */
Gladius::~Gladius(void)
{
    if (mUI) delete mUI;
}

/**
 *
 */
Gladius::Gladius(const core::Args &args) {
    try {
        mArgs = args;
        mUI = ui::UIFactory::getNewUI(mArgs, ui::UIFactory::UI_TERM);
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
void
Gladius::run(void)
{
    try {
        mUI->interact();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
