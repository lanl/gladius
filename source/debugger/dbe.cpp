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

#include "dbe.h"
#include "dbe-options.h"
#include "dbe-usage.h"
#include "core/core-includes.h"

#include <string>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <string>
#include <thread>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <inttypes.h>

#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "lldb/API/SBCommunication.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBEvent.h"
#include "lldb/API/SBHostOS.h"
#include "lldb/API/SBListener.h"
#include "lldb/API/SBStream.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBProcess.h"

using namespace lldb;
using namespace gladius::dbe;

namespace { ////////////////////////////////////////////////////////////////////

bool oldStdinTermIOSIsValid = false;
struct termios oldStdinTermIOS;
char *debuggerName =  const_cast<char *>("");
LLDBDriver *gDriver = NULL;

/**
 * Signal handler for SIGWINCH (window size change).
 */
void
sigwinchHandler(int signo)
{
    GLADIUS_UNUSED(signo);
    struct winsize window_size;
    if (isatty (STDIN_FILENO)
        && 0 == ioctl(STDIN_FILENO, TIOCGWINSZ, &window_size)) {
        if ((window_size.ws_col > 0) && NULL != gDriver) {
            gDriver->resizeWindow (window_size.ws_col);
        }
    }
}

/**
 * Signal handler for SIGINT (Ctrl-C).
 */
void
sigintHandler(int signo)
{
    static bool gInterruptSent = false;
    if (gDriver) {
        if (!gInterruptSent) {
            gInterruptSent = true;
            gDriver->getDebugger().DispatchInputInterrupt();
            gInterruptSent = false;
            return;
        }
    }
    exit(signo);
}

/**
 * Signal handler for SIGTSTP (Ctrl-Z).
 */
void
sigtstpHandler (int signo)
{
    gDriver->getDebugger().SaveInputTerminalState();
    signal(signo, SIG_DFL);
    kill(getpid(), signo);
    signal(signo, sigtstpHandler);
}

/**
 * Signal handler for SIGCONT (continue (restart) a process previously paused by
 * the SIGSTOP or SIGTSTP).
 */
void
sigcontHandler (int signo)
{
    gDriver->getDebugger().RestoreInputTerminalState();
    signal(signo, SIG_DFL);
    kill(getpid(), signo);
    signal(signo, sigcontHandler);
}

/**
 * Sets all signal handlers.
 */
void
setSignalHandlers(void)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGWINCH, sigwinchHandler);
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigtstpHandler);
    signal(SIGCONT, sigcontHandler);
}

/**
 * In the LLDBDriver::mainLoop, we change the terminal settings.  This function
 * is added as an atexit handler to make sure we clean them up.
 */
void
resetStdinTermIOS(void)
{
    if (oldStdinTermIOSIsValid) {
        oldStdinTermIOSIsValid = false;
        tcsetattr(STDIN_FILENO, TCSANOW, &oldStdinTermIOS);
    }
}
} // end private utility routine namespace /////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LLDBDriver
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
LLDBDriver::LLDBDriver(void) :
    SBBroadcaster("LLDBDriver"),
    mDebugger(SBDebugger::Create(false)),
    mOptionData()
{
    // We want to be able to handle CTRL+D in the terminal to have it terminate
    // certain input
    mDebugger.SetCloseInputOnEOF(false);
    debuggerName = (char *)mDebugger.GetInstanceName();
    if (!debuggerName) debuggerName = (char *)"";
    gDriver = this;
}

/**
 *
 */
LLDBDriver::~LLDBDriver(void)
{
    gDriver = NULL;
    debuggerName = NULL;
}

/**
 *
 */
void
LLDBDriver::resizeWindow(unsigned short col)
{
    getDebugger().SetTerminalWidth(col);
}

/**
 *
 */
bool
LLDBDriver::GetDebugMode(void) const
{
    return mOptionData.mDebugMode;
}

/**
 *
 */
void
LLDBDriver::ResetOptionValues(void)
{
    mOptionData.clear();
}

/**
 *
 */
const char *
LLDBDriver::GetFilename(void) const
{
    if (mOptionData.mArgs.empty()) return NULL;
    return mOptionData.mArgs.front().c_str();
}

/**
 *
 */
const char *
LLDBDriver::GetCrashLogFilename(void) const
{
    if (mOptionData.mCrashLog.empty()) return NULL;
    return mOptionData.mCrashLog.c_str();
}

/**
 *
 */
lldb::ScriptLanguage
LLDBDriver::GetScriptLanguage(void) const
{
    return mOptionData.mScriptLang;
}

/**
 * Check the arguments that were passed to this program to make sure they are
 * valid and to get their argument values (if any).  Return a boolean value
 * indicating whether or not to start up the full debugger (i.e. the Command
 * Interpreter) or not.  Return FALSE if the arguments were invalid OR if the
 * user only wanted help or version information.
 */
SBError
LLDBDriver::ParseArgs(
    int argc,
    const char **argv,
    FILE *outFH,
    bool &exiting
) {
    ResetOptionValues ();
    SBCommandReturnObject result;
    SBError error;
    std::string optionString;
    struct option *longOptions = NULL;
    std::vector<struct option> longOptionsVec;
    uint32_t numOptions;

    for (numOptions = 0; gOptions[numOptions].longOpt; ++numOptions) {
        /* Do Nothing. */;
    }
    if (0 == numOptions) {
        if (argc > 1) {
            error.SetErrorStringWithFormat ("invalid number of options");
        }
        return error;
    }

    BuildGetOptTable(gOptions, longOptionsVec, numOptions);

    if (longOptionsVec.empty()) longOptions = NULL;
    else longOptions = &longOptionsVec.front();

    if (!longOptions) {
        error.SetErrorStringWithFormat ("invalid long options");
        return error;
    }

    // Build the optionString argument for call to getopt_long_only.

    for (int i = 0; longOptions[i].name != NULL; ++i) {
        if (longOptions[i].flag == NULL) {
            optionString.push_back((char)longOptions[i].val);
            switch (longOptions[i].has_arg) {
                default:
                case no_argument:
                    break;
                case required_argument:
                    optionString.push_back (':');
                    break;
                case optional_argument:
                    optionString.append ("::");
                    break;
            }
        }
    }
    // This is kind of a pain, but since we make the debugger in the
    // LLDBDriver's constructor, we can't know at that point whether we should
    // read in init files yet.  So we don't read them in in the LLDBDriver
    // constructor, then set the flags back to "read them in" here, and then if
    // we see the "-n" flag, we'll turn it off again.  Finally we have to read
    // them in by hand later in the main loop.

    mDebugger.SkipLLDBInitFiles (false);
    mDebugger.SkipAppInitFiles (false);

    // Prepare for & make calls to getopt_long_only.
#if __GLIBC__
    optind = 0;
#else
    optreset = 1;
    optind = 1;
#endif
    int val;
    while (1) {
        int longOptions_index = -1;
        val = getopt_long_only(argc, const_cast<char **>(argv),
                               optionString.c_str(),
                               longOptions, &longOptions_index);
        if (val == -1) break;
        else if (val == '?') {
            mOptionData.mPrintHelp = true;
            error.SetErrorStringWithFormat ("unknown or ambiguous option");
            break;
        }
        else if (val == 0) continue;
        else {
            mOptionData.mSeenOptions.insert((char) val);
            if (longOptions_index == -1) {
                for (int i = 0;
                     longOptions[i].name || longOptions[i].has_arg ||
                     longOptions[i].flag || longOptions[i].val;
                     ++i) {
                    if (longOptions[i].val == val) {
                        longOptions_index = i;
                        break;
                    }
                }
            }
            if (longOptions_index >= 0) {
                const int shortOpt = gOptions[longOptions_index].shortOpt;
                switch (shortOpt) {
                    case 'h':
                        mOptionData.mPrintHelp = true;
                        break;

                    case 'v':
                        mOptionData.mPrintVersion = true;
                        break;

                    case 'P':
                        mOptionData.mPrintPythonPath = true;
                        break;

                    case 'c':
                        do {
                            SBFileSpec file(optarg);
                            if (file.Exists()) {
                                mOptionData.mCoreFile = optarg;
                            }
                            else {
                                error.SetErrorStringWithFormat(
                                    "file specified in --core (-c) "
                                    "option doesn't exist: '%s'", optarg
                                );
                            }
                        } while (0);
                        break;

                    case 'e':
                        mOptionData.mUseExternalEditor = true;
                        break;

                    case 'x':
                        mDebugger.SkipLLDBInitFiles(true);
                        mDebugger.SkipAppInitFiles(true);
                        break;

                    case 'X':
                        mDebugger.SetUseColor(false);
                        break;

                    case 'f':
                        do {
                            SBFileSpec file(optarg);
                            if (file.Exists()) {
                                mOptionData.mArgs.push_back (optarg);
                            }
                            else if (file.ResolveExecutableLocation()) {
                                char path[PATH_MAX];
                                file.GetPath(path, sizeof(path));
                                mOptionData.mArgs.push_back (path);
                            }
                            else {
                                error.SetErrorStringWithFormat(
                                    "file specified in --file (-f) option "
                                    "doesn't exist: '%s'", optarg
                                );
                            }
                        } while (0);
                        break;

                    case 'a':
                        if (!mDebugger.SetDefaultArchitecture(optarg)) {
                            error.SetErrorStringWithFormat(
                                "invalid architecture in the -a or "
                                "--arch option: '%s'", optarg
                            );
                        }
                        break;

                    case 'l':
                        mOptionData.mScriptLang = mDebugger.GetScriptingLanguage(
                                                      optarg
                                                  );
                        break;

                    case 'd':
                        mOptionData.mDebugMode = true;
                        break;

                    case 'Q':
                        mOptionData.mSourceQuietly = true;
                        break;

                    case 'n':
                        mOptionData.mProcessName = optarg;
                        break;

                    case 'w':
                        mOptionData.mWaitFor = true;
                        break;

                    case 'p':
                        do {
                            char *remainder;
                            mOptionData.mProcessPID = strtol(optarg,
                                                             &remainder, 0);
                            if (remainder == optarg || *remainder != '\0') {
                                error.SetErrorStringWithFormat(
                                    "Could not convert process PID: "
                                    "\"%s\" into a pid.", optarg);
                            }
                        } while (0);
                        break;

                    case 's':
                        mOptionData.AddInitialCommand(optarg, false,
                                                      true, error);
                        break;

                    case 'o':
                        mOptionData.AddInitialCommand(optarg, false,
                                                      false, error);
                        break;

                    case 'S':
                        mOptionData.AddInitialCommand(optarg, true,
                                                      true, error);
                        break;

                    case 'O':
                        mOptionData.AddInitialCommand(optarg, true,
                                                      false, error);
                        break;

                    default:
                        mOptionData.mPrintHelp = true;
                        error.SetErrorStringWithFormat(
                            "unrecognized option %c", shortOpt
                        );
                        break;
                }
            }
            else {
                error.SetErrorStringWithFormat(
                    "invalid option with value %i", val
                );
            }
            if (error.Fail()) {
                return error;
            }
        }
    }
    if (error.Fail() || mOptionData.mPrintHelp) {
        ShowUsage(outFH, gOptions, mOptionData);
        exiting = true;
    }
    else if (mOptionData.mPrintVersion) {
        fprintf(outFH, "%s\n", mDebugger.GetVersionString());
        exiting = true;
    }
    else if (mOptionData.mPrintPythonPath) {
        SBFileSpec python_file_spec = SBHostOS::GetLLDBPythonPath();
        if (python_file_spec.IsValid()) {
            char python_path[PATH_MAX];
            size_t num_chars = python_file_spec.GetPath(python_path, PATH_MAX);
            if (num_chars < PATH_MAX) {
                fprintf(outFH, "%s\n", python_path);
            }
            else fprintf (outFH, "<PATH TOO LONG>\n");
        }
        else {
            fprintf(outFH, "<COULD NOT FIND PATH>\n");
        }
        exiting = true;
    }
    else if (mOptionData.mProcessName.empty() &&
             mOptionData.mProcessPID == LLDB_INVALID_PROCESS_ID) {
        // Any arguments that are left over after option parsing are for the
        // program. If a file was specified with -f then the filename is already
        // in the mOptionData.mArgs array, and any remaining args are arguments
        // for the inferior program. If no file was specified with -f, then what
        // is left is the program name followed by any arguments.

        // Skip any options we consumed with getopt_long_only
        argc -= optind;
        argv += optind;
        if (argc > 0) {
            for (int argIdx = 0; argIdx < argc; ++argIdx) {
                const char *arg = argv[argIdx];
                if (arg) mOptionData.mArgs.push_back(arg);
            }
        }
    }
    else {
        // Skip any options we consumed with getopt_long_only
        argc -= optind;
        //argv += optind; // Commented out to keep static analyzer happy

        if (argc > 0) {
            fprintf( outFH, "Warning: program arguments are ignored "
                             "when attaching.\n");
        }
    }
    return error;
}

/**
 *
 */
void
LLDBDriver::WriteInitialCommands(
    bool before_file,
    SBStream &strm
) {
    using namespace std;
    vector< pair<bool, string> > &command_set = before_file ?
        mOptionData.mInitialCommands : mOptionData.mAfterFileCommands;

    for (const auto &command_pair : command_set) {
        const char *command = command_pair.second.c_str();
        if (command_pair.first) {
            strm.Printf("command source -s %i '%s'\n",
                         mOptionData.mSourceQuietly, command);
        }
        else strm.Printf("%s\n", command);
    }
}

/**
 *
 */
void
LLDBDriver::mainLoop(void)
{
    if (::tcgetattr(STDIN_FILENO, &oldStdinTermIOS) == 0) {
        oldStdinTermIOSIsValid = true;
        atexit(resetStdinTermIOS);
    }

    ::setbuf (stdin, NULL);
    ::setbuf (stdout, NULL);

    mDebugger.SetErrorFileHandle (stderr, false);
    mDebugger.SetOutputFileHandle (stdout, false);
    mDebugger.SetInputFileHandle (stdin, false); // Don't take ownership of STDIN yet...

    mDebugger.SetUseExternalEditor(mOptionData.mUseExternalEditor);

    struct winsize window_size;
    if (isatty(STDIN_FILENO)
        && ::ioctl (STDIN_FILENO, TIOCGWINSZ, &window_size) == 0) {
        if (window_size.ws_col > 0)
            mDebugger.SetTerminalWidth (window_size.ws_col);
    }

    SBCommandInterpreter sb_interpreter = mDebugger.GetCommandInterpreter();

    // Before we handle any options from the command line, we parse the
    // .lldbinit file in the user's home directory.
    SBCommandReturnObject result;
    sb_interpreter.SourceInitFileInHomeDirectory(result);
    if (GetDebugMode())
    {
        result.PutError (mDebugger.GetErrorFileHandle());
        result.PutOutput (mDebugger.GetOutputFileHandle());
    }

    // Now we handle options we got from the command line
    SBStream commands_stream;

    // First source in the commands specified to be run before the file arguments are processed.
    WriteInitialCommands(true, commands_stream);

    const size_t numArgs = mOptionData.mArgs.size();
    if (numArgs > 0)
    {
        char arch_name[64];
        if (mDebugger.GetDefaultArchitecture (arch_name, sizeof (arch_name)))
            commands_stream.Printf("target create --arch=%s \"%s\"", arch_name, mOptionData.mArgs[0].c_str());
        else
            commands_stream.Printf("target create \"%s\"", mOptionData.mArgs[0].c_str());

        if (!mOptionData.mCoreFile.empty())
        {
            commands_stream.Printf(" --core \"%s\"", mOptionData.mCoreFile.c_str());
        }
        commands_stream.Printf("\n");

        if (numArgs > 1)
        {
            commands_stream.Printf ("settings set -- target.run-args ");
            for (size_t argIdx = 1; argIdx < numArgs; ++argIdx)
            {
                const char *arg_cstr = mOptionData.mArgs[argIdx].c_str();
                if (strchr(arg_cstr, '"') == NULL)
                    commands_stream.Printf(" \"%s\"", arg_cstr);
                else
                    commands_stream.Printf(" '%s'", arg_cstr);
            }
            commands_stream.Printf("\n");
        }
    }
    else if (!mOptionData.mCoreFile.empty())
    {
        commands_stream.Printf("target create --core \"%s\"\n", mOptionData.mCoreFile.c_str());
    }
    else if (!mOptionData.mProcessName.empty())
    {
        commands_stream.Printf ("process attach --name \"%s\"", mOptionData.mProcessName.c_str());

        if (mOptionData.mWaitFor)
            commands_stream.Printf(" --waitfor");

        commands_stream.Printf("\n");

    }
    else if (LLDB_INVALID_PROCESS_ID != mOptionData.mProcessPID)
    {
        commands_stream.Printf ("process attach --pid %" PRIu64 "\n", mOptionData.mProcessPID);
    }

    WriteInitialCommands(false, commands_stream);

    // Now that all option parsing is done, we try and parse the .lldbinit
    // file in the current working directory
    sb_interpreter.SourceInitFileInCurrentWorkingDirectory (result);
    if (GetDebugMode())
    {
        result.PutError(mDebugger.GetErrorFileHandle());
        result.PutOutput(mDebugger.GetOutputFileHandle());
    }

    bool handle_events = true;
    bool spawn_thread = false;

    // Check if we have any data in the commands stream, and if so, save it to a temp file
    // so we can then run the command interpreter using the file contents.
    const char *commands_data = commands_stream.GetData();
    const size_t commands_size = commands_stream.GetSize();
    if (commands_data && commands_size)
    {
        enum PIPES { READ, WRITE }; // Constants 0 and 1 for READ and WRITE

        bool success = true;
        int fds[2] = { -1, -1 };
        int err = 0;
#ifdef _WIN32
        err = _pipe(fds, commands_size, O_BINARY);
#else
        err = pipe(fds);
#endif
        if (err == 0)
        {
            ssize_t nrwr = write(fds[WRITE], commands_data, commands_size);
            if (nrwr < 0)
            {
                fprintf(stderr, "error: write(%i, %p, %zd) failed (errno = %i) "
                                "when trying to open LLDB commands pipe\n",
                        fds[WRITE], commands_data, commands_size, errno);
                success = false;
            }
            else if (static_cast<size_t>(nrwr) == commands_size)
            {
                // Close the write end of the pipe so when we give the read end to
                // the debugger/command interpreter it will exit when it consumes all
                // of the data
#ifdef _WIN32
                _close(fds[WRITE]); fds[WRITE] = -1;
#else
                close(fds[WRITE]); fds[WRITE] = -1;
#endif
                // Now open the read file descriptor in a FILE * that we can give to
                // the debugger as an input handle
                FILE *commands_file = fdopen(fds[READ], "r");
                if (commands_file)
                {
                    fds[READ] = -1; // The FILE * 'commands_file' now owns the read descriptor
                    // Hand ownership if the FILE * over to the debugger for "commands_file".
                    mDebugger.SetInputFileHandle (commands_file, true);
                    mDebugger.RunCommandInterpreter(handle_events, spawn_thread);
                }
                else
                {
                    fprintf(stderr,
                            "error: fdopen(%i, \"r\") failed (errno = %i) when "
                            "trying to open LLDB commands pipe\n",
                            fds[READ], errno);
                    success = false;
                }
            }
            else
            {
                assert(!"partial writes not handled");
                success = false;
            }
        }
        else
        {
            fprintf(stderr, "error: can't create pipe file descriptors for LLDB commands\n");
            success = false;
        }

        // Close any pipes that we still have ownership of
        if ( fds[WRITE] != -1)
        {
#ifdef _WIN32
            _close(fds[WRITE]); fds[WRITE] = -1;
#else
            close(fds[WRITE]); fds[WRITE] = -1;
#endif

        }

        if ( fds[READ] != -1)
        {
#ifdef _WIN32
            _close(fds[READ]); fds[READ] = -1;
#else
            close(fds[READ]); fds[READ] = -1;
#endif
        }

        // Something went wrong with command pipe
        if (!success)
        {
            exit(1);
        }

    }

    // Now set the input file handle to STDIN and run the command
    // interpreter again in interactive mode and let the debugger
    // take ownership of stdin
    mDebugger.SetInputFileHandle (stdin, true);
    mDebugger.RunCommandInterpreter(handle_events, spawn_thread);

    resetStdinTermIOS();
    fclose (stdin);

    SBDebugger::Destroy (mDebugger);
}

////////////////////////////////////////////////////////////////////////////////
// LLDBDriver::OptionData
////////////////////////////////////////////////////////////////////////////////

LLDBDriver::OptionData::OptionData(void) :
    mArgs(),
    mScriptLang (lldb::eScriptLanguageDefault),
    mCoreFile(),
    mCrashLog(),
    mInitialCommands(),
    mAfterFileCommands(),
    mDebugMode(false),
    mSourceQuietly(false),
    mPrintVersion(false),
    mPrintPythonPath(false),
    mPrintHelp(false),
    mWaitFor(false),
    mProcessName(),
    mProcessPID(LLDB_INVALID_PROCESS_ID),
    mUseExternalEditor(false),
    mSeenOptions()
{
    ;
}

/**
 *
 */
LLDBDriver::OptionData::~OptionData(void)
{
    ;
}

/**
 *
 */
void
LLDBDriver::OptionData::clear(void)
{
    mArgs.clear();
    mScriptLang = lldb::eScriptLanguageDefault;
    mInitialCommands.clear();
    mAfterFileCommands.clear();
    mDebugMode = false;
    mSourceQuietly = false;
    mPrintHelp = false;
    mPrintVersion = false;
    mPrintPythonPath = false;
    mUseExternalEditor = false;
    mWaitFor = false;
    mProcessName.erase();
    mProcessPID = LLDB_INVALID_PROCESS_ID;
}

/**
 *
 */
void
LLDBDriver::OptionData::AddInitialCommand(
    const char *command,
    bool before_file,
    bool is_file,
    SBError &error
) {
    std::vector<std::pair<bool, std::string> > *command_set;
    if (before_file)
        command_set = &(mInitialCommands);
    else
        command_set = &(mAfterFileCommands);

    if (is_file)
    {
        SBFileSpec file(command);
        if (file.Exists())
            command_set->push_back (std::pair<bool, std::string> (true, optarg));
        else if (file.ResolveExecutableLocation())
        {
            char final_path[PATH_MAX];
            file.GetPath (final_path, sizeof(final_path));
            std::string path_str (final_path);
            command_set->push_back (std::pair<bool, std::string> (true, path_str));
        }
        else
            error.SetErrorStringWithFormat("file specified in --source (-s) option doesn't exist: '%s'", optarg);
    }
    else
        command_set->push_back (std::pair<bool, std::string> (false, optarg));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// GladiusDBE
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
GladiusDBE::GladiusDBE(
    int argc,
    const char **argv,
    const char **envp
) {
    GLADIUS_UNUSED(envp);
    using namespace gladius;
    using namespace lldb;

    try {
        // stash argument info
        this->argC = argc;
        this->argV = argv;
        this->envP = envp;
        SBDebugger::Initialize();
        SBHostOS::ThreadCreated("<lldb.driver.main-thread>");
        setSignalHandlers();
    }
    catch(const std::exception &e) {
        throw core::GladiusException(GLADIUS_WHERE, e.what());
    }
}

/**
 *
 */
GladiusDBE::~GladiusDBE(void)
{
    using namespace lldb;
    SBDebugger::Terminate();
}

/**
 *
 */
void
GladiusDBE::mainLoop(void)
{
    using namespace lldb;
    // Create a scope for driver so that the driver object will destroy itself
    // before SBDebugger::Terminate() is called in the destructor.
    do {
        LLDBDriver driver;
        bool exiting = false;
        SBError error(driver.ParseArgs(argC, argV, stdout, exiting));
        if (error.Fail()) {
            const char *errorCStr = error.GetCString();
            if (errorCStr) fprintf(stderr, "error: %s\n", errorCStr);
        }
        else if (!exiting) {
            driver.mainLoop();
        }
    } while(0);
}
