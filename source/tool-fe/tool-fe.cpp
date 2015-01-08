/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-fe.h"
#include "tool-be/tool-be.h"

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius::toolfe;

namespace {
// This component's name.
static const std::string CNAME = "tool-fe";
// CNAME's color code.
static const std::string NAMEC = gladius::core::utils::ansiBeginColorMagenta();
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
/**
 *
 */
void
echoLaunchStart(const gladius::core::Args &args)
{
    std::string lstr;
    for (auto i = 0; i < args.argc(); ++i) {
        lstr += args.argv()[i];
        lstr += " ";
    }
    GLADIUS_COUT_STAT << "Launch Sequence Initiated..." << std::endl;
    GLADIUS_COUT_STAT << "Starting: " << lstr << std::endl;
}
}

/**
 * Tool front-end constructor.
 */
ToolFE::ToolFE(
    void
) : mBeVerbose(false)
{
    if (core::utils::envVarSet(GLADIUS_TOOL_FE_VERBOSE_STR)) {
        mBeVerbose = true;
        mLMON.verbose(mBeVerbose);
    }
}

/**
 * Returns tool front-end help string.
 */
std::string
ToolFE::helpText(void)
{
    return std::string {"toolFE help string goes here..."};
}

/**
 * Returns whether or not the tool-fe's environment setup is sane.
 */
bool
ToolFE::envSane(std::string &whatsWrong)
{
    GLADIUS_UNUSED(whatsWrong);
    bool sane = true;
    return sane;
}

/**
 * Responsible for running the tool front-end instance. This is the tool-fe
 * entry point from a caller's perspective.
 */
void
ToolFE::run(
    const core::Args &args
) {
    // First make sure that all the required bits are set before we get to
    // launching anything.
    std::string whatsWrong;
    if (!envSane(whatsWrong)) {
        GLADIUS_CERR << whatsWrong << std::endl;
        return;
    }
    // If we are here, then our environment is sane enough to start...
    try {
        mAppArgs = args;
        // FIXME dup stdout?
        mLocalBody();
    }
    // If something went south, just print the haps and return to the top-level
    // REPL. Insulate the caller by catching things and handling them here.
    catch (const std::exception &e) {
        GLADIUS_CERR << std::endl << e.what() << std::endl;
    }
}

/**
 * The local tool front-end thread (the main thread).
 */
void
ToolFE::mLocalBody(void)
{
    try {
        // One-time init things go in init.
        mLMON.init();
        std::thread beThread(&ToolFE::mRemoteBody, this);
        std::unique_lock<std::mutex> lock(mtFEBELock);
        mtBELaunchComplete.wait(lock);
        beThread.join();
    }
    catch (const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 * The thread that interacts with the tool back-end. This is NOT the main
 * thread, so this is why we don't throw in the exceptional case.
 */
void
ToolFE::mRemoteBody(void)
{
    try {
        echoLaunchStart(mAppArgs);
        // And so it begins...
        mLMON.launchAndSpawnDaemons(mAppArgs);
    }
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
    }
    // Notify main thread unconditionally.
    mtBELaunchComplete.notify_one();
}
