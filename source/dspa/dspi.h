/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * The Domain-Specific Plugin Architecture. 
 */

#ifndef GLADIUS_DSPA_DSPA_H_INCLUDED
#define GLADIUS_DSPA_DSPA_H_INCLUDED

namespace gladius {
namespace dspa {

/**
 * Update when breaking plugin ABI.
 */
#define GLADIUS_DSPI_ABI 0

/**
 * The Domain-Specific Plugin Interface (DSPI) that plugins must adhere to.
 */
class DSPI {
public:
    /**
     *
     */
    DSPI(void) { ; }

    /**
     *
     */
    virtual ~DSPI(void) { ; }
    //
    virtual void
    activate(void) = 0;
    //
    virtual void
    mainLoop(void) = 0;
    //
    virtual void
    destroy(void) = 0;
};

/**
 * Domain-Specific Plugin manager.
 */
class DSPA {
public:
    //
    DSPA(void);
    //
    ~DSPA(void);
};


} // end dspa namespace
} // end gladius namespace

#endif
