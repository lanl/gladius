/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-common/tool-common.h"
#include "core/core.h"

namespace {
// This component's name.
static const std::string CNAME = "toolcmn";
// CNAME's color code.
static const std::string NAMEC = gladius::core::utils::ansiBeginColorMagenta();
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
}

using namespace gladius::toolcommon;
/**
 *
 */
void
ProcessTable::dump(void) {
    using namespace std;
    COMP_COUT << "*** Process Table Dump ***" << endl;
    if (!mProcTab) {
        COMP_COUT << "XXX Process Table Not Allocated XXX" << endl;
        return;
    }
    for (auto i = 0U; i < mNEntries; ++i) {
        COMP_COUT << "Host Name: " << mProcTab[i].pd.host_name << endl;
        COMP_COUT << "Executable Name: "
                  << mProcTab[i].pd.executable_name << endl;
        COMP_COUT << "PID: " << mProcTab[i].pd.pid << " "
                     // NID: "Node ID"
                  << "NID: " << mProcTab[i].mpirank
                  << endl;
    }
    COMP_COUT << endl;
}

using namespace gladius::toolcommon;
