/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Back-End (BE) API. The interface to the back-end tool actions.
 */

#pragma once

#include "tool-api/gladius-toolbe.h"

#include "core/core.h"
#include "mrnet/mrnet-be.h"
#include "gpa/core/gp-manager.h"
#include "gpa/core/gladius-plugin.h"

#include <string>

namespace gladius {
namespace toolbe {

class Tool::ToolBE {
private:
    //
    bool mBeVerbose;
    // Out UID for the job.
    int mUID;
    // Our MRNet back-end instance.
    mrnetbe::MRNetBE mMRNBE;
    // The name of the target plugin.
    std::string mPluginName;
    // The path to the plugin pack.
    std::string mPathToPluginPack;
    // The plugin manager.
    gpa::GladiusPluginManager mPluginManager;
    // The plugin pack for our current session.
    gpa::GladiusPluginPack mPluginPack;
    // The plugin instance pointer.
    gpi::GladiusPlugin *mBEPlugin = nullptr;
    //
    void
    mLoadPlugins(void);

public:
    //
    ToolBE(void);
    //
    ~ToolBE(void);
    //
    int
    init(bool beVerbose);
    //
    int
    create(int uid);
    //
    int
    connect(void);
    //
    static void
    redirectOutputTo(const std::string &base);
    //
    void
    enterPluginMain(void);
};

} // end toolbe namespace
} // end gladius namespace
