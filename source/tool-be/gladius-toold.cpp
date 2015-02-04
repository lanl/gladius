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
        core::Args args(
            argc,
            const_cast<const char **>(argv),
            const_cast<const char **>(envp)
        );
        toolbe::ToolBE toolBE;
        toolBE.init(args, true /* be verbose */);
        // FIXME
        toolBE.redirectOutputTo("/tmp");
    }

#if 0
        ////////////////////////////////////////////////////////////////////////
        toolcommon::ProcessTable mProcTab;
        auto numProcTabEntries = 0;
        auto rc = LMON_be_getMyProctabSize(&numProcTabEntries);
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_getMyProctabSize", rc);
        }
        // Allocate room for the entries.
        mProcTab = toolcommon::ProcessTable(numProcTabEntries);
        // Now populate the thing...
        int pSize = 0;
        rc = LMON_be_getMyProctab(
                 mProcTab.procTab(),
                 &pSize,
                 numProcTabEntries // Max Length
             );
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_fe_getProctable", rc);
        }
        if (true) {
            std::cout << "Done Getting Process Table" << std::endl;
            mProcTab.dumpTo(std::cout);
        }
        ////////////////////////////////////////////////////////////////////////
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
