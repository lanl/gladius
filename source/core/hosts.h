/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <string>
#include <set>

namespace gladius {
namespace core {

class Hosts {
    //
    std::set<std::string> mHostNames;
public:
    //
    Hosts(void) { ; }

    /**
     * Returns reference to our set of host names.
     */
    const std::set<std::string> &
    hostNames(void) const {
        return mHostNames;
    }

    /**
     * Returns the number of hosts.
     */
    size_t
    nHosts(void) const {
        return mHostNames.size();
    }
};

} // end core namespace
} // end gladius namespace
