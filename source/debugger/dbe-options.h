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

#ifndef GLADIUS_DEBUGGER_DBE_OPTIONS_H_INCLUDED
#define GLADIUS_DEBUGGER_DBE_OPTIONS_H_INCLUDED

#include "dbe.h"
#include "core/core-includes.h"

#include "lldb/API/SBCommandInterpreter.h"

#include <cstdint>
#include <vector>
#include <bitset>

#include <getopt.h>

namespace gladius {
namespace dbe {

#define LLDB_3_TO_5 LLDB_OPT_SET_3|LLDB_OPT_SET_4|LLDB_OPT_SET_5
#define LLDB_4_TO_5 LLDB_OPT_SET_4|LLDB_OPT_SET_5

typedef struct {
    // Used to mark options that can be used together.
    // If (1 << n & usage_mask) != 0 then this option belongs to option set n.
    uint32_t usage_mask;
    // This option is required (in the current usage level)
    bool required;
    // Full name for this option.
    const char *long_option;
    // Single character for this option.
    int short_option;
    // no_argument, required_argument or optional_argument
    int option_has_arg;
    // Cookie the option class can use to do define the argument completion.
    uint32_t completion_type;
    // Type of argument this option takes
    lldb::CommandArgumentType argument_type;
    // Full text explaining what this options does and what (if any) argument to
    // pass it.
    const char *usage_text;
} OptionDefinition;

static OptionDefinition gOptions[] = {
    { LLDB_OPT_SET_1,    true , "help"           , 'h', no_argument      , 0,  lldb::eArgTypeNone,
        "Prints out the usage information for the LLDB debugger." },
    { LLDB_OPT_SET_2,    true , "version"        , 'v', no_argument      , 0,  lldb::eArgTypeNone,
        "Prints out the current version number of the LLDB debugger." },
    { LLDB_OPT_SET_3,    true , "arch"           , 'a', required_argument, 0,  lldb::eArgTypeArchitecture,
        "Tells the debugger to use the specified architecture when starting and running the program.  <architecture> must "
        "be one of the architectures for which the program was compiled." },
    { LLDB_OPT_SET_3,    true , "file"           , 'f', required_argument, 0,  lldb::eArgTypeFilename,
        "Tells the debugger to use the file <filename> as the program to be debugged." },
    { LLDB_OPT_SET_3,    false, "core"           , 'c', required_argument, 0,  lldb::eArgTypeFilename,
        "Tells the debugger to use the fullpath to <path> as the core file." },
    { LLDB_OPT_SET_5,    true , "attach-pid"     , 'p', required_argument, 0,  lldb::eArgTypePid,
        "Tells the debugger to attach to a process with the given pid." },
    { LLDB_OPT_SET_4,    true , "attach-name"    , 'n', required_argument, 0,  lldb::eArgTypeProcessName,
        "Tells the debugger to attach to a process with the given name." },
    { LLDB_OPT_SET_4,    true , "wait-for"       , 'w', no_argument      , 0,  lldb::eArgTypeNone,
        "Tells the debugger to wait for a process with the given pid or name to launch before attaching." },
    { LLDB_3_TO_5,       false, "source"         , 's', required_argument, 0,  lldb::eArgTypeFilename,
        "Tells the debugger to read in and execute the lldb commands in the given file, after any file provided on the command line has been loaded." },
    { LLDB_3_TO_5,       false, "one-line"         , 'o', required_argument, 0,  lldb::eArgTypeNone,
        "Tells the debugger to execute this one-line lldb command after any file provided on the command line has been loaded." },
    { LLDB_3_TO_5,       false, "source-before-file"         , 'S', required_argument, 0,  lldb::eArgTypeFilename,
        "Tells the debugger to read in and execute the lldb commands in the given file, before any file provided on the command line has been loaded." },
    { LLDB_3_TO_5,       false, "one-line-before-file"         , 'O', required_argument, 0,  lldb::eArgTypeNone,
        "Tells the debugger to execute this one-line lldb command before any file provided on the command line has been loaded." },
    { LLDB_3_TO_5,       false, "source-quietly"          , 'Q', no_argument      , 0,  lldb::eArgTypeNone,
        "Tells the debugger suppress output from commands provided in the -s, -S, -O and -o commands." },
    { LLDB_3_TO_5,       false, "editor"         , 'e', no_argument      , 0,  lldb::eArgTypeNone,
        "Tells the debugger to open source files using the host's \"external editor\" mechanism." },
    { LLDB_3_TO_5,       false, "no-lldbinit"    , 'x', no_argument      , 0,  lldb::eArgTypeNone,
        "Do not automatically parse any '.lldbinit' files." },
    { LLDB_3_TO_5,       false, "no-use-colors"  , 'X', no_argument      , 0,  lldb::eArgTypeNone,
        "Do not use colors." },
    { LLDB_OPT_SET_6,    true , "python-path"    , 'P', no_argument      , 0,  lldb::eArgTypeNone,
        "Prints out the path to the lldb.py file for this version of lldb." },
    { LLDB_3_TO_5,       false, "script-language", 'l', required_argument, 0,  lldb::eArgTypeScriptLang,
        "Tells the debugger to use the specified scripting language for user-defined scripts, rather than the default.  "
        "Valid scripting languages that can be specified include Python, Perl, Ruby and Tcl.  Currently only the Python "
        "extensions have been implemented." },
    { LLDB_3_TO_5,       false, "debug"          , 'd', no_argument      , 0,  lldb::eArgTypeNone,
        "Tells the debugger to print out extra information for debugging itself." },
    { 0,                 false, NULL             , 0  , 0                , 0,  lldb::eArgTypeNone,         NULL }
};

/**
 *
 */
static void
BuildGetOptTable(
    OptionDefinition *expanded_option_table,
    std::vector<struct option> &getopt_table,
    uint32_t num_options
) {
    if (num_options == 0) return;
    uint32_t i;
    uint32_t j;
    std::bitset<256> option_seen;
    getopt_table.resize(num_options + 1);
    for (i = 0, j = 0; i < num_options; ++i) {
        char short_opt = expanded_option_table[i].short_option;
        if (option_seen.test(short_opt) == false) {
            getopt_table[j].name    = expanded_option_table[i].long_option;
            getopt_table[j].has_arg = expanded_option_table[i].option_has_arg;
            getopt_table[j].flag    = NULL;
            getopt_table[j].val     = expanded_option_table[i].short_option;
            option_seen.set(short_opt);
            ++j;
        }
    }
    getopt_table[j].name    = NULL;
    getopt_table[j].has_arg = 0;
    getopt_table[j].flag    = NULL;
    getopt_table[j].val     = 0;
}

} // end dbe namespace
} // end gladius namespace

#endif
