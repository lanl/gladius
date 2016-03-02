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
#include <sstream>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
DSI::DSI(
    void
) : mCurLineBufSize(sInitBufSize)
  , mApplPID(-1)
{
    memset(mToAppl,   -1, sizeof(mToAppl));
    memset(mFromAppl, -1, sizeof(mFromAppl));
}

/**
 *
 */
DSI::~DSI(void)
{
    using namespace std;
    if (-1 != mApplPID) {
        // Wait for dsys child process
        pid_t w;
        int status;
        do {
            w = waitpid(mApplPID, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                int err = errno;
                auto errs = core::utils::getStrError(err);
                GLADIUS_CERR_WARN << "pipe(2): " + errs << endl;
            }
            if (WIFEXITED(status)) {
                VCOMP_COUT(
                    "Appl Exited Status: " << WEXITSTATUS(status) << endl
                );
            }
            else if (WIFSIGNALED(status)) {
                VCOMP_COUT(
                    "Appl Killed By Signal: " << WTERMSIG(status) << endl
                );
            }
            else if (WIFSTOPPED(status)) {
                VCOMP_COUT(
                    "Appl Stopped By Signal: " << WSTOPSIG(status) << endl
                );
            }
            else if (WIFCONTINUED(status)) {
                VCOMP_COUT("Appl Continued..." << endl);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    // Other cleanup.
    if (mTo) fclose(mTo);
    if (mFrom) fclose(mFrom);
    //
    if (mToAppl[1]   != -1) close(mToAppl[1]);
    if (mFromAppl[0] != -1) close(mFromAppl[0]);
    //
    if (mFromDSysLineBuf) free(mFromDSysLineBuf);
}

/**
 *
 */
int
DSI::init(
    const cmdr::Commandr &cmdr,
    bool beVerbose
) {
    using namespace std;
    using namespace core;

    mBeVerbose = beVerbose;
    mCommandr = cmdr;
    //
    VCOMP_COUT("Initializing the DSI..." << std::endl);
    // Allocate initial string buffer.
    mFromDSysLineBuf = (char *)calloc(mCurLineBufSize, sizeof(*mFromDSysLineBuf));
    if (!mFromDSysLineBuf) GLADIUS_THROW_OOR();
    //
    if (-1 == pipe(mToAppl) || -1 == pipe(mFromAppl)) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_CERR << utils::formatCallFailed(
                            "pipe(2): " + errs, GLADIUS_WHERE
                        ) << endl;
        return GLADIUS_ERR_IO;
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
            GLADIUS_CERR << utils::formatCallFailed(
                                "dup2(2): " + errs, GLADIUS_WHERE
                            ) << endl;
            exit(EXIT_FAILURE);
        }
        // Build the argv for execvp
        std::vector<std::string> dsysArgv = {sDSysName};
        core::Args argv = mCommandr.getLaunchCMDFor(core::Args(dsysArgv));
        //
        execvp(argv.argv()[0], argv.argv());
        // Reached only on execvp failure.
        _exit(127);
    }
    // Fork failure.
    else if (-1 == mApplPID) {
        int err = errno;
        auto errs = core::utils::getStrError(err);
        GLADIUS_CERR << utils::formatCallFailed(
                            "fork(2): " + errs, GLADIUS_WHERE
                        ) << endl;
        return GLADIUS_ERR;
    }
    ////////////////////////////////////////////////////////////////////////////
    // Parent.
    ////////////////////////////////////////////////////////////////////////////
    // TODO check for running child.
    // Close unused.
    close(mToAppl[0]);
    close(mFromAppl[1]);
    mTo = fdopen(mToAppl[1], "w");
    // TODO not sure this is needed.
    mFrom = fdopen(mFromAppl[0], "r");
    if (!mTo || !mFrom) {
        GLADIUS_CERR << utils::formatCallFailed(
                            "fdopen(3): ", GLADIUS_WHERE
                        ) << endl;
        return GLADIUS_ERR_IO;
    }
    //
    mWaitForPrompt();
    //
    assert(std::string(mFromDSysLineBuf) == sPromptString);
    //
    VCOMP_COUT("Done initializing the DSI..." << std::endl);
    //
    return GLADIUS_SUCCESS;
}

/**
 *
 */
void
DSI::mWaitForPrompt(void)
{
    VCOMP_COUT("Waiting for prompt..." << std::endl);
    while (0 != strcmp(mFromDSysLineBuf, sPromptString)) {
        mGetRespLine();
    }
    VCOMP_COUT("Done waiting for prompt..." << std::endl);
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
    mGetRespLine();
    while (0 != strcmp(mFromDSysLineBuf, sPromptString)) {
        result += std::string(mFromDSysLineBuf) + "\n";
        mGetRespLine();
    }
    return result;
}

/**
 *
 */
int
DSI::mSendCommand(
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
DSI::mRecvResp(
    std::string &outputIfSuccess
) {
    outputIfSuccess = mDrainToString();
    return GLADIUS_SUCCESS;
}

/**
 *
 */
int
DSI::getProcessLandscape(
    core::ProcessLandscape &pl
) {
    using namespace std;
    // Gather info from dsys
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mSendCommand("h"))) {
        return rc;
    }
    string respStr;
    if (GLADIUS_SUCCESS != (rc = mRecvResp(respStr))) {
        return rc;
    }
    // Now process and populate the landscape object.
    stringstream ss(respStr);
    string line;
    char hnbuf[HOST_NAME_MAX] = {'\0'};
    int hnn = 0;
    while (std::getline(ss, line, '\n')) {
        int n = sscanf(line.c_str(), "%s %d", hnbuf, &hnn);
        if (2 != n) {
            GLADIUS_CERR << "Invalid response detected: " << line << endl;
            return GLADIUS_ERR;
        }
        if (GLADIUS_SUCCESS != (rc = pl.insert(hnbuf, hnn))) {
            GLADIUS_CERR << "Could not update process landscape!" << endl;
            return rc;
        }
    }
    return GLADIUS_SUCCESS;
}

/**
 * Sends shutdown command to dsys.
 */
int
DSI::shutdown(void)
{
    using namespace std;
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mSendCommand("q"))) {
        return rc;
    }
    return rc;
}
