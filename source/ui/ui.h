/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_UI_UI_H_INCLUDED
#define GLADIUS_UI_UI_H_INCLUDED

#include "core/core.h"

namespace gladius {
namespace ui {

/**
 * Base User Interface (UI) (virtual) class.
 */
class UI {
protected:
    core::Args mArgs;

public:
    /**
     *
     */
    UI(void);

    /**
     *
     */
    UI(const core::Args &args);

    /**
     *
     */
    virtual ~UI(void);

    /**
     * Top-level function that starts the UI interaction.
     */
    virtual void
    interact(void) = 0;
};

} // end ui namespace
} // end gladius namespace

#endif
