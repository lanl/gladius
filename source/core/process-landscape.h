/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Gives the 'process landscape' of a parallel and distributed application
 * wherein the used hosts and number of targets for each host is maintained.
 */

#pragma once

#include "core/core.h"
#include "core/macros.h"

#include <iostream>
#include <string>
#include <map>
#include <set>

namespace gladius {
namespace core {

class ProcessLandscape {
    // Mapping between hostnames and the number of processes they'll be hosting.
    // Guaranteed to be unique by host name.
    std::map<std::string, int> mLandscape;
public:
    //
    ProcessLandscape(void) { ; }

    /**
     * Returns a set of all the host names in the current landscape.
     */
    const std::set<std::string>
    hostNames(void) const {
        std::set<std::string> res;
        for (const auto mi : mLandscape) {
            res.insert(mi.first);
        }
        return res;
    }

    /**
     * Returns the number of hosts.
     */
    size_t
    nHosts(void) const {
        return mLandscape.size();
    }

    /**
     * Returns the total number of tasks across all the hosts.
     */
    size_t
    nProcesses(void) const {
        size_t res = 0;
        for (const auto &mi : mLandscape) {
            res += mi.second;
        }
        return res;
    }

    /**
     *
     */
    int
    insert(
        const std::string &hn,
        int nProc
    ) {
        using namespace std;
        // Already in the table
        if (0 != mLandscape.count(hn)) {
            GLADIUS_CERR_WARN << hn << " already in table..." << endl;
            return GLADIUS_ERR;
        }
        mLandscape[hn] = nProc;
        //
        return GLADIUS_SUCCESS;
    }
};

} // end core namespace
} // end gladius namespace
