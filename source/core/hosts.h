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

/**
 * TODO add real host resolution things?
 */
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

    /**
     * Returns the canonical hostname form from an arbitrary hostname string.
     */
    static std::string
    canonicalForm(const std::string &hostname) {
        auto chn = hostname;
        auto dotPos = chn.find_first_of(".");
        if (std::string::npos != dotPos) {
            chn = chn.substr(0, dotPos);
        }
        return chn;
    }
};

} // end core namespace
} // end gladius namespace
