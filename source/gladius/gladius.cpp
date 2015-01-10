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

////////////////////////////////////////////////////////////////////////////////
// Place for Gladius-specific environment variables.
////////////////////////////////////////////////////////////////////////////////
namespace {
static const std::vector<core::EnvironmentVar> gladiusEnvVars = {
    {GLADIUS_TOOL_FE_VERBOSE_STR, "Makes tools actions verbose when set."},
    {GLADIUS_NO_TERM_COLORS_STR, "Disables colorized terminal output when set."}
};
}

/**
 *
 */
Gladius::Gladius(
    const core::Args &args
) : mCurrentSession(core::Session::TheSession())
  , mEnv(core::Environment::TheEnvironment())
  , mUI(ui::UIFactory::getUI(mArgs, ui::UIFactory::UI_TERM))
{
    try {
        // Stash a copy of the args.
        mArgs = args;
        mUI.init(args);
        mEnv.addToRegistry(PACKAGE_NAME, gladiusEnvVars);
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
