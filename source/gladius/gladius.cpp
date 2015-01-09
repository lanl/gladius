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
Gladius::Gladius(
    const core::Args &args
) : mCurrentSession(core::Session::TheSession())
  , mUI(ui::UIFactory::getUI(mArgs, ui::UIFactory::UI_TERM))
{
    try {
        // Stash a copy of the args.
        mArgs = args;
        mUI.init(args);
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
Gladius::~Gladius(void)
{
}

/**
 *
 */
void
Gladius::run(void)
{
    try {
        mUI.interact();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
bool
Gladius::shutdown(void)
{
    try {
        // Ask active instance.
        return mUI.quit();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}
