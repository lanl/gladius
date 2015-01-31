/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-common/tool-common.h"
#include "core/core.h"

using namespace gladius;
using namespace gladius::toolcommon;

namespace {
// This component's name.
static const std::string CNAME = "toolcmn";
// CNAME's color code.
static const std::string NAMEC =
    core::colors::color().ansiBeginColor(core::colors::DGRAY);
// Convenience macro to decorate this component's output.
#define COMP_COUT GLADIUS_COMP_COUT(CNAME, NAMEC)
}

/**
 *
 */
void
ProcessTable::dumpTo(std::ostream &os) {
    using namespace std;
    os << "*** Process Table Dump ***" << endl;
    if (!mProcTab) {
        os << "XXX Process Table Not Allocated XXX" << endl;
        return;
    }
    for (decltype(mNEntries) i = 0; i < mNEntries; ++i) {
        os << "Host Name: " << mProcTab[i].pd.host_name << endl;
        os << "Executable Name: "
           << mProcTab[i].pd.executable_name << endl;
        os << "PID: " << mProcTab[i].pd.pid << " "
           // TID: "Task ID"
           << "TID: " << mProcTab[i].mpirank
           << endl;
    }
}
