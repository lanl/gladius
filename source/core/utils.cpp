/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/utils.h"
#include "gladius-paths.h"

using namespace gladius::core;

std::string
utils::installPrefix(void)
{
    return GLADIUS_PREFIX;
}