/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the debugger backend interface.
 */

#ifndef GLADIUS_LLDB_H_INCLUDED
#define GLADIUS_LLDB_H_INCLUDED

#include <lldb/API/LLDB.h>

namespace gladius {
namespace dbe {

class GladiusDBE {
private:
    GladiusDBE(void);

public:
    GladiusDBE(
        int argc,
        char **argv,
        char **envp
    );

    void
    mainLoop(void);
};

} // end dbe namespace
} // end gladius namespace

#endif // GLADIUS_H_INCLUDED
