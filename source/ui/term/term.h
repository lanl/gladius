/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
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

#ifndef GLADIUS_UI_TERM_TERM_H_INCLUDED
#define GLADIUS_UI_TERM_TERM_H_INCLUDED

#include "core/core.h"
#include "core/args.h"
#include "core/session.h"
#include "ui/ui.h"

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <sstream>

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
    /// Command name
    std::string mCMD;
    /// Command name aliases in "v, v, v" format.
    std::string mCMDAliases;
    /// Short command usage.
    std::string mShortUsage;
    /// Long command usage.
    std::string mLongUsage;
    /// The call-back function that implements the command's functionality.
    std::function<bool (const EvalInputCmdCallBackArgs &)> mCBFun;
public:
    TermCommand(void) { ; }

    TermCommand(
        std::string cmd,
        std::string cmdAliases,
        std::string shortUsage,
        std::string longUsage,
        std::function<bool (const EvalInputCmdCallBackArgs &)> cmdCallBack
    ) : mCMD(cmd)
      , mCMDAliases(cmdAliases)
      , mShortUsage(shortUsage)
      , mLongUsage(longUsage)
      , mCBFun(cmdCallBack) { ; }

    std::string
    command(void) const { return mCMD; }

    std::string
    commandAliases(void) const { return mCMDAliases; }

    std::string
    shortUsage(void) const { return mShortUsage; }

    std::string
    longUsage(void) const { return mLongUsage; }

    bool
    exec(const EvalInputCmdCallBackArgs &args) const {
        return mCBFun(args);
    }
};

/**
 *
 */
class TermCommands {
private:
    // Map containing command name, callback pairs.
    std::map<std::string , TermCommand> mNameTermMap;
    /**
     * Returns a vector of command aliases from a string of comma-separated
     * values.
     */
    std::vector<std::string>
    getCommandAliases(std::string nameCSV) {
        std::vector<std::string> nameVec;
        std::istringstream buf(nameCSV);
        for (std::string tok; std::getline(buf, tok, ','); ) {
            nameVec.push_back(core::utils::trim(tok));
        }
        return nameVec;
    }
public:
    /**
     *
     */
    TermCommands(std::initializer_list<TermCommand> tCMDs) {
        // For each TermCommand
        for (auto tm : tCMDs) {
            // Bind the official name with the callback
            mNameTermMap[tm.command()] = tm;
            // And do the rest (aliases)
            for (auto cmdName : getCommandAliases(tm.commandAliases())) {
                mNameTermMap[cmdName] = tm;
            }
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
     * Returns a vector of available terminal commands.
     */
    std::vector<TermCommand>
    availableCommands(void) const {
        std::vector<TermCommand> tv;
        for (auto nameCmd : mNameTermMap) {
            tv.push_back(nameCmd.second);
        }
        return tv;
    }
};

class Terminal : public UI {
    //
    static constexpr int sHistSize = 100;
    //
    static const std::string sHistFileName;
    //
    core::SessionFE &mSession;
    //
    std::string mHistFile;
    //
    EditLine *mEditLine = nullptr;
    //
    Tokenizer *mTokenizer = nullptr;
    //
    History *mHist = nullptr;
    //
    HistEvent mHistEvent;
    //
    Terminal(void) : mSession(core::SessionFE::TheSession()) { ; }
    //
    ~Terminal(void);
    //
    void
    installSignalHandlers(void);
    //
    void
    evaluateInput(
        int ac,
        const char **argv,
        bool &continueREPL
    );
    //
    void
    mEnterREPL(void);
    //
    void
    mLoadHistory(void);
    //
    void
    mSaveHistory(void);
    //
    bool
    mHistRecallRequest(
        const std::string &input,
        std::string &histStringIfValid
    );
public:
    //
    static Terminal &
    TheTerminal(void);

    /**
     * Disable copy constructor.
     */
    Terminal(const Terminal &that) = delete;

    /**
     * Just return the singleton.
     */
    Terminal &
    operator=(const Terminal &other) {
        GLADIUS_UNUSED(other);
        return Terminal::TheTerminal();
    }

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

    /**
     *
     */
    const TermCommands &
    getTermCommands (void) const {
        return sTermCommands;
    }
    //
    void
    init(const core::Args &args);
    //
    void
    interact(void);
    //
    bool
    quit(void);
    //
    std::vector< std::pair<std::string, std::string> >
    cmdPairs(void) const;
private:
    static TermCommands sTermCommands;
};

} // end term namespace
} // end ui namespace
} // end gladius namespace

#endif
