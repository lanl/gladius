/*
 * Copyright (c) 2014-2016 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <string>
#include <vector>
#include <utility>

namespace gladius {
namespace core {

class colors {
private:
    // Are we colorizing the output?
    bool mColorize = false;
    //
    colors(void) { ; }
    //
    ~colors(void) { ; }
public:
    /**
     * Supported colors. Order matters here. If updated, also update
     * ColorCodeTab.
     */
    enum Color {
        NONE = 0,
        WHITE,
        RED,
        GREEN,
        YELLOW,
        MAGENTA,
        DGRAY,
    };

    /**
     * Returns reference to the color instance.
     */
    static colors &
    color(void);

    /**
     * Disable copy constructor.
     */
    colors(const colors &that) = delete;

    //
    colors &
    operator=(const colors &other);

    /**
     *
     */
    std::string
    ansiBeginColor(
        Color c
    ) const {
        if (!mColorize || c > ColorCodeTab.size()) return "";
        return ColorCodeTab[c].second;
    }

    /**
     *
     */
    std::string
    ansiEndColor(
        void
    ) const {
        if (!mColorize) return "";
        return "\033[0m";
    }

    /**
     *
     */
    void
    colorize(bool colors) {
        mColorize = colors;
    }
private:
    //
    static std::vector< std::pair<Color, std::string> > ColorCodeTab;
};

} // end core namespace
} // end gladius namespace
