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

#include "lmon_api/lmon_be.h"

#include <cstdlib>
#include <string>


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

    try {
        auto beVerbose = true;
        // Turn off colors. They make logs look awful.
        core::colors::color().colorize(false);
        ToolBE::redirectOutputTo("/tmp");
        core::Args args(
            argc,
            const_cast<const char **>(argv),
            const_cast<const char **>(envp)
        );
        toolbe::ToolBE toolBE;
        toolBE.init(args, beVerbose);
        // FIXME
    }

#if 0
        mLMONBE::connect();

        int mLMONRank = 0;
        LMON_be_getMyRank(&mLMONRank);
        std::cout << "Hi from: " << mLMONRank << std::endl;

        //lmonRC = LMON_be_recvUsrData(NULL);
        //lmonRC = LMON_be_sendUsrData(NULL);
        lmonRC = LMON_be_finalize();
        if (LMON_OK != lmonRC) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_ready", lmonRC);
        }

    }
#endif
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
