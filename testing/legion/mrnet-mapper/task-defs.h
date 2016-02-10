#pragma once
#include "convenience.h"
#include "daxpy-defs.h"
//
inline void
launchInitializeRegionTasks(
    MainTaskState &s, Context ctx, HighLevelRuntime *rt
) {
    ArgumentMap argMap;
    // Schedule launching of tasks responsible for region initialization.
    IndexLauncher initl(INIT_TASK_ID, s.colorDomain, nullTaskArgs, argMap);
    initl.add_region_requirement(
        RegionRequirement(s.inputLP, 0, WRITE_DISCARD, EXCLUSIVE, s.inputLR)
    );
    initl.region_requirements[0].add_field(FID_X);
    // X
    rt->execute_index_space(ctx, initl);
    initl.region_requirements[0].privilege_fields.clear();
    initl.region_requirements[0].instance_fields.clear();
    initl.region_requirements[0].add_field(FID_Y);
    // Y
    rt->execute_index_space(ctx, initl);
}
//
void initTask(
    const Task *task, const PhysicalRegions &regions, Context ctx, HighLevelRuntime *rt
) {
    // Determine on which field we are working.
    FieldID fid = *(task->regions[0].privilege_fields.begin());
    // Obtain region accessors required for this computation.
    DRegionAccessor acc = regions[0].getFieldAccessor(fid);
    // Initialize our sub-region.
    Rect<1> rect = getSubRect1DFromRegion(0);
    for (GenericPointInRectIterator<1> pir(rect); pir; pir++) {
        acc.writePointVal(pir.p, drand48());
    }
}
