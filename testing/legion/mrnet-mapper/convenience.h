#pragma once
#include <cstdio>
#include <cstdlib>
#include "legion.h"
//
enum TaskIDs {MAIN_TASK_ID, INIT_TASK_ID, DAXPY_TASK_ID};
enum FieldIDs {FID_X, FID_Y, FID_Z};
//
using namespace std;
using namespace LegionRuntime::HighLevel;
using namespace LegionRuntime::Accessor;
//
struct MainTaskState {
    IndexSpace       is;
    IndexPartition   ip;
    FieldSpace       inputFS, outputFS;
    LogicalRegion    inputLR, outputLR;
    Domain           colorDomain;
    LogicalPartition inputLP, outputLP;
};
//
#define getFieldAccessor(fid) get_field_accessor((fid)).typeify<double>()

#define getSubRect1DFromRegion(regionID)                                       \
    rt->get_index_space_domain(                                                \
        ctx, task->regions[(regionID)].region.get_index_space()                \
    ).get_rect<1>()

#define readPointVal(pir1dp) read(DomainPoint::from_point<1>((pir1dp)))

#define writePointVal(p, val) write(DomainPoint::from_point<1>((p)), (val))

#define getRect1D(rect) Domain::from_rect<1>((rect))

#define nullTaskArgs TaskArgument(NULL, 0)

#define registerTask(fnPtr, tid, procKind, name)                               \
    HighLevelRuntime::register_legion_task<fnPtr>(                             \
        tid, procKind, true, false,                                            \
        AUTO_GENERATE_ID, TaskConfigOptions(), name                            \
    );
//
typedef RegionAccessor<AccessorType::Generic, double> DRegionAccessor;
typedef std::vector<PhysicalRegion> PhysicalRegions;
