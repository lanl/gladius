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

#include <cassert>
#include <iostream>

#include <errno.h>
#include <string.h>

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
    close(mToGDB[0]);
    close(mFromGDB[1]);
    // TODO check for running child.
    char charBuf;
    std::cout << "GOT: " << mFromGDBLineBuf << std::endl;
    // TODO FIXME
    int32_t bCount = 0;
    bool skip = true;
    while (1 == read(mFromGDB[0], &charBuf, 1)) {
        if (charBuf == '=' && skip) {
            while (1 == read(mFromGDB[0], &charBuf, 1)) {
                if (charBuf == '\n') {
                    skip = false;
                    break;
                }
            }
        }
        else {
            mFromGDBLineBuf[bCount] = charBuf;
            if (charBuf == '\n') {
                mFromGDBLineBuf[bCount] = '\0';
                break;
            }
            ++bCount;
        }
    }
    assert(std::string(mFromGDBLineBuf) == "(gdb)");
    //
    VCOMP_COUT("Done Initializing the DMI..." << std::endl);
}
