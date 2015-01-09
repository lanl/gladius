/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_GLADIUS_GLADIUS_H_INCLUDED
#define GLADIUS_GLADIUS_GLADIUS_H_INCLUDED

#include "core/core.h"
#include "core/session.h"
#include "ui/ui.h"

namespace gladius {

class Gladius {
private:
    //
    core::Args mArgs;
    //
    core::Session &mCurrentSession;
    //
    ui::UI &mUI;
    //
    Gladius(void);

public:
    /**
     *
     */
    ~Gladius(void);

    /**
     *
     */
    Gladius(const core::Args &args);

    /**
     *
     */
    void
    run(void);

    bool
    shutdown(void);

};

} // end gladius namespace

#endif
