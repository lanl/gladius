/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Distributed System (dsys) Interface (DSI).
 */

#pragma once

#include <string>

#include <unistd.h>

namespace gladius {
namespace dsi {

/**
 *
 */
class DSI {
private:
    static const std::string sPromptString;
    /**
     * The initial size of the output buffer. 16k should be plenty.
     */
    static constexpr size_t sInitBufSize = 1024 * 16;
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
    DSI(void);
    //
    ~DSI(void);
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


} // end dsi namespace
} // end gladius namespace
