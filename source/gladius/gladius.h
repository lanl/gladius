/*
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include "core/core.h"
#include "core/session.h"
#include "core/env.h"
#include "ui/ui.h"

namespace gladius {

class Gladius {
private:
    //
    core::Args mArgs;
    //
    core::SessionFE &mCurrentSession;
    //
    core::Environment &mEnv;
    //
    ui::UI &mUI;
    //
    Gladius(void);
    //
    void
    mCoreComponentRegistration(void);

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
    //
    bool
    shutdown(void);

};

} // end gladius namespace
