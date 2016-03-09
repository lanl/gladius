/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "gladius.h"

#include "core/core.h"
#include "tool-fe/tool-fe.h"
#include "ui/ui-factory.h"

#include <string>

using namespace gladius;

////////////////////////////////////////////////////////////////////////////////
// Place for Gladius-specific environment variables.
////////////////////////////////////////////////////////////////////////////////
namespace {
const std::vector<core::EnvironmentVar> gladiusEnvVars = {
    {GLADIUS_ENV_TOOL_FE_VERBOSE_NAME,
     "Makes tool front-end actions verbose when set."
    },
    {GLADIUS_ENV_TOOL_BE_VERBOSE_NAME,
     "Makes tool back-end actions verbose when set."
    },
    {GLADIUS_ENV_TOOL_BE_LOG_DIR_NAME,
     "Specifies the path where tool back-end logs will be written."
    },
    {GLADIUS_ENV_NO_TERM_COLORS_NAME,
     "Disables colorized terminal output when set."
    },
    {GLADIUS_ENV_PLUGIN_PATH_NAME,
     "A colon-delimited list of paths to search for Gladius plugins."
    },
    {GLADIUS_ENV_DOMAIN_MODE_NAME,
     "Name of the session's default domain mode."
    }
};
}

/**
 *
 */
Gladius::Gladius(
    const core::Args &args
) : mCurrentSession(core::SessionFE::TheSession())
  , mEnv(core::Environment::TheEnvironment())
  , mUI(ui::UIFactory::getUI(mArgs, ui::UIFactory::UI_TERM))
{
    try {
        // Register core components.
        mCoreComponentRegistration();
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
Gladius::~Gladius(void) = default;

/**
 * For each core component, register its environment.
 */
void
Gladius::mCoreComponentRegistration(void)
{
    mEnv.addToRegistry(PACKAGE_NAME, gladiusEnvVars);
    toolfe::ToolFE::registerComponent();
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
