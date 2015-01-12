/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-be.h"

#include "core/core.h"
#include "tool-common/tool-common.h"

#include "lmon_api/lmon_be.h"

#include <cstdlib>
#include <string>

/**
 * Tool daemon main
 */
int
main(
    int argc,
    char **argv,
    char **envp
) {
    using namespace gladius;
    using namespace gladius::toolbe;
    GLADIUS_UNUSED(envp);

    std::string fName = "/tmp/BE-" + std::to_string(getpid()) + ".txt";
    // TODO FIXME
    freopen(fName.c_str(), "w", stdout);

    try {
        auto lmonRC = LMON_be_init(LMON_VERSION, &argc, &argv);
        if (LMON_OK != lmonRC) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_init", lmonRC);
        }
        lmonRC = LMON_be_handshake(NULL);
        if (LMON_OK != lmonRC) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_handshake", lmonRC);
        }
        lmonRC = LMON_be_ready(NULL);
        if (LMON_OK != lmonRC) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_ready", lmonRC);
        }
        ////////////////////////////////////////////////////////////////////////
        toolcommon::ProcessTable mProcTab;
        auto numProcTabEntries = 0;
        auto rc = ::LMON_be_getMyProctabSize(
                      &numProcTabEntries
                  );
        if (LMON_OK != rc) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_getMyProctabSize", rc);
        }
        // Allocate room for the entries.
        mProcTab.allocate(numProcTabEntries);
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
            mProcTab.dump();
        }
        ////////////////////////////////////////////////////////////////////////
#if 0
        lmonRC = LMON_be_sendUsrData(NULL);
        lmonRC = LMON_be_recvUsrData(NULL);
#endif
        lmonRC = LMON_be_finalize();
        mProcTab.deallocate();
        if (LMON_OK != lmonRC) {
            GLADIUS_THROW_CALL_FAILED_RC("LMON_be_ready", lmonRC);
        }

    }
    catch (const std::exception &e) {
        GLADIUS_CERR << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
