/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to job the SessionKey type. 
 * Session keys are used as a unique identifier to differentiate between
 * different analysis jobs, etc.
 */

#pragma once

#include <limits.h>

namespace gladius {
namespace toolcommon {

typedef char SessionKey[32 + HOST_NAME_MAX];

} // end namespace
} // end namespace
