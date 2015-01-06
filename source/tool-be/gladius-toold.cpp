/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-be.h"
#include "core/core.h"

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
    using namespace gladius::toolbe;
    GLADIUS_UNUSED(envp);

    FILE *test = fopen("/tmp/BE.txt", "wr+");
    fprintf(test, "HI FROM BE!!!\n");
    fclose(test);

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
        lmonRC = LMON_be_sendUsrData(NULL);
        lmonRC = LMON_be_recvUsrData(NULL);
        lmonRC = LMON_be_finalize();
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
