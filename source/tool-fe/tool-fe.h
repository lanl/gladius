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
#include "tool-common/tool-common.h"
#include "lmon/lmon-fe.h"
#include "mrnet/mrnet-fe.h"
#include "dspa/core/dsp-manager.h"
#include "dspa/core/gladius-dspi.h"

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
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
    //
    std::atomic<int> maStatus;
    ////////////////////////////////////////////////////////////////////////////
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
    // stdin copy
    int mStdInCopy = 0;
    // Our LaunchMON front-end instance.
    lmonfe::LaunchMonFE mLMONFE;
    // Our MRNet instance.
    mrnetfe::MRNetFE mMRNFE;
    // The plugin manager.
    dspa::DSPManager mDSPManager;
    // The plugin instance pointer.
    dspi::DomainSpecificPlugin *mFEPlugin = nullptr;
    // Target application arguments.
    core::Args mAppArgs;
    // Connection timeout (in seconds).
    toolcommon::timeout_t mConnectionTimeoutInSec;
    // Max number of connection retries.
    toolcommon::retry_t mMaxRetries;
    // The path to a valid plugin pack.
    std::string
    mPathToPluginPack;
    // The plugin pack for our current session.
    dspa::DSPluginPack mPluginPack;
    //
    void
    mGetStateFromEnvs(void);
    //
    void
    mPreToolInitActons(void);
    //
    void
    mForwardEnvsToBEsIfSetOnFE(void);
    //
    void
    mInitializeToolInfrastructure(void);
    //
    void
    mPostToolInitActons(void);
    //
    void
    mStartToolLashUpThread(void);
    //
    void
    mInitiateToolLashUp(void);
    //
    void
    mLoadPlugins(void);
    //
    void
    mSendPluginInfoToBEs(void);
    //
    void
    mEnterPluginMain(void);

public:
    // Default timeout (in seconds)
    static const toolcommon::timeout_t sDefaultTimeout;
    // Default max number of retry attempts.
    static const toolcommon::retry_t sDefaultMaxRetries;
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
    //
    void
    mConnectMRNetTree(void);
    //
    static void
    registerComponent(void);
};

}
} // end gladius namespace

#endif
