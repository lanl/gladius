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
#include "lmon/lmon-fe.h"
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
    //
    std::mutex mtLashUpLock;
    //
    std::condition_variable mtLashUpComplete;
    ////////////////////////////////////////////////////////////////////////////
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
    // Our LaunchMON front-end instance.
    LaunchMonFE mLMONFE;
    // Our MRNet instance.
    mrnet::MRNetFE mMRNFE;
    // Target application arguments.
    core::Args mAppArgs;
    //
    void
    mGetStateFromEnvs(void);
    //
    void
    mInitializeToolInfrastructure(void);
    //
    void
    mStartToolLashUpThread(void);
    //
    void
    mInitiateToolLashUp(void);

public:
    //
    ToolFE(void);
    //
    void
    mainLoop(const core::Args &args);
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
