/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_COMMON_H
#define TIMELINE_COMMON_H

#include <QMetaType>
#include <QString>

#include <cstring>

#include <stdint.h>

#define APP_NAME      "timeline"
#define APP_WIN_TITLE "Task Execution Timeline"

struct Status {
    //
    static constexpr uint8_t ok     = 0;
    static constexpr uint8_t errors = 1;
    //
    uint8_t code;
    //
    QString errs;
    //
    Status(const QString &errs)
        : code(errors)
        , errs(errs) { }
    //
    Status(
        uint8_t code,
        const QString &errs
    ) : code(code)
      , errs(errs) { }
    //
    static Status
    Okay(void) {
        static const Status statOk(ok, "Ok");
        return statOk;
    }
    //
    bool operator==(const Status &other) const {
        return other.code == this->code;
    }
};

// Keep This In Sync With Legion
enum ProcType {
    TOC_PROC,   // Throughput core (GPU)
    LOC_PROC,   // Latency core (CPU)
    UTIL_PROC,  // Utility core
    IO_PROC,    // I/O core
    PROC_GROUP, // Processor group
    UNKNOWN     // ???
};

//
enum StatusKind {
    INFO = 0,
    WARN,
    ERR
};
Q_DECLARE_METATYPE(StatusKind);

namespace Common {

inline void
registerMetaTypes(void) {
    qRegisterMetaType<StatusKind>("StatusKindType");
}

}


static const int sMicroSecPerPixel = 1e2;

#endif // TIMELINE_COMMON_H

