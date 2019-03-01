/*
 * Copyright (c) 2014-2015 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/colors.h"
#include "core/utils.h"
#include "core/env.h"

using namespace gladius::core;

std::vector< std::pair<colors::Color, std::string> > colors::ColorCodeTab = {
    std::make_pair(colors::NONE,   ""),
    std::make_pair(colors::WHITE,  "\033[0;37m"),
    std::make_pair(colors::RED,    "\033[0;31m"),
    std::make_pair(colors::GREEN,  "\033[0;32m"),
    std::make_pair(colors::YELLOW, "\033[0;33m"),
    std::make_pair(colors::MAGENTA,"\033[0;35m"),
    std::make_pair(colors::DGRAY,  "\033[0;90m")
};

/**
 * Returns reference to the color instance.
 */
colors &
colors::color(void) {
    static bool initialized = false;
    static colors singleton;
    if (!initialized) {
        initialized = true;
        singleton.mColorize = !utils::envVarSet(GLADIUS_ENV_NO_TERM_COLORS_NAME);
    }
    return singleton;
}

/**
 * Just return the singleton.
 */
colors &
colors::operator=(const colors &other) {
    GLADIUS_UNUSED(other);
    return colors::color();
}
