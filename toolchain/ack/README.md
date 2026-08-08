This directory is a working extraction of the ACK-based C toolchain pieces
relevant to the 68k Unix effort in this repository.

This tree is the local working copy used for focused work, reduction, and
local build logic.

Current layout:

- `cpp/`
  - ACK ANSI C preprocessor sources from `lang/cem/cpp.ansi`
- `c1/`
  - ACK ANSI C frontend/compiler sources from `lang/cem/cemcom.ansi`
- `c2/`
  - reserved for a later backend/codegen split if one proves useful
- `opt/`
  - ACK optimizer sources from `util/opt`
- `ln/`
  - ACK linker sources from `util/led`
- `as/`
  - ACK m68k assembler sources from `mach/m68k2/as`
- `shared/`
  - shared headers, support modules, and generator tools needed by the pieces above
- `targets/68kunix/`
  - current 68k Unix target baseline copied from `plat/minix68k`
- `man+doc/`
  - selected ACK documentation relevant to C, code generation, optimization, and installation
- `cc/cc/cc`
  - local shell driver for the host-built pipeline
  - supports `-O0`, `-O1`, `-O2`, `-o`, and `-c`
  - stages host tools with short names like `cpp`, `c1`, `opt`, and `opt2`

This is intentionally conservative:

- no target-specific rewrites were done yet
- `targets/68kunix/` still reflects the `minix68k` baseline and needs adaptation

Immediate next steps:

1. add a standalone host `Makefile.clang` for `cpp/`
2. decide whether `c1/` can build against `shared/` without the full ACK tree
3. replace Minix-specific assumptions in `targets/68kunix/`
4. decide whether a real `c2/` split is warranted
