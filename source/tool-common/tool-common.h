/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Home to common infrastructure used by tool front-ends and back-ends.
 */

#ifndef GLADIUS_TOOL_COMMON_TOOL_COMMON_H_INCLUDED
#define GLADIUS_TOOL_COMMON_TOOL_COMMON_H_INCLUDED

#include "core/core.h"
#include "core/utils.h"

#include "lmon_api/lmon_proctab.h"

#include <string>
#include <set>
#include <cstdlib>
#include <ostream>
#include <iostream>

namespace gladius {
namespace toolcommon {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Process table class.
 */
class ProcessTable {
    //
    unsigned int mNEntries = 0;
    //
    MPIR_PROCDESC_EXT *mProcTab = nullptr;

    /**
     * Allocates space for process table.
     */
    void
    mAllocate(size_t nEntries) {
        mNEntries = nEntries;
        // Now that we know this, allocate the process table.
        mProcTab = (MPIR_PROCDESC_EXT *)calloc(nEntries, sizeof(*mProcTab));
        if (!mProcTab) GLADIUS_THROW_OOR();
    }

    /**
     *
     */
    void
    mDeallocate(void) {
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
     *
     */
    static MPIR_PROCDESC_EXT *
    dupMPIRProcDescExt(
        unsigned int nEntries,
        MPIR_PROCDESC_EXT *from
    ) {
        if (!from || 0 == nEntries) return nullptr;
        MPIR_PROCDESC_EXT *res = nullptr;
        res = (MPIR_PROCDESC_EXT *)calloc(nEntries, sizeof(*res));
        if (!res) GLADIUS_THROW_OOR();
        for (auto i = 0UL; i < nEntries; ++i) {
            res[i].cnodeid = from[i].cnodeid;
            res[i].mpirank = from[i].mpirank;
            res[i].pd.pid = from[i].pd.pid;
            if (res[i].pd.host_name) {
                res[i].pd.host_name = strdup(from[i].pd.host_name);
                if (!res[i].pd.host_name) GLADIUS_THROW_OOR();
            }
            if (res[i].pd.executable_name) {
                res[i].pd.executable_name = strdup(from[i].pd.executable_name);
                if (!res[i].pd.executable_name) GLADIUS_THROW_OOR();
            }
        }
        return res;
    }


public:
    /**
     * Constructors.
     */
    ProcessTable(size_t nEntries) {
        mAllocate(nEntries);
    }
    //
    ProcessTable(void)
        : mNEntries(0)
        , mProcTab(nullptr) { ; }

    /**
     * Copy constructor.
     */
    ProcessTable(const ProcessTable &other)
    {
        mNEntries = other.mNEntries;
        mProcTab = dupMPIRProcDescExt(mNEntries, other.mProcTab);
    }

    /**
     * Destructor.
     */
    ~ProcessTable(void) {
        mDeallocate();
    }

    /**
     *
     */
    ProcessTable &
    operator=(const ProcessTable &other)
    {
        mNEntries = other.mNEntries;
        mProcTab = dupMPIRProcDescExt(mNEntries, other.mProcTab);
        return *this;
    }

    /**
     *
     */
    void
    dumpTo(std::ostream &os);

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

    /**
     * Returns a set of node (host) names.
     */
    std::set<std::string>
    hostNamesInTable(void) const {
        std::set<std::string> nameSet;
        for (auto te = 0UL; te < mNEntries; ++te) {
            nameSet.insert(mProcTab[te].pd.host_name);
        }
        return nameSet;
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * TODO add real host resolution things?
 */
class Hosts {
    //
    std::set<std::string> mHostNames;
public:
    //
    Hosts(void) { ; }
    //
    Hosts(const ProcessTable &pTab) {
        mHostNames = pTab.hostNamesInTable();
    }

    /**
     * Returns reference to our set of host names.
     */
    std::set<std::string> &
    hostNames(void) {
        return mHostNames;
    }

    /**
     * Returns the number of hosts.
     */
    size_t
    nHosts(void) {
        return mHostNames.size();
    }
};

}
}

#endif
