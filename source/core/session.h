/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_SESSION_H_INCLUDED
#define GLADIUS_CORE_SESSION_H_INCLUDED

#include <string>

namespace gladius {
namespace core {
/**
 *
 */
class SessionFE {
    static const std::string sDotName;
    //
    std::string mSessionFEDir;
    //
    SessionFE(void) { ; }
    //
    void
    mOpen(void);
    //
    ~SessionFE(void) { ; }
public:
    //
    static SessionFE &
    TheSession(void);
    //
    std::string
    sessionDir(void) { return mSessionFEDir; }

    /**
     * Disable copy constructor.
     */
    SessionFE(const SessionFE &that) = delete;
    //
    SessionFE &
    operator=(const SessionFE &other);
};

}
}
#endif
