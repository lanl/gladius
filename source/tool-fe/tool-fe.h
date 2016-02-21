/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Front-End (FE) API. The interface to the tool actions.
 */

#pragma once

#include "core/core.h"
#include "tool-common/tool-common.h"
#include "app-launcher/app-launcher.h"
#include "mrnet/mrnet-fe.h"
#include "dspa/core/dsp-manager.h"
#include "dspa/core/gladius-dspi.h"

#include <string>
#include <thread>
#include <mutex>

namespace gladius {
namespace toolfe {

class ToolFE {
private:
    // Threading things ////////////////////////////////////////////////////////
    //
    ////////////////////////////////////////////////////////////////////////////
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
    // stdin copy
    int mStdInCopy = 0;
    // Out parallel application launcher.
    applauncher::AppLauncher mAppLauncher;
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
    // The name of the parallel application launcher.
    std::string mAppLauncherName;
    // The path to a valid plugin pack.
    std::string mPathToPluginPack;
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

} // end toolfe namespace
} // end gladius namespace
