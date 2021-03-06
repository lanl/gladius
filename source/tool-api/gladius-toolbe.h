/*
 * Copyright (c) 2016      Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Outward facing back-end interface used by tools developers.
 */

#pragma once

#include "core/gladius-rc.h"

#include <memory>

namespace gladius {
namespace toolbe {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Tool {
public:
    //
    Tool(void);
    //
    ~Tool(void);
    //
    int
    create(
        int uid,
        bool beVerbose = false
    );
    //
    int
    connect(void);
private:
    // Forward declaration of ToolBE to avoid include hell on the app side.
    class ToolBE;
    //
    std::unique_ptr<ToolBE> mImpl;
};

} // namespace toolbe
} // gladius
