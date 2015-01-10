/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Front-End (FE) API. The interface to the tool actions.
 */

#ifndef GLADIUS_TOOL_FE_TOOL_FE_H_INCLUDED
#define GLADIUS_TOOL_FE_TOOL_FE_H_INCLUDED

#include "core/core.h"
#include "tool-fe/lmon.h"
#include "mrnet/mrnet-fe.h"

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace gladius {
namespace toolfe {

class ToolFE {
private:
    // Threading things ////////////////////////////////////////////////////////
    // XXX NEEDED?
    std::mutex mtFEBELock;
    //
    std::condition_variable mtBELaunchComplete;
    ////////////////////////////////////////////////////////////////////////////
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
    // Our LaunchMON instance.
    LaunchMon mLMON;
    // Our MRNet instance.
    mrnet::MRNetFE mMRNFE;
    // Target application arguments.
    core::Args mAppArgs;
    //
    void
    mLocalBody(void);
    //
    void
    mRemoteBody(void);

public:
    //
    ToolFE(void);
    //
    void
    run(const core::Args &args);
    //
    static std::string
    helpText(void);
    //
    void
    beSend();
    //
    void
    beRecv();
    //
    bool
    envSane(std::string &whatsWrong);
};

}
} // end gladius namespace

#endif
