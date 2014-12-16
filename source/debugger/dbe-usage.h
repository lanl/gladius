/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
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

#ifndef GLADIUS_DEBUGGER_DBE_USAGE_H_INCLUDED
#define GLADIUS_DEBUGGER_DBE_USAGE_H_INCLUDED

#include "dbe.h"
#include "core/core-includes.h"

#include "lldb/API/SBCommandInterpreter.h"

#include <cstdint>
#include <vector>
#include <bitset>
#include <cstring>

#include <getopt.h>

namespace gladius {
namespace dbe {

static const uint32_t last_option_set_with_args = 2;

// This function takes INDENT, which tells how many spaces to output at the front
// of each line; TEXT, which is the text that is to be output. It outputs the
// text, on multiple lines if necessary, to RESULT, with INDENT spaces at the
// front of each line.  It breaks lines on spaces, tabs or newlines, shortening
// the line if necessary to not break in the middle of a word. It assumes that
// each output line should contain a maximum of OUTPUT_MAX_COLUMNS characters.
static void
OutputFormattedUsageText(
    FILE *out,
    int indent,
    const char *text,
    int output_max_columns
) {
    int len = strlen(text);
    std::string text_string(text);

    // Force indentation to be reasonable.
    if (indent >= output_max_columns) {
        indent = 0;
    }
    // Will it all fit on one line?
    if (len + indent < output_max_columns) {
        // Output as a single line
        fprintf(out, "%*s%s\n", indent, "", text);
    }
    else {
        // We need to break it up into multiple lines.
        int text_width = output_max_columns - indent - 1;
        int start = 0;
        int end = start;
        int final_end = len;
        int sub_len;

        while (end < final_end) {
            // Dont start the 'text' on a space, since we're already outputting
            // the indentation.
            while ((start < final_end) && (text[start] == ' ')) {
                start++;
            }
            end = start + text_width;
            if (end > final_end) {
                end = final_end;
            }
            else {
                // If we're not at the end of the text, make sure we break the
                // line on white space.
                while (end > start && text[end] != ' ' &&
                       text[end] != '\t' && text[end] != '\n') {
                    end--;
                }
            }
            sub_len = end - start;
            std::string substring = text_string.substr(start, sub_len);
            fprintf(out, "%*s%s\n", indent, "", substring.c_str());
            start = end + 1;
        }
    }
}

static void
ShowUsage(
    FILE *out,
    OptionDefinition *optionTable,
    LLDBDriver::OptionData data
) {
    using namespace lldb;

    GLADIUS_UNUSED(data);
    uint32_t screen_width = 80;
    uint32_t indent_level = 0;
    const char *name = PACKAGE_NAME;
    fprintf(out, "\nUsage:\n\n");
    indent_level += 2;


    // First, show each usage level set of options, e.g.
    // <cmd> [options-for-level-0]
    // <cmd> [options-for-level-1]
    // etc.
    uint32_t num_options;
    uint32_t num_option_sets = 0;
    for (num_options = 0;
         optionTable[num_options].longOpt != NULL;
         ++num_options) {
        uint32_t this_usageMask = optionTable[num_options].usageMask;
        if (this_usageMask == LLDB_OPT_SET_ALL) {
            if (num_option_sets == 0) {
                num_option_sets = 1;
            }
        }
        else {
            for (uint32_t j = 0; j < LLDB_MAX_NUM_OPTION_SETS; j++) {
                if (this_usageMask & 1 << j) {
                    if (num_option_sets <= j) {
                        num_option_sets = j + 1;
                    }
                }
            }
        }
    }
    for (uint32_t opt_set = 0; opt_set < num_option_sets; opt_set++) {
        uint32_t opt_set_mask;
        opt_set_mask = 1 << opt_set;
        if (opt_set > 0) {
            fprintf(out, "\n");
        }
        fprintf(out, "%*s%s", indent_level, "", name);
        bool isHelpLine = false;

        for (uint32_t i = 0; i < num_options; ++i) {
            if (optionTable[i].usageMask & opt_set_mask) {
                lldb::CommandArgumentType argType = optionTable[i].argType;
                const char *argName =
                    SBCommandInterpreter::GetArgumentTypeAsCString(argType);
                // This is a bit of a hack, but there's no way to say certain
                // options don't have arguments yet...  so we do it by hand
                // here.
                if (optionTable[i].shortOpt == 'h') {
                    isHelpLine = true;
                }
                if (optionTable[i].required) {
                    if (optionTable[i].option_has_arg == required_argument) {
                        fprintf(out, " -%c <%s>",
                                optionTable[i].shortOpt, argName);
                    }
                    else if (optionTable[i].option_has_arg == optional_argument) {
                        fprintf(out, " -%c [<%s>]",
                                optionTable[i].shortOpt, argName);
                    }
                    else {
                        fprintf(out, " -%c", optionTable[i].shortOpt);
                    }
                }
                else {
                    if (optionTable[i].option_has_arg == required_argument) {
                        fprintf(out, " [-%c <%s>]",
                                optionTable[i].shortOpt, argName);
                    }
                    else if (optionTable[i].option_has_arg == optional_argument) {
                        fprintf(out, " [-%c [<%s>]]",
                                optionTable[i].shortOpt, argName);
                    }
                    else {
                        fprintf(out, " [-%c]", optionTable[i].shortOpt);
                    }
                }
            }
        }
        if (!isHelpLine && (opt_set <= last_option_set_with_args)) {
            fprintf(out, " [[--] <PROGRAM-ARG-1> [<PROGRAM_ARG-2> ...]]");
        }
    }

    fprintf(out, "\n\n");

    // Now print out all the detailed information about the various options:
    // long form, short form and help text:
    //   -- long_name <argument>
    //   - short <argument>
    //   help text

    // This variable is used to keep track of which options' info we've printed
    // out, because some options can be in
    // more than one usage level, but we only want to print the long form of its
    // information once.
    LLDBDriver::OptionData::OptionSet options_seen;
    LLDBDriver::OptionData::OptionSet::iterator pos;

    indent_level += 5;
    for (uint32_t i = 0; i < num_options; ++i) {
        // Only print this option if we haven't already seen it.
        pos = options_seen.find(optionTable[i].shortOpt);
        if (pos == options_seen.end()) {
            CommandArgumentType argType = optionTable[i].argType;
            const char *argName =
                SBCommandInterpreter::GetArgumentTypeAsCString(argType);
            options_seen.insert(optionTable[i].shortOpt);
            fprintf(out, "%*s-%c ", indent_level, "",
                    optionTable[i].shortOpt);
            if (argType != eArgTypeNone) {
                fprintf(out, "<%s>", argName);
            }
            fprintf(out, "\n");
            fprintf(out, "%*s--%s ", indent_level, "",
                    optionTable[i].longOpt);
            if (argType != eArgTypeNone) {
                fprintf(out, "<%s>", argName);
            }
            fprintf(out, "\n");
            indent_level += 5;
            OutputFormattedUsageText(out, indent_level,
                                     optionTable[i].usage_text, screen_width);
            indent_level -= 5;
            fprintf(out, "\n");
        }
    }
    indent_level -= 5;
    fprintf(out, "\n%*sNotes:\n",
            indent_level, "");
    indent_level += 5;

    fprintf(out,
            "\n%*sMultiple \"-s\" and \"-o\" options can be provided.  "
            "They will be processed from left to right in order, "
            "\n%*swith the source files and commands interleaved.  "
            "The same is true of the \"-S\" and \"-O\" options."
            "\n%*sThe before file and after file sets can intermixed freely, "
            "the command parser will sort them out."
            "\n%*sThe order of the file specifiers (\"-c\", \"-f\", etc.) is "
            "not significant in this regard.\n\n",
            indent_level, "",
            indent_level, "",
            indent_level, "",
            indent_level, ""
    );
    fprintf(out,
            "\n%*sIf you don't provide -f then the first argument will be the "
            "file to be debugged \n%*swhich means that '%s -- <filename> [<ARG1> "
            "[<ARG2>]]' also works."
            "\n%*sBut remember to end the options with \"--\" if any of your "
            "arguments have a \"-\" in them.\n\n",
            indent_level, "",
            indent_level, "",
            name,
            indent_level, ""
    );
}

} // end dbe namespace
} // end gladius namespace

#endif
