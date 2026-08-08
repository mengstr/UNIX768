# Software

UNIX768 is a self-hosting UNIX V7 system for the Epoch68 68000 platform. The
host build produces the emulator, ROM monitor, initial UNIX image, and source
archives; the installed V7 system can then rebuild its userland, ACK toolchain,
and kernel.

## System layers

| Layer | Role |
| --- | --- |
| Epoch68 emulator | CPU, paged RAM, UARTs, timer, RTC, RAM disk, and serial disk |
| ROM monitor | Filesystem creation, loading, boot control, and YMODEM transfer |
| UNIX V7 kernel | Processes, filesystems, tty handling, signals, accounting, and device drivers |
| ACK toolchain | Native C compiler, assembler, linker, object tools, and runtime libraries |
| V7 userland | Shells, system commands, development tools, text processing, mail, UUCP, games, editors, manuals, and documents |

The active kernel and its memory model are summarized in
[architecture.md](architecture.md). The pre-UNIX loader and its hardware-port
boundary are described in [monitor.md](monitor.md), and the generated syscall
interface is in [syscalls.md](syscalls.md).

## Boot and installation

The monitor installs a small single-user seed containing the kernel, recovery
shell, transfer tools, and native compiler. `/dist/bootstrap` then receives
packed source groups, builds them inside V7, installs the results, and removes
intermediate files. The kernel build installs `/unix.new` and
`/etc/ksyms.new` as a matched pair so the running kernel is not replaced
underneath active processes.

The source groups are deliberately separate to fit the guest filesystem:

| Group | Contents |
| --- | --- |
| base | init, shells, core commands, and filesystem utilities |
| cmd | general commands, sed, and Bourne shell |
| tools | yacc, lex, make, awk, m4, ar/ranlib, and object utilities |
| comm | mail, calendar, cron/at, and UUCP |
| text | cawf/nroff, man, eqn/neqn, tbl, spell, refer, prep, and ee |
| games | portable V7 games and their data |
| dev | ACK, 68kasm, srec2aout, and mksdroot |
| kernel | kernel sources and build/install/clean scripts |

Manual pages are installed last. The full document collection is available
through an optional installer transferred at the end of the bootstrap.

## Native development

The installed compiler is the ACK-based `/bin/cc`; its private passes live
under `/usr/lib/ack`, while conventional assembler and linker entry points are
available as `/bin/as` and `/bin/ld`. Source builds normally leave objects and
executables beside their sources and use one build, install, and clean script
per source group. This matches the small V7 environment and keeps the build
procedure inspectable from inside the guest.

The kernel follows the same model with `build.kernel`, `install.kernel`, and
`clean.kernel`. Kernel assembly uses ACK syntax, and `/etc/ksyms` supplies the
symbol information used by commands such as `ps`.

## Runtime services

Multi-user startup uses V7 `init`, getty/login, `/bin/sh`, and `/etc/rc`.
The platform provides console and terminal UARTs, a YMODEM transfer channel,
RAM-disk and serial-disk block devices, RTC time, `/dev/mem`, `/dev/kmem`, and
`/dev/null`. UUCP uses its dedicated tty and can exchange mail through an
external gateway. Its V7, Alpine, hardware, and Internet-mail setup is
described in [uucp.md](uucp.md).

Historical PDP-11 compilers, assembly sources, hardware drivers, swapping, and
programs tied to unavailable peripherals are not active parts of the port.
