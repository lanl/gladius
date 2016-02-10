#pragma once
#include "convenience.h"

inline void
createIndexSpace(
    MainTaskState &s, int numElements, Context ctx, HighLevelRuntime *rt
) {
    Rect<1> elemRect(Point<1>(0), Point<1>(numElements - 1));
    s.is = rt->create_index_space(ctx, getRect1D(elemRect));
}

inline void
createFieldSpaces(
    MainTaskState &s, Context ctx, HighLevelRuntime *rt
) {
    s.inputFS = rt->create_field_space(ctx); {
        FieldAllocator a = rt->create_field_allocator(ctx, s.inputFS);
        a.allocate_field(sizeof(double), FID_X);
        a.allocate_field(sizeof(double), FID_Y);
    }
    s.outputFS = rt->create_field_space(ctx); {
        FieldAllocator a = rt->create_field_allocator(ctx, s.outputFS);
        a.allocate_field(sizeof(double), FID_Z);
    }
}

inline void
createLogicalRegions(
    MainTaskState &s, Context ctx, HighLevelRuntime *rt
) {
    s.inputLR  = rt->create_logical_region(ctx, s.is, s.inputFS );
    s.outputLR = rt->create_logical_region(ctx, s.is, s.outputFS);
}

inline void
partitionIndexSpace(
    MainTaskState &s, int numParts, Context ctx, HighLevelRuntime *rt
) {
    int numElements = rt->get_index_space_domain(ctx, s.is).get_volume();
    s.colorDomain = getRect1D(Rect<1>(Point<1>(0), Point<1>(numParts - 1)));
    Blockify<1> coloring(numElements / numParts);
    s.ip = rt->create_index_partition(ctx, s.is, coloring);
    s.inputLP  = rt->get_logical_partition(ctx, s.inputLR,  s.ip);
    s.outputLP = rt->get_logical_partition(ctx, s.outputLR, s.ip);
}

inline void
finalize(
    MainTaskState &s, Context ctx, HighLevelRuntime *rt
) {
    rt->destroy_logical_region(ctx, s.inputLR);
    rt->destroy_logical_region(ctx, s.outputLR);
    rt->destroy_field_space(ctx, s.inputFS);
    rt->destroy_field_space(ctx, s.outputFS);
    rt->destroy_index_space(ctx, s.is);
}
