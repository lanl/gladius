/*
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#pragma once

#include <iostream>
#include <sstream>

namespace gladius {
namespace core {

class console {
    /**
     *
     */
    class OutS : public std::ostream {
        /**
         *
         */
        class StreamBuf : public std::stringbuf {
            //
            std::ostream &mOutput;
        public:
            /**
             *
             */
            StreamBuf(std::ostream &str) : mOutput(str) { ; }

            /**
             *
             */
            virtual int
            sync(void) {
                mOutput << "[blah]" << str();
                str("");
                mOutput.flush();
                return 0;
            }
        };

        StreamBuf buffer;
    public:
        /**
         *
         */
        OutS(
            void
        ) : std::ostream(&buffer)
          , buffer(std::cout) { ; }
    };
public:
    //
    OutS outs;
};

} // end core namespace
} // end gladius namespace
