/*
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <iostream>

namespace gladius {
namespace core {

class console {

    struct Out {
        template<class TYPE>
        Out &operator<<(const TYPE &tval) {
            std::cout << tval << std::endl;
            return *this;
        }
    };

    struct Decorator {

    }
public:
    Out out;
};

} // end core namespace
} // end gladius namespace
