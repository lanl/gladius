/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

/**
 * Implements the Automatic Process Acquisition (APA) interface.
 */

#ifndef GLADIUS_APA_APA_H_INCLUDED
#define GLADIUS_APA_APA_H_INCLUDED

#include "core/core-includes.h"

namespace gladius {
namespace apa {

class AutomaticProcessAcquisition {
private:
    AutomaticProcessAcquisition(void);

public:
    ~AutomaticProcessAcquisition(void);
};

} // end term namespace
} // end gladius namespace

#endif
