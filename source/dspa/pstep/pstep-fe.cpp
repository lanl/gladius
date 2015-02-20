/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gladius-dspi.h"

#include <iostream>

using namespace gladius::dspi;

class PStep : public DomainSpecificPlugin {
public:
    //
    PStep(void) { ; }
    //
    ~PStep(void) { ; }
    //
    virtual void
    activate(void);
    //
    virtual void
    mainLoop(void);
    //
    virtual void
    destroy(void);
};

GLADIUS_PLUGIN(PStep, "pstep", "0.0.1");

/**
 *
 */
void
PStep::activate(void)
{
    std::cout << "activation" << std::endl;
}

/**
 *
 */
void
PStep::mainLoop(void)
{
}

/**
 *
 */
void
PStep::destroy(void)
{
}
