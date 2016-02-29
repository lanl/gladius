/*
 * Copyright (c) 2016      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the dsys interface.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dsys/dsi.h"

#include "core/core.h"
#include "core/utils.h"

#include <cstdio>
#include <cassert>
#include <iostream>

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace gladius;
using namespace gladius::dsi;

namespace {
// This component's name.
const std::string CNAME = "****dsi";
//
const auto COMPC = core::colors::GREEN;
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
 * The prompt string we are expecting when interacting with dsys.
 */
const char DSI::sPromptString[] = "(dsys) ";

/**
 * The name of the executable that we are interacting with.
 */
const char DSI::sDSysName[] = "gladius-dsys";

/**
 *
 */
DSI::DSI(void)
{
    mCurLineBufSize = sInitBufSize;
}

/**
 *
 */
DSI::~DSI(void)
{
    using namespace std;
    // Wait for GDB (child)
    pid_t w;
    int status;
    do {
        w = waitpid(mApplPID, &status, WUNTRACED | WCONTINUED);
        if (w == -1) {
            int err = errno;
            auto errs = core::utils::getStrError(err);
            GLADIUS_THROW("pipe(2): " + errs);
        }
        if (WIFEXITED(status)) {
            VCOMP_COUT("GDB Exited Status: " << WEXITSTATUS(status) << endl);
        }
        else if (WIFSIGNALED(status)) {
            VCOMP_COUT("GDB Killed By Signal: " << WTERMSIG(status) << endl);
        }
        else if (WIFSTOPPED(status)) {
            VCOMP_COUT("GDB Stopped By Signal: " << WSTOPSIG(status) << endl);
        }
        else if (WIFCONTINUED(status)) {
            VCOMP_COUT("GDB Continued..." << endl);
        }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    // Other cleanup.
    fclose(mTo);
    fclose(mFrom);
    //
    close(mToAppl[1]);
    close(mFromAppl[0]);
    //
    if (mFromDSysLineBuf) free(mFromDSysLineBuf);
}

/**
 *
 */
void
DSI::init(
    const applauncher::AppLauncher &appl,
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    //
    VCOMP_COUT("Initializing the DSI..." << std::endl);
    // Get dsys' path.
    mPathToAppl = appl.which();
    // Allocate initial string buffer.
    mFromDSysLineBuf = (char *)calloc(mCurLineBufSize, sizeof(*mFromDSysLineBuf));
    if (!mFromDSysLineBuf) GLADIUS_THROW_OOR();
    //
    if (-1 == pipe(mToAppl) || -1 == pipe(mFromAppl)) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_THROW("pipe(2): " + errs);
    }
    // TODO set O_NONBLOCK?
    // Create new process for GDB.
    mApplPID = fork();
    ////////////////////////////////////////////////////////////////////////////
    // Child. Don't throw here.
    ////////////////////////////////////////////////////////////////////////////
    if (0 == mApplPID) {
        close(mToAppl[1]);
        close(mFromAppl[0]);
        // Connect stdin and stdout
        if (-1 == dup2(mToAppl[0], STDIN_FILENO) ||
            -1 == dup2(mFromAppl[1], STDOUT_FILENO)) {
            int err = errno;
            auto errs = core::utils::getStrError(err);
            GLADIUS_CERR << "dup2(2): " + errs << std::endl;
            exit(EXIT_FAILURE);
        }
        // Build the argv for execvp
        char *argv[4] = {
            (char *)mPathToAppl.c_str(),
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
    else if (-1 == mApplPID) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_THROW("Cannot create dsys processes: " + errs);
    }
    ////////////////////////////////////////////////////////////////////////////
    // Parent.
    ////////////////////////////////////////////////////////////////////////////
    // TODO check for running child.
    // Close unused.
    close(mToAppl[0]);
    close(mFromAppl[1]);
    // TODO add error checks
    mTo = fdopen(mToAppl[1], "w");
    // TODO not sure this is needed.
    mFrom = fdopen(mFromAppl[0], "r");
    //
    mWaitForPrompt();
    //
    assert(std::string(mFromDSysLineBuf) == sPromptString);
    //
    VCOMP_COUT("Done Initializing the DSI..." << std::endl);
}

/**
 *
 */
void
DSI::mWaitForPrompt(void)
{
    while (0 != strcmp(mFromDSysLineBuf, sPromptString)) {
        mGetRespLine();
    }
}

/**
 *
 */
size_t
DSI::mGetRespLine(void)
{
    char charBuf = '\0';
    size_t nRead = 0;
    while (1 == read(mFromAppl[0], &charBuf, 1)) {
        // Need more memory.
        if (nRead == mCurLineBufSize) {
            // Double the size.
            mCurLineBufSize *= 2;
            mFromDSysLineBuf = (char *)realloc(mFromDSysLineBuf, mCurLineBufSize);
            if (!mFromDSysLineBuf) GLADIUS_THROW_OOR();
        }
        mFromDSysLineBuf[nRead] = charBuf;
        if (charBuf == '\n') {
            // Don't forget to update nRead before we break.
            mFromDSysLineBuf[nRead++] = '\0';
            break;
        }
        ++nRead;
    }
    return nRead;
}

/**
 *
 */
std::string
DSI::mDrainToString(void)
{
    std::string result = "";
    do {
        mGetRespLine();
        result += std::string(mFromDSysLineBuf) + "\n";
    } while (0 != strcmp(mFromDSysLineBuf, sPromptString));

    return result;
}

/**
 *
 */
void
DSI::attach(pid_t targetPID)
{
    VCOMP_COUT(
        "GDB PID: " << mApplPID <<
        " Attaching to Target PID: " << targetPID << std::endl
    );
    //
    std::string cmd = "attach " + std::to_string(targetPID) + "\n";
    fputs(cmd.c_str(), mTo);
    fflush(mTo);
    VCOMP_COUT("Attached!" << std::endl);
    if (0 != core::utils::sendSignal(targetPID, SIGCONT)) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_THROW("Signal delivery failed: " + errs);
    }
}

/**
 *
 */
int
DSI::sendCommand(
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
DSI::recvResp(
    std::string &outputIfSuccess
) {
    outputIfSuccess = mDrainToString();
    return GLADIUS_SUCCESS;
}
