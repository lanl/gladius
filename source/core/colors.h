/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef GLADIUS_CORE_COLORS_H_INCLUDED
#define GLADIUS_CORE_COLORS_H_INCLUDED

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
        WHITE = 0,
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
    ansiBeginColor(Color c) {
        if (!mColorize || c > ColorCodeTab.size()) return "";
        return ColorCodeTab[c].second;
    }
    /**
     *
     */
    std::string
    ansiEndColor(void) {
        if (!mColorize) return "";
        return "\033[0m";
    }
private:
    //
    static std::vector< std::pair<Color, std::string> > ColorCodeTab;
};

} // end core namespace
} // end gladius namespace

#endif
