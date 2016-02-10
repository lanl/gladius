#pragma once
#include "convenience.h"
//
inline void
launchDAXPYTasks(
    MainTaskState &s, Context ctx, HighLevelRuntime *rt
) {
    const double alpha = drand48();
    ArgumentMap argMap; TaskArgument taskArg(&alpha, sizeof(alpha));
    IndexLauncher daxpyIL(
        DAXPY_TASK_ID, s.colorDomain, taskArg, argMap
    );
    daxpyIL.add_region_requirement(
        RegionRequirement(s.inputLP, 0, READ_ONLY, EXCLUSIVE, s.inputLR)
    );
    daxpyIL.region_requirements[0].add_field(FID_X);
    daxpyIL.region_requirements[0].add_field(FID_Y);
    daxpyIL.add_region_requirement(
        RegionRequirement(s.outputLP, 0, WRITE_DISCARD, EXCLUSIVE, s.outputLR)
    );
    daxpyIL.region_requirements[1].add_field(FID_Z);
    rt->execute_index_space(ctx, daxpyIL);
}
//
inline void
daxpyTask(
    const Task *task, const PhysicalRegions &regions, Context ctx, HighLevelRuntime *rt
) {
    // Extract task arguments.
    const double alpha = *((const double *)task->args);
    // Obtain region accessors required for this computation.
    DRegionAccessor accX = regions[0].getFieldAccessor(FID_X);
    DRegionAccessor accY = regions[0].getFieldAccessor(FID_Y);
    DRegionAccessor accZ = regions[1].getFieldAccessor(FID_Z);
    // In parallel, perform the computation over the respective sub-regions.
    Rect<1> rect = getSubRect1DFromRegion(0);
    for (GenericPointInRectIterator<1> pir(rect); pir; pir++) {
        double v = alpha * accX.readPointVal(pir.p) + accY.readPointVal(pir.p);
        accZ.writePointVal(pir.p, v);
    }
}
