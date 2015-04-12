/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Back-End (BE) API. The interface to the back-end tool actions.
 */

#ifndef GLADIUS_TOOL_FE_TOOL_BE_H_INCLUDED
#define GLADIUS_TOOL_FE_TOOL_BE_H_INCLUDED

#include "core/core.h"
#include "core/args.h"
#include "lmon/lmon-be.h"
#include "mrnet/mrnet-be.h"
#include "dspa/core/dsp-manager.h"
#include "dspa/core/gladius-dspi.h"

#include <string>

namespace gladius {
namespace toolbe {

class ToolBE {
private:
    //
    bool mBeVerbose = false;
    //
    core::Args mArgs;
    // Our LaunchMON back-end instance.
    lmonbe::LaunchMonBE mLMONBE;
    // Our MRNet back-end instance.
    mrnetbe::MRNetBE mMRNBE;
    // My process table (not the job one, but mine).
    toolcommon::ProcessTable mProcTab;
    // The name of the target plugin. 
    std::string mPluginName;
    // The path to the plugin pack.
    std::string mPathToPluginPack;
    // The plugin manager.
    dspa::DSPManager mDSPManager;
    // The plugin pack for our current session.
    dspa::DSPluginPack mPluginPack;
    ////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ////////////////////////////////////////////////////////////////////////////
    void
    mInitLMON(
        const core::Args &args,
        bool beVerbose
    );
    //
    void
    mLoadPlugins(void);

public:
    //
    ToolBE(void);
    //
    ~ToolBE(void);
    //
    void
    init(
        const core::Args &args,
        bool beVerbose
    );
    //
    void
    connect(void);
    //
    static void
    redirectOutputTo(const std::string &base);
    //
    void
    enterDomain(void);
    //
    void
    finalize(void);
};

} // end toolbe namespace
} // end gladius namespace

#endif
