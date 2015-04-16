/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Debugger Machine Interface (DMI).
 */

#ifndef GLADIUS_DMI_DMI_H_INCLUDED
#define GLADIUS_DMI_DMI_H_INCLUDED

#include <string>

#include <unistd.h>

namespace gladius {
namespace dmi {

/**
 * Debugger Machine Interface.
 */
class DMI {
private:
    //
    bool mBeVerbose = false;
    //
    std::string mPathToGDB;
    //
    int mToGDB[2];
    //
    int mFromGDB[2];
    // PID of GDB process.
    pid_t mGDBPID = 0;
    // TODO will have to be dynamically sized.
    char mFromGDBLineBuf[4096];

public:
    //
    DMI(void);
    //
    ~DMI(void);
    //
    void
    init(
        bool beVerbose
    );
};


} // end dmi namespace
} // end gladius namespace

#endif
