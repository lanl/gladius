/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_SESSION_H_INCLUDED
#define GLADIUS_CORE_SESSION_H_INCLUDED

#include "core/ret-codes.h"
#include "core/gladius-exception.h"

namespace gladius {
namespace core {
/**
 *
 */
class Session {
    static const std::string sDotName;
    std::string mSessionDir;

public:
    Session(void) { ; }

    ~Session(void) { ; }

    std::string
    sessionDir(void) { return mSessionDir; }

    void
    open(void);
};

}
}
#endif
