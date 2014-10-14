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

#include <string>
#include <csignal>
#include <unistd.h>

using namespace gladius::dbe;

namespace {
/**
 * Window size change signal handler.
 */
void
sigwinchHandler(int signo)
{
#if 0
    struct winsize window_size;
    if (isatty(STDIN_FILENO) &&
        0 == ::ioctl(STDIN_FILENO, TIOCGWINSZ, &window_size)) {
        if ((window_size.ws_col > 0) && g_driver != NULL) {
            g_driver->ResizeWindow (window_size.ws_col);
        }
    }
#endif
}

/**
 * Interrupt program (Ctrl-C) signal handler.
 */
void
sigintHandler(int signo)
{
#if 0
    static bool g_interrupt_sent = false;
    if (g_driver) {
        if (!g_interrupt_sent) {
            g_interrupt_sent = true;
            g_driver->GetDebugger().DispatchInputInterrupt();
            g_interrupt_sent = false;
            return;
        }
    }
    exit (signo);
#endif
}

void
setSigHandlers(void)
{
    (void)signal(SIGPIPE, SIG_IGN);
    (void)signal(SIGWINCH, sigwinchHandler);
    (void)signal(SIGINT, sigintHandler);
#if 0
    (void)signal(SIGTSTP, sigtstp_handler);
    (void)signal(SIGCONT, sigcont_handler);
#endif
}
} // end private routines namespace

/**
 *
 */
GladiusDBE::~GladiusDBE(void)
{
}

/**
 *
 */
GladiusDBE::GladiusDBE(
    int argc,
    char **argv,
    char **envp
) {
    (void)argc;
    (void)argv;
    (void)envp;
    using namespace lldb;

    SBDebugger::Initialize();
    SBHostOS::ThreadCreated ("<lldb.driver.main-thread>");
    setSigHandlers();
#if 0

    // Create a scope for driver so that the driver object will destroy itself
    // before SBDebugger::Terminate() is called.
    do {
        Driver driver;

        bool exiting = false;
        SBError error (driver.ParseArgs (argc, argv, stdout, exiting));
        if (error.Fail())
        {
            const char *error_cstr = error.GetCString ();
            if (error_cstr)
                ::fprintf (stderr, "error: %s\n", error_cstr);
        }
        else if (!exiting)
        {
            driver.MainLoop ();
        }
    } while(0);
#endif
    SBDebugger::Terminate();

}

/**
 *
 */
void
GladiusDBE::mainLoop(
    void
) {
}
