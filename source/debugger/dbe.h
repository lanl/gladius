/**
 * Copyright (c) 2014      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 *
 * University of Illinois/NCSA Open Source License
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 *
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
#include <bitset>
#include <string>
#include <vector>

namespace gladius {
namespace dbe {

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


class IOChannel;

class Driver : public lldb::SBBroadcaster {
public:
    Driver ();

    virtual
    ~Driver ();

    void
    MainLoop ();

    lldb::SBError
    ParseArgs (int argc, const char *argv[], FILE *out_fh, bool &do_exit);

    const char *
    GetFilename() const;

    const char *
    GetCrashLogFilename() const;

    const char *
    GetArchName() const;

    lldb::ScriptLanguage
    GetScriptLanguage() const;

    void
    WriteInitialCommands (bool before_file, lldb::SBStream &strm);

    bool
    GetDebugMode() const;


    class OptionData
    {
    public:
        OptionData ();
       ~OptionData ();

        void
        Clear();

        void
        AddInitialCommand (const char *command, bool before_file, bool is_file, lldb::SBError &error);

        //static OptionDefinition m_cmd_option_table[];

        std::vector<std::string> m_args;
        lldb::ScriptLanguage m_script_lang;
        std::string m_core_file;
        std::string m_crash_log;
        std::vector<std::pair<bool,std::string> > m_initial_commands;
        std::vector<std::pair<bool,std::string> > m_after_file_commands;
        bool m_debug_mode;
        bool m_source_quietly;
        bool m_print_version;
        bool m_print_python_path;
        bool m_print_help;
        bool m_wait_for;
        std::string m_process_name;
        lldb::pid_t m_process_pid;
        bool m_use_external_editor;  // FIXME: When we have set/show variables we can remove this from here.
        typedef std::set<char> OptionSet;
        OptionSet m_seen_options;
    };


    static lldb::SBError
    SetOptionValue (int option_idx,
                    const char *option_arg,
                    Driver::OptionData &data);


    lldb::SBDebugger &
    GetDebugger()
    {
        return m_debugger;
    }

    void
    ResizeWindow (unsigned short col);

private:
    lldb::SBDebugger m_debugger;
    OptionData m_option_data;

    void
    ResetOptionValues ();

    void
    ReadyForCommand ();
};

} // end dbe namespace
} // end gladius namespace

#endif
