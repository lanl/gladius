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
    int mArgc;
    char **mArgV = nullptr;
    char **mEnvp = nullptr;
    term::Terminal *mTerm = nullptr;
    dbe::GladiusDBE *mDBE = nullptr;
    mrnet::MRNetFE *mMRNetFE = nullptr;

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
