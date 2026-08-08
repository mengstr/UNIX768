# UNIX768

UNIX768 is a port of UNIX Seventh Edition to a Motorola 68000 system. The
repository contains the patched V7 userland and kernel, the modified Amsterdam
Compiler Kit toolchain, Epoch68 firmware and emulator, host utilities, adopted
guest ports, tests, and bootstrap scripts needed to build and boot the
system.

## Getting started

Build the complete system:

```sh
make clean all
```

Start the UART bridge, then keep the automatic YMODEM sender running in a
terminal:

```sh
build/bin/uartbridge -n 6 -public-base 24000 -backend-base 23000
build/bin/ysend -auto
```

The sender serves `rx` and `rxscript` requests from `build/ramdisk` on UART1
and uses the preserved root-level `serialdisk.img` for serial-disk requests.
In another terminal, start the emulator:

```sh
make run
```

Connect to the console with `nc 127.0.0.1 24000`. For a new disk, enter
`rxscript bootstrap` at the monitor prompt to create the seed filesystem and
boot single-user. The current system documentation starts with
[`docs/software.md`](docs/software.md); monitor commands and bootstrap details
are in [`docs/monitor.md`](docs/monitor.md).

## Repository layout

- `unix/`: patched V7 sources, guest ports, and bootstrap scripts
- `platform/epoch68/`: ROM firmware and host emulator
- `toolchain/ack/`: the modified ACK compiler and runtime
- `host/`: project-owned host commands, scripts, and tools
- `third_party/`: separately sourced dependencies kept outside project code
- `docs/`: concise current system documentation
- `build/`: ignored generated output

The root Makefile is the supported entry point. Run `make help` for its
canonical targets.

## Provenance and licensing

Project-authored material is licensed under the [BSD-2-Clause `LICENSE`](LICENSE).
Source origins, imported revisions, copyright notices, and applicable
third-party license files are recorded in [`THIRD_PARTY.md`](THIRD_PARTY.md)
and [`LICENSES/`](LICENSES).
