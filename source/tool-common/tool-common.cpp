/*
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#include "tool-common/tool-common.h"
#include "core/core.h"

#include "mrnet/MRNet.h"

using namespace gladius;
using namespace gladius::toolcommon;

/**
 *
 */
int
MRNetCoreTags::getFirstAppTag(void) const {
    return FirstApplicationTag;
}

/**
 *
 */
void
ProcessTable::dumpTo(
    std::ostream &os,
    const std::string &outPrefix,
    core::colors::Color color
) const {
    using namespace std;
    using namespace core;

    std::string outp;
    if (core::colors::Color::NONE != color) {
        const auto &cr = core::colors::color();
        outp = cr.ansiBeginColor(color) + outPrefix + cr.ansiEndColor();
    }
    else {
        outp = outPrefix;
    }
    os << outp << "*** Process Table Dump ***" << endl;
    if (!mProcTab) {
        os << outp << "XXX Process Table Not Allocated XXX" << endl;
        return;
    }
    for (decltype(mNEntries) i = 0; i < mNEntries; ++i) {
        os << outp << "Host Name: " << mProcTab[i].pd.host_name << endl;
        os << outp << "Executable Name: "
           << mProcTab[i].pd.executable_name << endl;
        os << outp << "PID: " << mProcTab[i].pd.pid << " "
           // TID: "Task ID"
           << "TID: " << mProcTab[i].mpirank
           << endl;
    }
}
