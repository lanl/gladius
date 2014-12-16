/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
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
Gladius::Gladius(const core::Args &args) {
    try {
        mArgs = args;
        // TODO add parsing and real UI instantiation.
        mUI = ui::UIFactory::getNewUI(mArgs, ui::UIFactory::UI_TERM);
        if (!mUI) GLADIUS_THROW_CALL_FAILED("getNewUI");
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

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
