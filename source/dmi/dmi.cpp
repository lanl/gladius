/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the DMI.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "core/core.h"
#include "core/utils.h"

#include "dmi/dmi.h"

#include <cstdio>
#include <cassert>
#include <iostream>

#include <errno.h>
#include <string.h>
#include <signal.h>

using namespace gladius;
using namespace gladius::dmi;

namespace {
// This component's name.
const std::string CNAME = "****dmi";
//
const auto COMPC = core::colors::NONE;
// CNAME's color code.
const std::string NAMEC = core::colors::color().ansiBeginColor(COMPC);
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
 *
 */
DMI::DMI(void)
{
    (void)memset(mFromGDBLineBuf, '\0', sizeof(mFromGDBLineBuf));
}

/**
 *
 */
DMI::~DMI(void)
{
    // TODO close pipes.
    fclose(mTo);
    fclose(mFrom);
}

/**
 *
 */
void
DMI::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    //
    VCOMP_COUT("Initializing the DMI..." << std::endl);
    // Get GDB's path.
    auto status =  core::utils::which("gdb", mPathToGDB);
    if (GLADIUS_SUCCESS != status) {
        GLADIUS_THROW(
            "It appears as if GDB is either "
            "not installed or not in your $PATH. "
            " Please fix this and try again."
        );
    }
    if (-1 == pipe(mToGDB) || -1 == pipe(mFromGDB)) {
        auto errs = core::utils::getStrError(errno);
        GLADIUS_THROW("pipe(2): " + errs);
    }
    // TODO set O_NONBLOCK?
    // Create new process for GDB.
    mGDBPID = fork();
    ////////////////////////////////////////////////////////////////////////////
    // Child. Don't throw here.
    ////////////////////////////////////////////////////////////////////////////
    if (0 == mGDBPID) {
        close(mToGDB[1]);
        close(mFromGDB[0]);
        // Connect stdin and stdout
        if (-1 == dup2(mToGDB[0], STDIN_FILENO) ||
            -1 == dup2(mFromGDB[1], STDOUT_FILENO)) {
            auto errs = core::utils::getStrError(errno);
            GLADIUS_CERR << "dup2(2): " + errs << std::endl;
            exit(EXIT_FAILURE);
        }
        // Build the argv for execvp
        char *argv[4] = {
            (char *)mPathToGDB.c_str(),
            (char *)"--interpreter=mi",
            (char *)"--quiet",
            nullptr
        };
        //
        execvp(argv[0], argv);
        // Reached only on execvp failure.
        _exit(127);
    }
    // Fork failure.
    else if (-1 == mGDBPID) {
        auto errs = core::utils::getStrError(errno);
        GLADIUS_THROW("Cannot Create GDB Process: " + errs);
    }
    ////////////////////////////////////////////////////////////////////////////
    // Parent.
    ////////////////////////////////////////////////////////////////////////////
    // TODO check for running child.
    // Close unused.
    close(mToGDB[0]);
    close(mFromGDB[1]);
    // TODO add error checks
    mTo = fdopen(mToGDB[1], "w");
    // TODO not sure this is needed.
    mFrom = fdopen(mFromGDB[0], "r");
    //
    mWaitForPrompt();
    //
    assert(std::string(mFromGDBLineBuf) == "(gdb) ");
    //
    VCOMP_COUT("Done Initializing the DMI..." << std::endl);
}

/**
 *
 */
void
DMI::mWaitForPrompt(void)
{
    (void)memset(mFromGDBLineBuf, '\0', sizeof(mFromGDBLineBuf));
    // TODO add max iters? Timeout? Something?
    while (0 != strcmp(mFromGDBLineBuf, "(gdb) ")) {
        mGetGDBRespLine();
    }
}


size_t
DMI::mGetGDBRespLine(void)
{
    char charBuf = '\0';
    size_t nRead = 0;
    while (1 == read(mFromGDB[0], &charBuf, 1)) {
        mFromGDBLineBuf[nRead] = charBuf;
        if (charBuf == '\n') {
            mFromGDBLineBuf[nRead] = '\0';
            break;
        }
        ++nRead;
    }
    return nRead;
}

std::string
DMI::mDrainToString(void)
{
    std::string result = "";
    (void)memset(mFromGDBLineBuf, '\0', sizeof(mFromGDBLineBuf));
    while (0 != strcmp(mFromGDBLineBuf, "(gdb) ")) {
        mGetGDBRespLine();
        result += mFromGDBLineBuf;
        result += "\n";
    }
    return result;
}

/**
 *
 */
void
DMI::attach(pid_t targetPID)
{
    VCOMP_COUT(
        "GDB PID: " << mGDBPID <<
        " Attaching to Target PID: " << targetPID << std::endl
    );
    //
    std::string cmd = "attach " + std::to_string(targetPID) + "\n";
    fputs(cmd.c_str(), mTo);
    fflush(mTo);
    VCOMP_COUT("Attached!" << std::endl;);
    kill(targetPID, SIGCONT);
}

/**
 *
 */
int
DMI::sendCommand(
    const std::string &rawCMD
) {
    fputs(std::string(rawCMD + "\n").c_str(), mTo);
    fflush(mTo);
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
DMI::recvResp(
    std::string &outputIfSuccess
) {
    outputIfSuccess = mDrainToString();
    return GLADIUS_SUCCESS;
}
