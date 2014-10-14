/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/gladius-exception.h"
#include "gladius.h"

#include <cstdlib>

#include <lldb/API/LLDB.h>

int
main(int argc, char **argv, char **envp)
{
    using namespace gladius;
    try {
        Gladius gladius(argc, argv, envp);
        gladius.mainLoop();
    } catch (...) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
