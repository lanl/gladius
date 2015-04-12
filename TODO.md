# Gladius TODO

- Add higher-level send/recv abstraction that can be used by all infrastructure.
- On BE failure, make sure that we can determine which node failed.
- Add package config to get plugin flags.
- Make some libraries internal and others usable by external plugins.
- Install gladius plugin headers so external plugins can be built.
- Set environment/runtime variables for parallel debugging
  (e.g. -mca debugger mpirx).
- Can we use the "Scalable Binary Deployment system?"
- Implement component registry scheme.
- Register timeout and retry environment variabes to tool FE.
- Automatically push PATH to remote hosts.
  Now, we are relying on paths to the base infrastructure to be in .rcs that get
  sourced during remote login. What does LMON_fe_putToBeDaemonEnv do? This may
  be useful.
- Add logging support in terminal.
- Add verbosity levels.
- Populate help strings in term-cmds.
- Parsing for application version, etc.
- Terminal UI autocomplete.
- Terminal ls and cd
- ToolFE reset signal handlers and such (LMON).
- Fix weird terminal behavior after LMON launch.
- GUI
- Add basic fe/be plugins that perform basic communication.
- MRNetFE dynamically create a reasonable topology based on job size.
