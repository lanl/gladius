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
#include <string>

namespace gladius {
namespace core {
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class console {
     ///////////////////////////////////////////////////////////////////////////
     ///////////////////////////////////////////////////////////////////////////
    class OutS : public std::ostream {
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
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
        }; // class StreamBuf
        //
        StreamBuf mBuffer;
    public:
        /**
         *
         */
        OutS(
            void
        ) : std::ostream(&mBuffer)
          , mBuffer(std::cout) { ; }

        /**
         *
         */
        OutS(
            std::ostream &os
        ) : std::ostream(&mBuffer)
          , mBuffer(os) { ; }
    }; // class OutS
    //
    std::string mMsgPrefix;
    //
    OutS *mOuts = nullptr;
    //
    OutS *mErrs = nullptr;
public:
    /**
     *
     */
    console(
        std::string msgPrefix = ""
    ) : mMsgPrefix(msgPrefix)
      , mOuts(new OutS(std::cout))
      , mErrs(new OutS(std::cerr)) { ; }

    /**
     *
     */
    OutS &
    outs(void) { return *mOuts; }

    /**
     *
     */
    OutS &
    errs(void) { return *mErrs; }
}; // class console

} // end core namespace
} // end gladius namespace
