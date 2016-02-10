#include "legion.h"      // Legion API.
#include "convenience.h" // Convenience code.
#include "data-ops.h"    // Data operations.
#include "task-defs.h"   // Task definitions.

// The first Legion function that is executed by the top-level task.
void
mainTask(
    const Task *, const PhysicalRegions &, Context ctx, HighLevelRuntime *rt
) {
    // The main task is not replicated, so only 1 task
    // executes the code in this function block.
    // A convenience structure that holds Legion DAXPY structures.
    MainTaskState mts;
    // Total number of elements (globally); The number of partitions.
    int numElements = 1024, numParts = 4;
    cout << "Running DAXPY on "       << numElements << " elements" << endl;
    cout << "Partitioning data into " << numParts << " sub-regions" << endl;
    // Create a 1D index space for 1024 points.
    createIndexSpace(mts, numElements, ctx, rt);
    // Create the field spaces.
    createFieldSpaces(mts, ctx, rt);
    // Create logical regions: input (with two fields) and output (with one).
    createLogicalRegions(mts, ctx, rt);
    // Describe the logical partitioning (coloring) of the logical data.
    partitionIndexSpace(mts, numParts, ctx, rt);
    // Done describing data to the runtime. Now perform computations.
    // Schedule launching of tasks responsible for initializing X and Y.
    launchInitializeRegionTasks(mts, ctx, rt);
    // Schedule launching of tasks responsible for DAXPY computation.
    launchDAXPYTasks(mts, ctx, rt);
    // Tear down Legion structures allocated during execution.
    finalize(mts, ctx, rt);
}
//
int
main(
    int argc, char **argv
) {
    // Register all our Legion tasks.
    HighLevelRuntime::set_top_level_task_id(MAIN_TASK_ID);
    registerTask(mainTask,  MAIN_TASK_ID,  Processor::LOC_PROC, "main");
    registerTask(initTask,  INIT_TASK_ID,  Processor::LOC_PROC, "init");
    registerTask(daxpyTask, DAXPY_TASK_ID, Processor::LOC_PROC, "daxpy");
    // Start the runtime. mainTask will called by the runtime.
    return HighLevelRuntime::start(argc, argv);
}
