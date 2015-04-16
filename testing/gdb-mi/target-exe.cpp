/*
 * Copyright (c)      2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include <cstdlib>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>


int
main(void)
{
    int done = 0;
    std::cout << "starting... pid: " << getpid() << std::endl;
    while (!done) {
        sleep(1);
    }
    return EXIT_SUCCESS;
}
