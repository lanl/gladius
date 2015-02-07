/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 2008-2012, Lawrence Livermore National Security, LLC
 */

#include "tool-be.h"

#include "core/core.h"
#include "core/session.h"
#include "tool-be/tool-be.h"

#include <cstdlib>
#include <cstdio>
#include <string>

#include <sys/types.h>
#include <unistd.h>


namespace {
// This component's name.
const std::string CNAME = "*gtoold";
// CNAME's color code.
const std::string NAMEC =
    gladius::core::colors::color().ansiBeginColor(gladius::core::colors::NONE);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
}

/**
 * Tool daemon main.
 */
int
main(
    int argc,
    char **argv,
    char **envp
) {
    using namespace gladius;
    using namespace gladius::toolbe;
    // Return status.
    int rs = EXIT_SUCCESS;
    //
    try {
        // No stdout buffering.
        setbuf(stdout, NULL);
        //
        const auto beVerbose = true;
        // Turn off colors first. They make logs look awful.
        core::colors::color().colorize(false);
        // FIXME
        ToolBE::redirectOutputTo("/tmp");
        // Let it begin!
        COMP_COUT << "Tool Daemon Started." << std::endl;
        //
        COMP_COUT << "*** PID: " << getpid() << std::endl;
        core::Args args(
            argc,
            const_cast<const char **>(argv),
            const_cast<const char **>(envp)
        );
        toolbe::ToolBE toolBE;
        toolBE.init(args, beVerbose);
        toolBE.connect();
        toolBE.mainLoop();
        toolBE.finalize();
    }
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
        rs = EXIT_FAILURE;
    }
    exit(rs);
}
