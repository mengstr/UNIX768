# Architecture

UNIX768 runs an ACK-built, 68000 port of UNIX V7 on the Epoch68 machine and
emulator. The port keeps the V7 kernel model where practical, while making
the 32-bit address and memory-management boundaries explicit.

## Machine model

The 68000 runs nominally at 12 MHz with 32 MB of RAM divided into 256 KB
pages. User mode sees one 256 KB page at `0x000000–0x03ffff`. Supervisor
mode maps the kernel page at that address and can use additional page windows:

| Range | Purpose |
| --- | --- |
| `0x000000–0x03ffff` | kernel/system page |
| `0x200000–0x23ffff` | first user-page mirror |
| `0x400000–0x43ffff` | second user-page mirror |
| `0x600000–0x7fffff` | disk-page window |
| `0x800000–0x80ffff` | boot ROM |
| `0xa00000–0xa0ffff` | MMIO |
| `0xc00000–0xc3ffff` | supervisor-only kernel metadata (physical page 63) |

MMIO holds the page-window latches and a four-channel 16550-style UART at
`0xa00100`. In supervisor mode Y0 is always physical page 0. The kernel uses
MMIO to select user pages in the two mirror windows and the active disk page;
this gives it access beyond the fixed system page. At reset a ROM-overlay
flip-flop disables normal decoding and mirrors the ROM across the CPU address
space. The first write clears the overlay and restores the normal map.

The protection boundary is enforced by address decoding: user code may access
only its Y0 page, apart from the narrow UART access required by the platform.
It cannot access page mirrors, page-control latches, timers, or kernel
metadata. The Y6 metadata window is fixed to physical page 63; it is neither
selected by a latch nor reached through a user-page mirror.

## Kernel and process model

The active kernel is the focused V7 set under `unix/v7/usr/sys/{sys,dev,conf}`
and is linked with ACK tools. It retains V7 process states and the historical
`proc`/`xproc` lifecycle, but each live user process owns one physical 256 KB
user page.

- `proc[]`, `uarea[]`, and kernel stacks are fixed supervisor-only arenas in
  the Y6/page-63 window at `0xc00000`.
- `p_addr` identifies the process's physical user page; it is not a process
  table index.
- `fork()` allocates a page, copies the parent image, initializes the child,
  and makes it runnable only after its saved context is complete.
- `exit()` releases the user page, then makes the process a zombie. The
  process-table, uarea, and kernel-stack slots remain until `wait()` reaps it.
- `swtch()` installs the selected process's uarea, page registers, and saved
  user context as one interrupt-protected operation.

Swapping and variable-sized physical-core allocation are intentionally not
implemented. A user process has one fixed page; `brk()` must remain within it.

## ABI and system calls

The V7 syscall numbering and kernel table remain visible. The 68000 trap path
marshals the implemented calls and ACK libc supplies their user-side veneers.
Pointers, offsets, times, and byte counts are 32-bit; V7 logical identifiers
such as file descriptors, pids, uid/gid values, modes, signals, and device
selectors remain small values.

The syscall interface should preserve V7 flow and names while widening only
values that carry an address, length, offset, or time. Unsupported historical
slots stay explicit rather than being silently redirected. Current coverage is
generated from the implementation in [syscalls.md](syscalls.md).

## Shared headers and filesystem format

`unix/v7/usr/sys/h/` is the canonical source of low-level shared definitions.
Userland headers under `usr/include/sys/` re-export it, with short `*_str.h`
fragments for layouts shared with otherwise kernel-private declarations.

Fixed-width types use 32-bit `long`, not host-dependent `int`. Compile-time
assertions protect the important cross-component contracts, including V7
filesystem layouts: `dinode` is 64 bytes, `direct` is 16 bytes, and `filsys`
is 512 bytes. These checks keep the kernel, filesystem utilities, and monitor
from silently disagreeing about on-disk data.

## Memory safety diagnostics

The flat user page has an upward-growing heap and a downward-growing stack.
`_brk`/`_sbrk` and the syscall trap path retain a 4096-byte guard between them.
A collision is reported once per process and should be treated as memory
corruption risk, not as an application-level failure to work around.
