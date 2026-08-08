# ACK libc for 68kunix

This directory is for ACK-native runtime and libc pieces for the Epoch68/V7
target.

The existing Sozobon/V7 libc is still the reference implementation for API
coverage, but its object files and assembler syntax are not directly usable by
ACK `led`. Reuse should happen at the source level, with target-specific
startup and syscall wrappers written in ACK assembler syntax.

Current contents:

- `68kunix/crt0.s`: minimal process startup for the current zero-based 0413
  loader path.
- `68kunix/*.s`: first syscall wrappers for `read`, `write`, `open`, `close`,
  `creat`, `unlink`, `lseek`, `brk`, `sbrk`, and `exit` using the current 68k
  V7 ABI.
- `string.c`: early string routines.
- `memory.c`: early memory routines.
- `stdlib.c`: minimal `atoi`, `atol`, and bump-allocation `malloc` support.

Known gaps before this can be a general libc:

- many more syscall wrappers
- real allocator/free-list support
- ACK arithmetic/runtime helper routines such as unsigned division
- investigation of static pointer initializers emitting 8-byte data
