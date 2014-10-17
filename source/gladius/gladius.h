/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_H_INCLUDED
#define GLADIUS_H_INCLUDED

#include "mrnet/mrnet-fe.h"
#include "debugger/dbe.h"

namespace gladius {

class Gladius {
private:
    Gladius(void);
    dbe::GladiusDBE *dbe = nullptr;
    mrnet::MRNetFE *mrnetFE = nullptr;

public:
    ~Gladius(void);

    Gladius(
        int argc,
        const char **argv,
        const char **envp
    );

    void
    mainLoop(void);
};

} // end gladius namespace

#endif
