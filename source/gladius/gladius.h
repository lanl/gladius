/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_H_INCLUDED
#define GLADIUS_H_INCLUDED

#include "term/term.h"
#include "mrnet/mrnet-fe.h"
#include "debugger/dbe.h"

namespace gladius {

class Gladius {
private:
    int argc;
    char **argv = nullptr;
    char **envp = nullptr;
    term::Terminal *term = nullptr;
    dbe::GladiusDBE *dbe = nullptr;
    mrnet::MRNetFE *mrnetFE = nullptr;

    Gladius(void);

public:
    ~Gladius(void);

    Gladius(
        int argc,
        const char **argv,
        const char **envp
    );

    void
    localBody(void);

    void
    remoteBody(void);

    void
    run(void);
};

} // end gladius namespace

#endif
