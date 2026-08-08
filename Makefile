# UNIX768 build entry points. The UNIX build projects the canonical V7 scripts.

.DEFAULT_GOAL := help

BIN_DIR := $(CURDIR)/build/bin
MONITOR_ROM := $(CURDIR)/build/firmware/bootrom.bin
RUN_DISK := $(CURDIR)/ramdisk.img
EPOCH68_ARGS ?=

.PHONY: help all host ack monitor emulator unix check-unix-host \
	uucp-tcp-gateway uucp-tcp-start uucp-tcp-stop uucp-tcp-status \
	run run12 clean

help:
	@printf '%s\n' 'UNIX768 targets:'
	@printf '  %-18s %s\n' 'all' 'build the host system and complete UNIX ramdisk'
	@printf '  %-18s %s\n' 'host' 'build ACK, monitor ROM, emulator, and host support tools'
	@printf '  %-18s %s\n' 'ack' 'build the ACK cross-toolchain and target runtime'
	@printf '  %-18s %s\n' 'monitor' 'build the ROM monitor'
	@printf '  %-18s %s\n' 'emulator' 'build Epoch68 and the YMODEM sender'
	@printf '  %-18s %s\n' 'unix' 'build the complete V7 filesystem payload from canonical scripts'
	@printf '  %-18s %s\n' 'uucp-tcp-gateway' 'build the UART5-to-TCP UUCP gateway'
	@printf '  %-18s %s\n' 'uucp-tcp-start' 'start the UART5-to-TCP UUCP gateway'
	@printf '  %-18s %s\n' 'uucp-tcp-stop' 'stop the UART5-to-TCP UUCP gateway'
	@printf '  %-18s %s\n' 'uucp-tcp-status' 'show the UART5-to-TCP UUCP gateway status'
	@printf '  %-18s %s\n' 'run' 'run the emulator at full speed'
	@printf '  %-18s %s\n' 'run12' 'run the emulator throttled to 12 MHz'
	@printf '  %-18s %s\n' 'clean' 'remove build/'

all: host
	@$(MAKE) --no-print-directory unix

host: ack
	@$(MAKE) --no-print-directory -C platform/epoch68 monitor
	@$(MAKE) --no-print-directory -C platform/epoch68 emulator
	@$(MAKE) --no-print-directory -C platform/epoch68 bootstrap-tools
	@$(MAKE) --no-print-directory -C platform/epoch68 generators
	@$(MAKE) --no-print-directory -C host

ack:
	@$(MAKE) --no-print-directory -C toolchain/ack

monitor: ack
	@$(MAKE) --no-print-directory -C platform/epoch68 monitor

emulator:
	@$(MAKE) --no-print-directory -C platform/epoch68 emulator

unix: check-unix-host
	@$(MAKE) --no-print-directory -C platform/epoch68 unix

uucp-tcp-gateway:
	@$(MAKE) --no-print-directory -C host uucp-tcp-gateway

uucp-tcp-start: uucp-tcp-gateway
	@host/scripts/uucp-tcp start

uucp-tcp-stop:
	@host/scripts/uucp-tcp stop

uucp-tcp-status:
	@host/scripts/uucp-tcp status

check-unix-host:
	@test -x "$(BIN_DIR)/cc" || { echo "make: ACK compiler is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/as" || { echo "make: ACK assembler is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/led" || { echo "make: ACK linker is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/cv" || { echo "make: ACK converter is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/ackimage" || { echo "make: ACK image tool is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/v7tar" || { echo "make: V7 archive tool is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/host-pack" || { echo "make: host pack tool is missing; run 'make host' first" >&2; exit 1; }
	@test -f "$(CURDIR)/build/toolchain/ack/68kunix/libc.a" || { echo "make: ACK target libc is missing; run 'make host' first" >&2; exit 1; }
	@test -f "$(CURDIR)/build/toolchain/ack/68kunix/libm.a" || { echo "make: ACK target libm is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/yacc" || { echo "make: host yacc is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/lex" || { echo "make: host lex is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(BIN_DIR)/awk-proc" || { echo "make: host AWK table generator is missing; run 'make host' first" >&2; exit 1; }

run: RUN_SPEED_ARGS := -cpu-hz 12000000 -max-cpu-hz 0
run12: RUN_SPEED_ARGS := -cpu-hz 12000000 -max-cpu-hz 12000000

run run12:
	@test -x "$(BIN_DIR)/Epoch68" || { echo "make: emulator is missing; run 'make emulator' first" >&2; exit 1; }
	@test -f "$(MONITOR_ROM)" || { echo "make: monitor ROM is missing; run 'make monitor' first" >&2; exit 1; }
	@mkdir -p "$(dir $(RUN_DISK))"
	@"$(BIN_DIR)/Epoch68" \
		-rom "$(MONITOR_ROM)" \
		-disk "$(RUN_DISK)" \
		$(EPOCH68_ARGS) \
		$(RUN_SPEED_ARGS)

clean:
	@rm -rf build
