/**
 * Copyright (c) 2014-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <string>
#include <exception>

namespace gladius {
namespace core {

#define GLADIUS_WHERE __FILE__, __LINE__

class GladiusException : public std::exception {
private:
    std::string whatString;
    GladiusException(void);

public:
    ~GladiusException(void) throw() { }

    GladiusException(std::string fileName,
                     int lineNo,
                     const std::string &errMsg,
                     bool where = true);

    virtual const char *what(void) const throw();

    void fooBar(void);
};

} // end core namespace
} // end gladius namespace
