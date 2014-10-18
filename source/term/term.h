/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * Copyright (c) 1992,1993 The Regents of the University of California.
 *                         All rights reserved.
 * See tc1.c in the libedit distribution for more details.
 */

/**
 * Implements the (pseudo) terminal functionality for the tool front-end. The
 * heavy lifting is performed by editline (libedit).
 */

#ifndef GLADIUS_TERM_TERM_H_INCLUDED
#define GLADIUS_TERM_TERM_H_INCLUDED

#include "core/core-includes.h"

#include "histedit.h"

namespace gladius {
namespace term {

class Terminal {
private:
    static constexpr int histSize = 100;
    EditLine *editLine = nullptr;
    Tokenizer *tokenizer = nullptr;
    History *hist = nullptr;
    Terminal(void);

public:
    ~Terminal(void);

    Terminal(
        int argc,
        const char **argv
    );

};

} // end term namespace
} // end gladius namespace

#endif
