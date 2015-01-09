/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_SESSION_H_INCLUDED
#define GLADIUS_CORE_SESSION_H_INCLUDED

#include "core/utils.h"

namespace gladius {
namespace core {
/**
 *
 */
class Session {
    static const std::string sDotName;
    //
    std::string mSessionDir;
    //
    Session(void) { ; }
    //
    ~Session(void) { ; }
    //
    void
    mOpen(void);
public:
    static Session &
    TheSession(void);
    //
    std::string
    sessionDir(void) { return mSessionDir; }
    // Disable copy constructor.
    Session(const Session &that) = delete;
    // Just return the singleton.
    Session &
    operator=(const Session &other) {
        GLADIUS_UNUSED(other);
        return Session::TheSession();
    }
};

}
}
#endif
