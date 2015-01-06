/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Resource Allocation Services (RAS)
 */

#ifndef GLADIUS_RAS_RAS_H_INCLUDED
#define GLADIUS_RAS_RAS_H_INCLUDED

#include "lmon_api/lmon_proctab.h"

namespace gladius {
namespace ras {

class ProcTab {
    MPIR_PROCDESC_EXT *mProcTab = nullptr;
};

}
}

#endif
