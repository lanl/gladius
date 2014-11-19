/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Front-End (FE) API. The interface to the tool actions.
 */

#ifndef GLADIUS_TOOL_FE_TOOL_FE_H_INCLUDED
#define GLADIUS_TOOL_FE_TOOL_FE_H_INCLUDED

#include "core/core.h"

#include <string>

namespace gladius {
namespace toolfe {

class ToolFE {
private:

public:
    std::string
    helpText(void) const;

    void
    launch();

    void
    beSend();

    void
    beRecv();
};

}
} // end gladius namespace

#endif
