/*
 * Copyright (c) 2016      Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-api/gladius-toolbe.h"

#include "tool-be/tool-be.h"

#include <memory>

using namespace gladius::toolbe;

/**
 *
 */
Tool::Tool(
    void
) : mImpl(std::unique_ptr<Tool::ToolBE>(new Tool::ToolBE())) { ; }

/**
 *
 */
Tool::~Tool(void) = default;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Public API
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Wrapper for Tool::ToolBE::init and  Tool::ToolBE::create
 */
int
Tool::create(
    int uid,
    bool beVerbose
) {
    int rc = mImpl->init(beVerbose);
    if (GLADIUS_SUCCESS != rc) return rc;
    return mImpl->create(uid);
}

/**
 * Wrapper for Tool::ToolBE::connect
 */
int
Tool::connect(void)
{
    return mImpl->connect();
}
