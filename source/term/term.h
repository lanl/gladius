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
 * Implements the (pseudo) terminal functionality for the tool front-end UI. The
 * heavy lifting for the CLI is performed by editline (libedit).
 */

/**
 * TODO
 * History - See source/Host/common/Editline.cpp in LLDB for an example.
 * Auto complete
 */

#ifndef GLADIUS_TERM_TERM_H_INCLUDED
#define GLADIUS_TERM_TERM_H_INCLUDED

#include "ui/ui.h"

#include <string>
#include <map>

#include "histedit.h"

namespace gladius {
namespace term {

class Terminal : public ui::UI {
private:
    static constexpr int sHistSize = 100;
    EditLine *mEditLine = nullptr;
    Tokenizer *mTokenizer = nullptr;
    History *mHist = nullptr;
    HistEvent mHistEvent;

    Terminal(void);

    void
    setSignalHandlers(void);

    void
    evaluateInput(
        int ac,
        const char **argv,
        bool *continueREPL
    );

    void
    mEnterREPL(void);


public:
    ~Terminal(void);

    Terminal(
        int argc,
        const char **argv
    );

    EditLine *
    getEditLine(void) {
        return mEditLine;
    }

    History *
    getHistory(void) {
        return mHist;
    }

    HistEvent &
    getHistEvent(void) {
        return mHistEvent;
    }

    struct EvalInputCmdCallBackArgs {
        Terminal *terminal;
        int argc;
        char **argv;

        EvalInputCmdCallBackArgs(
            Terminal *t,
            int argc,
            const char **argv)
            : terminal(t)
            , argc(argc)
            , argv((char **)argv
        ) { ; }
    };

    void
    interact(void);

private:
    static std::map<std::string,
                    void (*)(const EvalInputCmdCallBackArgs &)> sEvalCMDMap;
};

} // end term namespace
} // end gladius namespace

#endif
