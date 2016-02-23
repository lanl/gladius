/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "core/gladius-exception.h"

using namespace std;
using namespace gladius::core;

/**
 *
 */
GladiusException::GladiusException(
    string fileName,
    int lineNo,
    const string &errMsg,
    bool where
) {
    if (where) {
        string lineNoStr = to_string(lineNo);
        whatString = "\n-->[" + fileName + ", line " + lineNoStr + "] "
                   + errMsg;
    }
    else whatString = errMsg;
}

/**
 *
 */
const char *
GladiusException::what(
    void
) const throw() {
    return this->whatString.c_str();
}
