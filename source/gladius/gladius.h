/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_H_INCLUDED
#define GLADIUS_H_INCLUDED

#include <lldb/API/LLDB.h>

namespace gladius {

class Gladius {
private:

public:
    Gladius(int argc, char **argv, char **envp);

    void
    mainLoop(void);
};

} // end gladius namespace

#endif // GLADIUS_H_INCLUDED
