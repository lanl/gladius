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

#ifndef GLADIUS_UI_TERM_TERM_H_INCLUDED
#define GLADIUS_UI_TERM_TERM_H_INCLUDED

#include "core/core.h"
#include "ui/ui.h"
#include "tool-fe/tool-fe.h"

#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <functional>
#include <algorithm>

#include "histedit.h"

namespace gladius {
namespace ui {
namespace term {

class Terminal;

/**
 *
 */
struct EvalInputCmdCallBackArgs {
    Terminal *terminal;
    int argc;
    char **argv;

    EvalInputCmdCallBackArgs(
        Terminal *t,
        int argc,
        const char **argv
    ) : terminal(t)
      , argc(argc)
      , argv((char **)argv) { ; }
};

/**
 *
 */
class TermCommand {
private:
    // Command name
    std::string mCMD;
    // Short command usage.
    std::string mShortUsage;
    // Long command usage.
    std::string mLongUsage;
    // The call-back function that implements the command's functionality.
    std::function<void(const EvalInputCmdCallBackArgs &)> mCBFun;
public:
    TermCommand(void) { ; }

    TermCommand(
        std::string cmd,
        std::string shortUsage,
        std::string longUsage,
        std::function<void(const EvalInputCmdCallBackArgs &)> cmdCallBack
    ) : mCMD(cmd)
      , mShortUsage(shortUsage)
      , mLongUsage(longUsage)
      , mCBFun(cmdCallBack) { ; }

    std::string
    command(void) const { return mCMD; }

    std::string
    shortUsage(void) const { return mShortUsage; }

    std::string
    longUsage(void) const { return mLongUsage; }

    void
    exec(const EvalInputCmdCallBackArgs &args) const {
        mCBFun(args);
    }
};

/**
 *
 */
class TermCommands {
private:
    // Map containing command name, callback pairs.
    std::map<std::string , TermCommand> mNameTermMap;
public:
    /**
     *
     */
    TermCommands(std::initializer_list<TermCommand> tCMDs) {
        for (auto tm : tCMDs) {
            mNameTermMap[tm.command()] = tm;
        }
    }

    /**
     *
     */
    const TermCommand *
    getTermCMD(std::string name) const {
        auto tmi = mNameTermMap.find(name);
        // not found, so return nullptr;
        if (tmi == mNameTermMap.end()) {
            return nullptr;
        }
        else {
            return &tmi->second;
        }
    }

    /**
     *
     */
    std::vector<std::string>
    availableCommands(void) const {
    std::vector<std::string> v;
        for (auto nameCmd : mNameTermMap) {
            v.push_back(nameCmd.first);
        }
        std::sort(v.begin(), v.end());
        return v;
    }
};

class Terminal : public UI {
private:
    static constexpr int sHistSize = 100;
    EditLine *mEditLine = nullptr;
    Tokenizer *mTokenizer = nullptr;
    History *mHist = nullptr;
    HistEvent mHistEvent;
    toolfe::ToolFE *mToolFE = nullptr;

    /**
     *
     */
    Terminal(void);

    /**
     *
     */
    void
    setSignalHandlers(void);

    /**
     *
     */
    void
    evaluateInput(
        int ac,
        const char **argv,
        bool *continueREPL
    );

    /**
     *
     */
    void
    mEnterREPL(void);

public:
    /**
     *
     */
    ~Terminal(void);

    /**
     *
     */
    Terminal(const core::Args &args);

    /**
     *
     */
    EditLine *
    getEditLine(void) {
        return mEditLine;
    }

    /**
     *
     */
    History *
    getHistory(void) {
        return mHist;
    }

    /**
     *
     */
    HistEvent &
    getHistEvent(void) {
        return mHistEvent;
    }

    void
    interact(void);

    std::vector<std::string>
    termCmds(void) const;
private:
    static TermCommands sTermCommands;
};

} // end term namespace
} // end ui namespace
} // end gladius namespace

#endif