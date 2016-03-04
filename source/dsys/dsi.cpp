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
    //
    if (mTo) fclose(mTo);
    if (mFrom) fclose(mFrom);
    //
    if (mToAppl[1]   != -1) close(mToAppl[1]);
    if (mFromAppl[0] != -1) close(mFromAppl[0]);
    //
    if (mFromDSysLineBuf) free(mFromDSysLineBuf);
    // Nothing to do for child.
    if (-1 == mApplPID) return;
    // Wait for dsys child process
    static const int lsMaxRetries = 6;
    pid_t w;
    for (int retry = 0;
         retry < lsMaxRetries || (!WIFEXITED(w) && !WIFSIGNALED(w));
         ++retry
    ) {
        int status;
        w = waitpid(mApplPID, &status, WUNTRACED | WNOHANG);
        if (-1 == w) {
            int err = errno;
            auto errs = core::utils::getStrError(err);
            GLADIUS_CERR << "waitpid(2): " + errs << endl;
            break;
        }
        if (0 == w) {
            VCOMP_COUT("Waiting for dsys to complete..." << endl);
            sleep(1);
            if (retry % 2) {
                VCOMP_COUT("Done waiting for dsys..." << endl);
                // Hit the launcher a couple of times. Sometimes they need that.
                core::utils::sendSignal(mApplPID, SIGTERM);
                core::utils::sendSignal(mApplPID, SIGTERM);
                GLADIUS_CERR << "WARNING: An error occurred while"
                             << "running a parallel job." << endl;
                GLADIUS_CERR << "We cleaned up as best we could, "
                             << "but there may be " << endl;
                GLADIUS_CERR << "leftover " << sDSysName << " processes."
                             << endl;
            }
        }
        else if (mApplPID == w) {
            if (WIFEXITED(status)) {
                VCOMP_COUT(
                    "dsys exited with status: " << WEXITSTATUS(status) << endl
                );
            }
            else if (WIFSIGNALED(status)) {
                VCOMP_COUT(
                    "dsys signaled: " << WTERMSIG(status) << endl
                );
            }
            else if (WIFSTOPPED(status)) {
                VCOMP_COUT(
                    "dsys stopped by signal: " << WSTOPSIG(status) << endl
                );
            }
            break;
        }
    }
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
    VCOMP_COUT("Waiting for parallel job..." << std::endl);
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
int
DSI::mDrainToString(
    std::string &result
) {
    result = "";
    mGetRespLine();
    while (0 != strcmp(mFromDSysLineBuf, sPromptString)) {
        result += std::string(mFromDSysLineBuf) + "\n";
        mGetRespLine();
    }
    return GLADIUS_SUCCESS;
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
    return mDrainToString(outputIfSuccess);
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
    while (std::getline(ss, line)) {
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
    //
    return GLADIUS_SUCCESS;
}

/**
 * Sends shutdown command to dsys.
 */
int
DSI::shutdown(void)
{
    using namespace std;
    //
    VCOMP_COUT("Shutting down..." << endl);
    //
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mSendCommand("q"))) {
        return rc;
    }
    return rc;
}

/**
 * Publishes tool connection info across target resources.
 */
int
DSI::publishConnectionInfo(
    toolcommon::SessionKey sessionKey,
    const std::vector<std::string> &leafInfos
) {
    using namespace std;
    //
    VCOMP_COUT("Publishing connection info..." << endl);
    // See protocol in dsys.cpp
    int rc = GLADIUS_SUCCESS;
    if (GLADIUS_SUCCESS != (rc = mSendCommand("c"))) {
        return rc;
    }
    const string sKey(sessionKey);
    VCOMP_COUT("- Sending session key: " << sKey << endl);
    if (GLADIUS_SUCCESS != (rc = mSendCommand(sKey))) {
        return rc;
    }
    const string nInfosStr = to_string(leafInfos.size());
    VCOMP_COUT("- Sending number of infos: " << nInfosStr << endl);
    if (GLADIUS_SUCCESS != (rc = mSendCommand(nInfosStr))) {
        return rc;
    }
    VCOMP_COUT("- Sending encoded connection info..." << endl);
    for (const auto &li : leafInfos) {
        if (GLADIUS_SUCCESS != (rc = mSendCommand(li))) {
            return rc;
        }
    }
    return rc;
}
