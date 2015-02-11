# Notes on Debugging 

## Linux and Ptrace
To either get the tracer's PID or to determine whether or not a process is being
debugged, simply cat /proc/[PID]/status and consult the TracerPid entry.

## Setup TODO FIXME
http://askubuntu.com/questions/41629/after-upgrade-gdb-wont-attach-to-process
