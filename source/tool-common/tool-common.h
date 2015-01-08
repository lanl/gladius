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
#include <cstdlib>

namespace gladius {
namespace toolcommon {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Process table class.
 */
class ProcessTable {
    unsigned int mNEntries = 0;
    MPIR_PROCDESC_EXT *mProcTab = nullptr;
public:
    /**
     *
     */
    ProcessTable(void)
        : mNEntries(0)
        , mProcTab(nullptr) { ; }
    /**
     * Allocates space for process table.
     */
    void
    allocate(size_t nEntries) {
        mNEntries = nEntries;
        // Now that we know this, allocate the process table.
        mProcTab = (MPIR_PROCDESC_EXT *)calloc(nEntries, sizeof(*mProcTab));
        if (!mProcTab) GLADIUS_THROW_OOR();
    }
    /**
     *
     */
    void
    dump(void);
    /**
     *
     */
    void
    deallocate(void) {
        if (mProcTab) {
            for (auto i = 0UL; i < mNEntries; ++i) {
                if (mProcTab[i].pd.executable_name) {
                    free(mProcTab[i].pd.executable_name);
                }
                if (mProcTab[i].pd.host_name) {
                    free(mProcTab[i].pd.host_name);
                }
            }
            free(mProcTab);
            mProcTab = nullptr;
        }
        mNEntries = 0;
    }
    /**
     * Returns the number of entries in the process table.
     */
    size_t
    nEntries(void) {
        return mNEntries;
    }
    /**
     * Returns pointer to the process table.
     */
    MPIR_PROCDESC_EXT *
    procTab(void) {
        return mProcTab;
    }
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
