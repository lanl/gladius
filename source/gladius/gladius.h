/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_GLADIUS_GLADIUS_H_INCLUDED
#define GLADIUS_GLADIUS_GLADIUS_H_INCLUDED

#include "ui/ui.h"

namespace gladius {

class Gladius {
private:
    int mArgc;
    char **mArgV = nullptr;
    char **mEnvp = nullptr;
    ui::UI *mUI = nullptr;

    Gladius(void);

public:
    ~Gladius(void);

    Gladius(
        int argc,
        const char **argv,
        const char **envp
    );

    void
    run(void);
};

} // end gladius namespace

#endif
