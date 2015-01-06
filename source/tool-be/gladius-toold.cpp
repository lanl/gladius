/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-be.h"

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

    GLADIUS_UNUSED(argc);
    GLADIUS_UNUSED(argv);
    GLADIUS_UNUSED(envp);

    auto lmonRC =  LMON_be_init(LMON_VERSION, &argc, &argv);

    return EXIT_SUCCESS;
}
