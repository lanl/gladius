/*
 * Copyright (c) 2014-2016 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <string>

namespace gladius {
namespace core {
/**
 *
 */
class SessionFE {
    // The name of our user-specific session directory where we stash some of
    // our stuff.
    static const std::string sDotName;
    // The installation prefix of the tool front-end.
    std::string mExecPrefix;
    //
    std::string mSessionFEDir;
    //
    SessionFE(void) { ; }
    //
    void
    mOpen(void);
    //
    void
    init(void);
    //
    void
    mSetExecPrefix(void);
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

    /**
     *
     */
    std::string
    execPrefix(void) {
        return mExecPrefix;
    }
};

}
}
