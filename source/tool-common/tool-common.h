/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to common infrastructure by tool front-ends and back-ends.
 */

#ifndef GLADIUS_TOOL_COMMON_TOOL_COMMON_H_INCLUDED
#define GLADIUS_TOOL_COMMON_TOOL_COMMON_H_INCLUDED

#include "core/core.h"
#include "lmon_api/lmon_proctab.h"

#include <string>
#include <set>

namespace gladius {
namespace toolcommon {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
class ProcessTable {
    MPIR_PROCDESC_EXT *mProcTab = nullptr;
public:
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
class Hosts {
    std::set<std::string> mHostNames;
};

}
}

#endif
