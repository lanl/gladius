/*
 * Copyright (c) 2015      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the Gladius project. See the LICENSE.txt file at the
 * top-level directory of this distribution.
 */

// Enable Analysis Infrastructure
// ./tooli -hl:analysis [#ANLYS] -cat legion_analysis -level 2

#include <cstdio>

#include "legion.h"
#include "legion_analysis_utilities.h"

#include <vector>

using namespace LegionRuntime::HighLevel;

enum TaskID {
    TLTID,
    NLTID
};

// All single-launch tasks in Legion must have this signature with
// the extension that they can have different return values.
void
topLevelTask(
    const Task *,
    const std::vector<PhysicalRegion> &,
    Context ctx,
    HighLevelRuntime *rt
) {
    printf("app:enter: %s\n", __func__);
    //
    TaskLauncher launcher(NLTID, TaskArgument(NULL, 0));
    rt->execute_task(ctx, launcher);
    //
    printf("app:exit:  %s\n", __func__);
}

void
nextLevelTask(
    const Task *,
    const std::vector<PhysicalRegion> &,
    Context,
    HighLevelRuntime *
) {
    printf("app:enter: %s\n", __func__);
    printf("app:exit:  %s\n", __func__);
}

// We have a main function just like a standard C++ program.
// Once we start the runtime, it will begin running the top-level task.
int
main(
    int argc,
    char **argv
) {
    HighLevelRuntime::set_top_level_task_id(TLTID);

    HighLevelRuntime::register_legion_task<topLevelTask>(
        TLTID,
        Processor::LOC_PROC,
        true  /*single*/,
        false /*index*/
    );
    HighLevelRuntime::register_legion_task<nextLevelTask>(
        NLTID,
        Processor::LOC_PROC,
        true  /*single*/,
        false /*index*/
    );

    // Register ALL tools BEFORE HighLevelRuntime::start()
    LegionTool *aTool = NULL;
    HighLevelRuntime::register_legion_tool(&aTool);

    return HighLevelRuntime::start(argc, argv);
}
