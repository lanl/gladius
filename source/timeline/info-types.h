/**
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

#ifndef TIMELINE_INFO_TYPES_H_INCLUDED
#define TIMELINE_INFO_TYPES_H_INCLUDED

#include "common.h"

#include <deque>
#include <string>
#include <map>
#include <stdint.h>

typedef uint32_t taskid_t;
typedef uint32_t funcid_t;
typedef uint64_t procid_t;
typedef uint64_t ustime_t;
typedef uint32_t opid_t;
typedef uint32_t hlrid_t;

////////////////////////////////////////////////////////////////////////////////
struct TaskKind {
    taskid_t taskID = 0;
    //
    std::string name;
    //
    TaskKind(
        taskid_t taskID,
        const std::string &name
    ) : taskID(taskID)
      , name(name) { }

};

////////////////////////////////////////////////////////////////////////////////
struct TaskInfo {
    taskid_t taskID = 0;
    //
    funcid_t funcID = 0;
    //
    procid_t procID = 0;
    //
    ustime_t uCreateTime = 0;
    //
    ustime_t uReadyTime = 0;
    //
    ustime_t uStartTime = 0;
    //
    ustime_t uStopTime = 0;
    //
    TaskInfo(
        taskid_t taskID,
        funcid_t funcID,
        procid_t procID,
        ustime_t uCreateTime,
        ustime_t uReadyTime,
        ustime_t uStartTime,
        ustime_t uStopTime
    ) : taskID(taskID)
      , funcID(funcID)
      , procID(procID)
      , uCreateTime(uCreateTime)
      , uReadyTime(uReadyTime)
      , uStartTime(uStartTime)
      , uStopTime(uStopTime) { }
};

////////////////////////////////////////////////////////////////////////////////
struct ProcDesc {
    //
    procid_t procID = 0;
    //
    ProcType kind = ProcType::UNKNOWN;
    //
    ProcDesc(
        procid_t procID,
        ProcType kind
    ) : procID(procID)
      , kind(kind) { }
};

////////////////////////////////////////////////////////////////////////////////
struct MetaDesc {
    //
    opid_t id = 0;
    //
    std::string name;
    //
    MetaDesc(
        opid_t id,
        const std::string &name
    ) : id(id)
      , name(name) { }
};

////////////////////////////////////////////////////////////////////////////////
struct LegionProfData {
    //
    LegionProfData& operator=(const LegionProfData&) = delete;
    // Map between taskIDs to TaskKinds
    std::map<taskid_t, TaskKind *> taskKinds;
    //
    std::deque<TaskInfo> taskInfos;
    //
    std::deque<TaskInfo> metaInfos;
    //
    std::deque<ProcDesc> procDescs;
    //
    std::map<opid_t, MetaDesc *> metaDescs;
    //
    ~LegionProfData(void) {
        for (auto &taskKind : taskKinds) {
            delete taskKind.second;
        }
        for (auto &metaDesc: metaDescs) {
            delete metaDesc.second;
        }
    }
    //
    size_t
    nProcessors(void) const {
        return procDescs.size();
    }
};

////////////////////////////////////////////////////////////////////////////////
class Task {
public:
    Task(
        const std::string &name
    );
private:
    std::string mName;
};

#endif
