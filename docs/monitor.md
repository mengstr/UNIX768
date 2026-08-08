# ROM monitor

The Epoch68 ROM monitor is the small standalone environment that runs before
UNIX. It initializes the machine, provides enough V7 filesystem access to
create and populate a disk, receives files and loader scripts over YMODEM, and
loads either the kernel or a standalone user program. It is deliberately not a
second operating system: once UNIX is usable, normal administration belongs in
UNIX.

The implementation is under `platform/epoch68/firmware/`. The command parser
is in `monitor_cmd.c`; standalone V7 filesystem operations are in `v7/` and
`v7io.c`; and `uart.c`, `disk.c`, `rd.c`, and `sd.c` form the device boundary.

## Startup and machine state

At reset, hardware mirrors ROM across the CPU address space so the 68000 reads
its initial stack pointer and PC from the ROM image. The first write made by
`crt0.asm` clears that overlay. Startup then masks interrupts, copies the ROM
vector table to physical RAM page 0, initializes writable data and BSS, and
enters `main()` in supervisor mode.

The monitor initializes the MMIO page latches, uses UART0 for its `@` console,
and initializes UART1 for transfers. Its initial filesystem is `rd(0,0)` and
its initial directory is `/`. Paths may be absolute or relative to the current
monitor directory. The standalone device table currently recognizes `rd` for
the memory-backed disk and `sd` for the serial disk.

## Commands

`help` or `?` prints the built-in command summary. Numeric user and group IDs
are decimal; modes are octal.

| Purpose | Commands |
| --- | --- |
| Select or create storage | `attach <devspec>`, `mkfs <devspec> <blocks>` |
| Create and remove names | `mknod <path> <c\|b> <major> <minor>`, `mkdir <path> [mode]`, `rmdir <path>`, `rm <path>`, `ln <old> <new>` |
| Ownership and modes | `chown <uid> <path>`, `chgrp <gid> <path>`, `chmod <mode> <path>` |
| Navigate and inspect | `pwd`, `cd <path>`, `ls [path]`, `cat <path>`, `hd <path>` |
| Copy and move | `cp <src> <dst>`, `mv <src> <dst>` |
| Receive data | `rx [path] [mode]`, `rxscript [name]`, `. <name ...>` |
| Execute an image | `boot [path] [-m]`, `user <path>` |

`attach` probes the root directory before changing the active device and
resets the current directory to `/`. `mkdir` defaults to mode `0755`.

`rx` receives a file over YMODEM on UART1. With an explicit path it stores the
payload there and may apply an octal mode; without a path it uses the name sent
by the host. `rxscript` receives one named host script into RAM and executes
it. The `.` command can request and execute additional scripts in order.
Received scripts are not first written to the V7 filesystem.

Monitor scripts contain one command per line. Blank lines and lines beginning
with `#` are ignored, commands are echoed with a `+` prefix, and execution stops
on the first failed command. Scripted `mkdir` and `ln` operations are made
restart-friendly when their destination already exists.

`boot` defaults to `/unix`. It validates a big-endian 68000 ZMAGIC image, loads
it through the user-page mirror, clears BSS, disables timer and UART interrupts,
and transfers control to the kernel entry point. `-m` sets the boot flag that
makes the kernel start multi-user; without it the kernel starts single-user.
`user` loads a compatible standalone executable into a user page and enters it
in user mode, mainly for bring-up tests.

## Bootstrap role

The normal monitor entry script is `unix/bootstrap/monitor`. It creates the minimum
filesystem, transfers the kernel and compiler seed, and boots UNIX. The
installed `/dist/bootstrap`, sourced from `unix/bootstrap/system`, then runs
under `osh`, receives the split source archives, and rebuilds and installs the
rest of the system inside V7.

This division keeps the ROM side small. The monitor only needs device nodes,
basic filesystem mutation, YMODEM, image loading, and enough scripting to
assemble the initial disk. Shell semantics and package builds remain outside
the firmware.

`make firmware` builds the 64 KiB ROM as `build/firmware/bootrom.bin`. The
normal emulator targets consume that image; the host build copies the monitor
loader and V7 bootstrap scripts into the generated filesystem payload.

## Porting to real hardware

Most of the monitor is board-independent. Preserve the command layer,
`v7io.c`, the code under `v7/`, and the YMODEM implementation where possible.
Adapt the hardware beneath these interfaces.

The emulator-specific assumptions are:

- ROM is linked at `0x800000`, is mirrored across the address space at reset,
  and the first write restores normal address decoding.
- MMIO starts at `0xa00000`; the UART block starts at `0xa00100` with an
  eight-byte stride between channels.
- The memory-backed disk is exposed through a 256 KiB window at `0x600000`,
  selected by MMIO page latch 3.
- UART0 is the console. UART1 is shared by YMODEM reception and the current
  serial-disk protocol, so those functions are used at different times.
- Emulator trace and debug service opcodes are optional diagnostics, not
  required monitor services.

For a board with IDE, CompactFlash, SD/SPI, flash, SRAM, or a DMA controller,
replace the implementations of `epoch68_disk_read()` and
`epoch68_disk_write()` while retaining their sector interface. Keep media
persistence, flushing, and wear management below that boundary. If the board
has only one UART, console input must be suspended or multiplexed while YMODEM
or the serial disk owns it.

A practical bring-up order is:

1. Reach `main()` from the reset vectors and establish writable RAM and a safe
   supervisor stack.
2. Make UART0 transmit and receive reliable.
3. Verify raw 512-byte block reads, then writes, without involving the V7
   filesystem.
4. Use `attach` and `ls` on a known image.
5. Run `mkfs` only on a scratch device and verify the result after reset.
6. Bring up the transfer UART and receive one file with `rx`.
7. Load a standalone test with `user`, then boot UNIX.

Early fault handling intentionally remains simple: unexpected exceptions stop
in the ROM handler. During board bring-up, use the existing trace controls,
`DEBUG_SL()` for addresses and sizes, and `DEBUG_DUMP512()` at sector and image
transfer boundaries before adding new diagnostic infrastructure. Once raw UART
and storage I/O are stable, board-specific reset or fault-reporting behavior
can be added without disturbing the filesystem and command layers.
