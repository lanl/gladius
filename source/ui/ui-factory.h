/**
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_UI_UI_FACTORY_H_INCLUDED
#define GLADIUS_UI_UI_FACTORY_H_INCLUDED

#include "ui.h"
#include "term/term.h"

#include "core/core.h"

namespace gladius {
namespace ui {

/**
 * UI factory class.
 */
class UIFactory {
public:
    /**
     * UI Types.
     */
    enum UIType {
        UI_TERM,
        UI_GUI
    };

    /**
     * Factory function that takes a UI type and args and produces a UI based on
     * the provided input.
     */
    static UI &
    getUI(
        const core::Args &args,
        UIType uiType
    ) {
        GLADIUS_UNUSED(args);
        switch (uiType) {
            case UI_TERM: return term::Terminal::TheTerminal();
            default: GLADIUS_THROW_INVLD_ARG();
        }
    }
};

} // end ui namespace
} // end gladius namespace

#endif
