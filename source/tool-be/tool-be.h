/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Back-End (BE) API. The interface to the back-end tool actions.
 */

#ifndef GLADIUS_TOOL_FE_TOOL_BE_H_INCLUDED
#define GLADIUS_TOOL_FE_TOOL_BE_H_INCLUDED

#include "core/core.h"
#include "core/args.h"

namespace gladius {
namespace toolbe {

class ToolBE {
private:
    core::Args mAppArgs;

public:
    ToolBE(void);
};

} // end toolbe namespace
} // end gladius namespace

#endif
