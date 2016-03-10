/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 */

/**
 * Home to common infrastructure used by tool front-ends and back-ends.
 */

#pragma once

#include "tool-common/faux-mpir.h"
#include "tool-common/gladius-tli.h"

#include "core/core.h"
#include "core/utils.h"

#include <cstring>
#include <cstdint>
#include <string>
#include <set>
#include <cstdlib>
#include <ostream>
#include <iostream>

#include <limits.h>

namespace gladius {
namespace toolcommon {

////////////////////////////////////////////////////////////////////////////////
// Types and constants.
////////////////////////////////////////////////////////////////////////////////

// Timeout type.
typedef int64_t timeout_t;
// Constant that means "no timeout."
const timeout_t unlimitedTimeout = -1;
// Retry type
typedef int64_t retry_t;
// Constant that means "unlimited retries."
const retry_t unlimitedRetries = -1;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class utils {
    /**
     *
     */
    utils(void) = default;
    /**
     *
     */
    ~utils(void) = default;
public:
    /**
     *
     */
    static std::string
    genNotInPathErrString(
        const std::string &whatsNotInPath
    ) {
        auto msg = "It appears as if '" + whatsNotInPath
                   + "', is not in your $PATH.\n"
                   + "Please update your $PATH to include its location.";
        return msg;
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class MRNetCoreTags {
    //
    int
    getFirstAppTag(void) const;
public:
    /**
     *
     */
    constexpr
    MRNetCoreTags(
        void
    ) : InitHandshake      (getFirstAppTag() + 0)
      , PluginNameInfo     (getFirstAppTag() + 1)
      , BackEndPluginsReady(getFirstAppTag() + 2)
      , Shutdown           (getFirstAppTag() + 3)
      , FirstPluginTag     (getFirstAppTag() + 4)
    { ; }

    /**
     *
     */
    ~MRNetCoreTags(void) = default;
    // Tag for initial lash-up handshake.
    const int InitHandshake;
    // Tag for sending plugin info.
    const int PluginNameInfo;
    // Back-end plugins ready.
    const int BackEndPluginsReady;
    // Shutdown tag.
    const int Shutdown;
    // First plugin tag.
    const int FirstPluginTag;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * A struct of items and meta-data that can be transfered over-the-wire. No
 * complex data types, please! T should be things like: int, double, etc.
 */
template <class T>
class TxList {
    //
    T *
    dupElems(size_t ne, T *es)
    {
        if (0 == ne || !es) return nullptr;
        auto res = (T *)calloc(ne, sizeof(T));
        if (!res) GLADIUS_THROW_OOR();
        (void)memmove(res, es, sizeof(T) * ne);
        return res;
    }

public:
    // The length of the elems array.
    size_t nElems = 0;
    // Points to the element array.
    T *elems = nullptr;

    /**
     *
     */
    TxList(void)
        : nElems(0)
        , elems(nullptr) { ; }

    /**
     *
     */
    ~TxList(void) {
        if (elems) {
            free(elems);
            elems = nullptr;
        }
        nElems = 0;
    }

    /**
     * Copy constructor.
     */
    TxList(const TxList &other)
    {
        nElems = other.nElems;
        elems = dupElems(nElems, other.elems);
    }

    /**
     *
     */
    TxList &
    operator=(const TxList &other)
    {
        nElems = other.nElems;
        elems = dupElems(nElems, other.elems);
        return *this;
    }
};

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
            for (decltype(mNEntries) i = 0; i < mNEntries; ++i) {
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
        for (decltype(nEntries) i = 0; i < nEntries; ++i) {
            res[i].cnodeid = from[i].cnodeid;
            res[i].mpirank = from[i].mpirank;
            res[i].pd.pid = from[i].pd.pid;
            if (from[i].pd.host_name) {
                res[i].pd.host_name = strdup(from[i].pd.host_name);
                if (!res[i].pd.host_name) GLADIUS_THROW_OOR();
            }
            if (from[i].pd.executable_name) {
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
    ProcessTable(
        const ProcessTable &other
    ) {
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
    operator=(
        const ProcessTable &other
    ) {
        mNEntries = other.mNEntries;
        mProcTab = dupMPIRProcDescExt(mNEntries, other.mProcTab);
        return *this;
    }
    //
    void
    dumpTo(
        std::ostream &os,
        const std::string &outPrefix = "",
        core::colors::Color color = core::colors::Color::NONE
    ) const;

    /**
     * Returns the number of entries in the process table.
     */
    size_t
    nEntries(void) const {
        return mNEntries;
    };

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
        for (decltype(mNEntries) te = 0; te < mNEntries; ++te) {
            nameSet.insert(mProcTab[te].pd.host_name);
        }
        return nameSet;
    }
};

} // end namespace
} // end namespace
