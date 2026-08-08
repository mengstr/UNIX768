# ACK software floating-point runtime

The arithmetic sources and common headers in this directory were selected
from ACK's `mach/proto/fp`; `byte_order.h` came from `mach/m68k2/libfp`.
The complete upstream ACK revision is preserved in repository commit 1.

`Makefile.libfp` compiles the portable C sources with the active 68kunix ACK
pipeline. `symbols.sed` performs the historical symbol conversion from C names
such as `_adf8` to the backend runtime names such as `.adf8`.

The original `_fptrp` was written in EM. The active toolchain does not build EM
source directly, so `fptrp.c` maps floating exceptions to the V7 `SIGFPE`
mechanism instead.
