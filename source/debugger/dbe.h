/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * University of Illinois/NCSA Open Source License
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 * For more details, please see LICENSE.TXT in the LLDB distirubtion.
 */

/**
 * Implements the Debugger BackEnd interface.
 */

#ifndef GLADIUS_DEBUGGER_DBE_H_INCLUDED
#define GLADIUS_DEBUGGER_DBE_H_INCLUDED

#include "lldb/Utility/PseudoTerminal.h"
#include "lldb/API/SBDefines.h"
#include "lldb/API/SBBroadcaster.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBError.h"

#include <set>
#include <string>
#include <vector>

namespace gladius {
namespace dbe {

/**
 * GladiusDBE
 */
class GladiusDBE {
private:
    int argC;
    const char **argV;
    const char **envP;
    GladiusDBE(void);

public:
    ~GladiusDBE(void);

    GladiusDBE(
        int argc,
        const char **argv,
        const char **envp
    );

    void
    mainLoop(void);
};

/**
 * LLDBDriver
 */
class LLDBDriver : public lldb::SBBroadcaster {
public:
    LLDBDriver(void);

    virtual
    ~LLDBDriver(void);

    void
    mainLoop(void);

    lldb::SBError
    ParseArgs(
        int argc,
        const char **argv,
        FILE *out_fh,
        bool &do_exit
    );

    const char *
    GetFilename(void) const;

    const char *
    GetCrashLogFilename(void) const;

    const char *
    GetArchName(void) const;

    lldb::ScriptLanguage
    GetScriptLanguage(void) const;

    void
    WriteInitialCommands(
        bool before_file,
        lldb::SBStream &strm
    );

    bool
    GetDebugMode(void) const;

    /**
     * OptionData
     */
    class OptionData {
    public:
        std::vector<std::string> mArgs;
        lldb::ScriptLanguage mScriptLang;
        std::string mCoreFile;
        std::string mCrashLog;
        std::vector<std::pair<bool,std::string> > mInitialCommands;
        std::vector<std::pair<bool,std::string> > mAfterFileCommands;
        bool mDebugMode;
        bool mSourceQuietly;
        bool mPrintVersion;
        bool mPrintPythonPath;
        bool mPrintHelp;
        bool mWaitFor;
        std::string mProcessName;
        lldb::pid_t mProcessPID;
        bool mUseExternalEditor;
        typedef std::set<char> OptionSet;
        OptionSet mSeenOptions;

        OptionData(void);

       ~OptionData(void);

        void
        clear(void);

        void
        AddInitialCommand(
            const char *command,
            bool before_file,
            bool is_file,
            lldb::SBError &error
        );
    };

    static lldb::SBError
    SetOptionValue(
        int option_idx,
        const char *option_arg,
        LLDBDriver::OptionData &data
    );

    lldb::SBDebugger &
    getDebugger(void) {
        return mDebugger;
    }

    void
    resizeWindow(unsigned short col);

private:
    lldb::SBDebugger mDebugger;
    OptionData mOptionData;

    void
    ResetOptionValues(void);

    void
    ReadyForCommand(void);
};

} // end dbe namespace
} // end gladius namespace

#endif
