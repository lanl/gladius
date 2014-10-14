/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the Debugger BackEnd interface.
 */

#ifndef GLADIUS_DEBUGGER_DBE_H_INCLUDED
#define GLADIUS_DEBUGGER_DBE_H_INCLUDED

#include "lldb/Utility/PseudoTerminal.h"

#include "lldb/API/SBDefines.h"
#include "lldb/API/SBBroadcaster.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBError.h"

#include <lldb/API/LLDB.h>

namespace gladius {
namespace dbe {

class GladiusDBE {
private:
    GladiusDBE(void);

public:
    ~GladiusDBE(void);

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

#endif
