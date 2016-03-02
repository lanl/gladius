/*
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Distributed System (dsys) Interface (DSI).
 */

#pragma once

#include "app-launcher/app-launcher.h"

#include <string>

namespace gladius {
namespace dsi {

/**
 *
 */
class DSI {
private:
    //
    static const char sDSysName[];
    //
    static const char sPromptString[];
     //The initial size of the output buffer. 16k should be plenty.
    static constexpr size_t sInitBufSize = 1024 * 16;
    //
    applauncher::AppLauncher mAppl;
    //
    size_t mCurLineBufSize = 0;
    //
    bool mBeVerbose = false;
    //
    int mToAppl[2];
    //
    int mFromAppl[2];
    // PID of application launcher process.
    pid_t mApplPID = 0;
    //
    char *mFromDSysLineBuf = nullptr;
    //
    FILE *mTo = nullptr;
    //
    FILE *mFrom = nullptr;
    //
    size_t
    mGetRespLine(void);
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
    int
    init(
        const applauncher::AppLauncher &appl,
        bool beVerbose
    );
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
