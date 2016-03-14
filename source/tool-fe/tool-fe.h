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
#include "core/process-landscape.h"
#include "tool-common/tool-common.h"
#include "tool-common/session-key.h"
#include "dsys/cmdr.h"
#include "dsys/dsi.h"
#include "mrnet/mrnet-fe.h"
#include "plugin/core/gp-manager.h"
#include "plugin/core/gladius-plugin.h"

#include <string>
#include <thread>
#include <mutex>

namespace gladius {
namespace toolfe {

class ToolFE {
private:
    // Flag indicating whether or not we'll be verbose about our actions.
    bool mBeVerbose;
#if 0
    // stdin copy
    int mStdInCopy = 0;
#endif
    // Our distributed system interface.
    dsi::DSI mDSI;
    // Our parallel application launcher personality.
    cmdr::Commandr mCommandr;
    // Our target process landscape.
    core::ProcessLandscape mProcLandscape;
    // Our MRNet instance.
    mrnetfe::MRNetFE mMRNFE;
    // The plugin manager.
    gpa::GladiusPluginManager mPluginManager;
    // The plugin instance pointer.
    gpi::GladiusPlugin *mFEPlugin = nullptr;
    // Target application arguments.
    core::Args mAppArgs;
    // Launcher arguments.
    core::Args mLauncherArgs;
    // Connection timeout (in seconds).
    toolcommon::timeout_t mConnectionTimeoutInSec;
    // Max number of connection retries.
    toolcommon::retry_t mMaxRetries;
    // Unique ID for a given job.
    toolcommon::SessionKey mSessionKey;
    // The path to a valid plugin pack.
    std::string mPathToPluginPack;
    // The plugin pack for our current session.
    gpa::GladiusPluginPack mPluginPack;
    //
    int
    mSetupCore(void);
    //
    int
    mGetStateFromEnvs(void);
    //
    int
    mInitializeParallelLauncher(void);
    //
    int
    mPreToolInitActons(void);
    //
    int
    mDetermineProcLandscape(void);
    //
    int
    mBuildNetwork(void);
    //
    void
    mForwardEnvsToBEsIfSetOnFE(void);
    //
    int
    mPostToolInitActons(void);
    //
    int
    mLaunchUserApp(void);
    //
    int
    mInitiateToolLashUp(void);
    //
    int
    mPublishConnectionInfo(void);
    //
    int
    mLoadPlugins(void);
    //
    int
    mSendPluginInfoToBEs(void);
    //
    int
    mEnterPluginMain(void);

public:
    // Default timeout (in seconds)
    static constexpr toolcommon::timeout_t sDefaultTimeout = 30;
    // Default max number of retry attempts.
    static constexpr toolcommon::retry_t sDefaultMaxRetries = 8;
    //
    ToolFE(void);
    //
    ~ToolFE(void) = default;
    //
    int
    main(
        const core::Args &appArgv,
        const core::Args &launcherArgv
    );
    //
    int
    mConnectMRNetTree(void);
    //
    static void
    registerComponent(void);
}; // class ToolFE

} // end toolfe namespace
} // end gladius namespace
