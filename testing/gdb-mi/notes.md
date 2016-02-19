# Notes on GDB Machine Interface (MI)

## Starting GDB in MI Mode

```
gdb --interpreter=mi -q
(gdb) attach [PID]
(gdb) set var done=1
(gdb) continue
```
