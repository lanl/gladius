/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Distributed System (dsys) Interface (DSYSI).
 */

#pragma once

#include <string>

#include <unistd.h>

namespace gladius {
namespace dsysi {

/**
 *
 */
class DMI {
private:
    static const std::string sPromptString;
    //
    static const size_t sInitBufSize;
    //
    size_t mCurLineBufSize = 0;
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
    //
    char *mFromGDBLineBuf = nullptr;
    //
    pid_t mTargetPID = 0;
    //
    FILE *mTo = nullptr;
    //
    FILE *mFrom = nullptr;
    //
    size_t
    mGetGDBRespLine(void);
    //
    void
    mWaitForPrompt(void);
    //
    std::string
    mDrainToString(void);

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
    //
    void
    attach(pid_t targetPID);
    //
    int
    sendCommand(
        const std::string &rawCMD
    );
    //
    int
    recvResp(
        std::string &outputIfSuccess
    );
};


} // end dsysi namespace
} // end gladius namespace
