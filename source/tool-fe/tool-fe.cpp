/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-fe.h"

#include <string>

using namespace gladius::toolfe;

/**
 * Returns tool front-end help string.
 */
std::string
ToolFE::helpText(void) const
{
    return std::string {"toolFE help string goes here..."};
}
