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

#include "core/core.h"
#include "core/utils.h"

#include "dsys/dsi.h"

#include <cstdio>
#include <cassert>
#include <iostream>

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


using namespace gladius;
using namespace gladius::dsysi;

namespace {
// This component's name.
const std::string CNAME = "****dsi";
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
 * The initial size of the output buffer. 16k should be plenty.
 */
const size_t DSI::sInitBufSize = 1024 * 16;

/**
 *
 */
const std::string DSI::sPromptString = "(gdb) ";

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
        w = waitpid(mGDBPID, &status, WUNTRACED | WCONTINUED);
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
    close(mToGDB[1]);
    close(mFromGDB[0]);
    //
    if (mFromGDBLineBuf) free(mFromGDBLineBuf);
}

/**
 *
 */
void
DSI::init(
    bool beVerbose
) {
    mBeVerbose = beVerbose;
    //
    VCOMP_COUT("Initializing the DSI..." << std::endl);
    // Get GDB's path.
    auto status =  core::utils::which("gdb", mPathToGDB);
    if (GLADIUS_SUCCESS != status) {
        GLADIUS_THROW(
            "It appears as if GDB is either "
            "not installed or not in your $PATH. "
            " Please fix this and try again."
        );
    }
    // Allocate initial string buffer.
    mFromGDBLineBuf = (char *)calloc(mCurLineBufSize, sizeof(*mFromGDBLineBuf));
    if (!mFromGDBLineBuf) GLADIUS_THROW_OOR();
    //
    if (-1 == pipe(mToGDB) || -1 == pipe(mFromGDB)) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
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
            int err = errno;
            auto errs = core::utils::getStrError(err);
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
        int err = errno;
        auto errs = core::utils::getStrError(err);
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
    assert(std::string(mFromGDBLineBuf) == sPromptString);
    //
    VCOMP_COUT("Done Initializing the DSI..." << std::endl);
}

/**
 *
 */
void
DSI::mWaitForPrompt(void)
{
    while (0 != strcmp(mFromGDBLineBuf, sPromptString.c_str())) {
        mGetGDBRespLine();
    }
}

/**
 *
 */
size_t
DSI::mGetGDBRespLine(void)
{
    char charBuf = '\0';
    size_t nRead = 0;
    while (1 == read(mFromGDB[0], &charBuf, 1)) {
        // Need more memory.
        if (nRead == mCurLineBufSize) {
            // Double the size.
            mCurLineBufSize *= 2;
            mFromGDBLineBuf = (char *)realloc(mFromGDBLineBuf, mCurLineBufSize);
            if (!mFromGDBLineBuf) GLADIUS_THROW_OOR();
        }
        mFromGDBLineBuf[nRead] = charBuf;
        if (charBuf == '\n') {
            // Don't forget to update nRead before we break.
            mFromGDBLineBuf[nRead++] = '\0';
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
        mGetGDBRespLine();
        result += std::string(mFromGDBLineBuf) + "\n";
    } while (0 != strcmp(mFromGDBLineBuf, sPromptString.c_str()));

    return result;
}

/**
 *
 */
void
DSI::attach(pid_t targetPID)
{
    VCOMP_COUT(
        "GDB PID: " << mGDBPID <<
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
