/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 */

#include "tool-be/tool-be.h"

#include "core/core.h"
#include "core/colors.h"
#include "core/env.h"
#include "tool-common/tool-common.h"

#include <cstdlib>
#include <string>
#include <limits.h>
#include <signal.h>
#include <errno.h>

using namespace gladius;
using namespace gladius::toolbe;

////////////////////////////////////////////////////////////////////////////////
// ToolBE
////////////////////////////////////////////////////////////////////////////////
namespace {
// This component's name.
const std::string CNAME = "tool-be";
// CNAME's color code.
const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::NONE);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
// Output if this component is being verbose.
#define VCOMP_COUT(streamInsertions)                                           \
do {                                                                           \
    if (this->mBeVerbose) {                                                    \
        COMP_COUT << streamInsertions;                                         \
    }                                                                          \
} while (0)
} // end namespace

/**
 * Redirects stdout and stderr to a base directory with (hopefully) a unique
 * name. It is assumed that the base directory already exists.
 */
/* (static) */ void
Tool::ToolBE::redirectOutputTo(
    const std::string &base
) {
    static const auto pathSep = core::utils::osPathSep;
    std::string fName = base + pathSep
                      + PACKAGE + "-"
                      + core::utils::getHostname() + "-"
                      + std::to_string(getpid()) + ".txt";
    //
    FILE *outRedirectFile = freopen(fName.c_str(), "w", stdout);
    if (!outRedirectFile) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_THROW_CALL_FAILED_RC("freopen(3): " + errs, err);
    }
    //
    outRedirectFile = freopen(fName.c_str(), "w", stderr);
    if (!outRedirectFile) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_THROW_CALL_FAILED_RC("freopen(3): " + errs, err);
    }
}

/**
 * Constructor.
 */
Tool::ToolBE::ToolBE(
    void
) : mBeVerbose(false) { ; }

/**
 * Destructor.
 */
Tool::ToolBE::~ToolBE(void) = default;

/**
 *
 */
int
Tool::ToolBE::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    VCOMP_COUT("Initializing tool back-end..." << std::endl);
    return mMRNBE.init(mBeVerbose);
}

/**
 *
 */
int
Tool::ToolBE::create(int uid)
{
    VCOMP_COUT("Creating tool back-end..." << std::endl);
    mUID = uid;
    return mMRNBE.create(uid);
}

/**
 *
 */
int
Tool::ToolBE::connect(void)
{
    VCOMP_COUT("Connecting tool back-end..." << std::endl);
    return mMRNBE.connect();
}

#if 0
/**
 *
 */
void
Tool::ToolBE::mLoadPlugins(void)
{
    using namespace std;

    VCOMP_COUT("Loading Plugins." << std::endl);
    // We need to first get the info from the FE. The FE is responsible for
    // making sure that the requested plugin is valid, so we minimize the amount
    // of validation that we have to do on our end. We also rely on the FE to
    // find the path to the plugin pack. We don't want N (N = # of daemons) to
    // be statting around looking for plugins.
    mMRNBE.pluginInfoRecv(mPluginName, mPathToPluginPack);
    // Okay, now we know what to load. So, do that now.
    VCOMP_COUT("Loading Plugins." << std::endl);
    // Get the front-end plugin pack.
    mPluginPack = mPluginManager.getPluginPackFrom(
                      gpa::GladiusPluginPack::PluginBE,
                      mPathToPluginPack
                  );
    //
    auto *fePluginInfo = mPluginPack.pluginInfo;
    GLADIUS_COUT_STAT << "Back-End Plugin Info:" << endl;
    GLADIUS_COUT_STAT << "*Name      : " << fePluginInfo->pluginName << endl;
    GLADIUS_COUT_STAT << "*Version   : " << fePluginInfo->pluginVersion << endl;
    GLADIUS_COUT_STAT << "*Plugin ABI: " << fePluginInfo->pluginABI << endl;
    //
    mBEPlugin = fePluginInfo->pluginConstruct();
    //
    VCOMP_COUT("Done Loading Plugins." << std::endl);
}

/**
 *
 */
void
Tool::ToolBE::enterPluginMain(void)
{
    VCOMP_COUT("Entering Plugin Main." << std::endl);
    //
    try {
        const char *dummyArgv[] = {"dummy-arg-0", nullptr};
        int dummyArgc = 1;
        gladius::core::Args dummyAppArgs(dummyArgc, dummyArgv, dummyArgv);
        gpi::GladiusPluginArgs pluginArgs(
            mPathToPluginPack,
            dummyAppArgs,
            mProcTab,
            mMRNBE.getProtoStream(),
            mMRNBE.getNetwork()
        );
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Back-end Plugin Entry Point.
        mBEPlugin->pluginMain(pluginArgs);
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
    //
    VCOMP_COUT("Exited Plugin Main." << std::endl);
}
#endif
