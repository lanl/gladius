# Notes on GASNet + MPIR

## Configuration
Gladius assumes the presence of MPIR. Looks like the MPI spawner is only
optionally used for IBV and MXM conduits. We need to find a workaround for this.

## Debugging
See: gasneti_freezeForDebugger()
Environment Variable: GASNET_FREEZE

To Debug:
export GASNET_FREEZE=1
./gasnetApp

Can We Interpose:
- gasnet_init
- gasnet_exit

## For More Information Please See:
other/mpi-spawner in your GASNet source tree.
