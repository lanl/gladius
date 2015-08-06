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

////////////////////////////////////////////////////////////////////////////////
struct TaskKind {
    taskid_t taskID = 0;
    //
    std::string name;
    //
    TaskKind(
        uint32_t taskID,
        const std::string &name
    ) : taskID(taskID)
      , name(name) { }

};

////////////////////////////////////////////////////////////////////////////////
struct TaskInfo {
    taskid_t taskID = 0;
    //
    uint32_t funcID = 0;
    //
    uint64_t procID = 0;
    //
    uint64_t uCreateTime = 0;
    //
    uint64_t uReadyTime = 0;
    //
    uint64_t uStartTime = 0;
    //
    uint64_t uStopTime = 0;
    //
    TaskInfo(
        int32_t taskID,
        int32_t funcID,
        uint64_t procID,
        uint64_t uCreateTime,
        uint64_t uReadyTime,
        uint64_t uStartTime,
        uint64_t uStopTime
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
    uint64_t procID = 0;
    //
    ProcType kind = ProcType::UNKNOWN;
    //
    ProcDesc(
        uint64_t procID,
        ProcType kind
    ) : procID(procID)
      , kind(kind) { }
};

////////////////////////////////////////////////////////////////////////////////
struct LegionProfData {
    //
    LegionProfData& operator=(const LegionProfData&) = delete;
    // Map between taskIDs to TaskKinds
    std::map<taskid_t, TaskKind> taskKinds;
    //
    std::deque<TaskInfo> taskInfos;
    //
    std::deque<ProcDesc> procDescs;
    /**
     * @brief nProcessors
     * @return
     */
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
