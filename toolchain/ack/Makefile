# Host-side ACK cross-toolchain and 68kunix runtime.

ACK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(ACK_DIR)/../..)
BIN_DIR ?= $(PROJECT_DIR)/build/bin
RUNTIME_DIR ?= $(PROJECT_DIR)/build/toolchain/ack/68kunix
V7_INCLUDE_DIR ?= $(PROJECT_DIR)/unix/v7/usr/include

.DEFAULT_GOAL := all

.PHONY: all clean clean-compiler clean-runtime

all:
	@$(MAKE) --no-print-directory -f Makefile.clang
	@mkdir -p "$(BIN_DIR)"
	@clang -std=c89 -Wno-deprecated-declarations \
		-DCC_TOOL_DIR='"$(BIN_DIR)"' \
		-DCC_RUNTIME_DIR='"$(RUNTIME_DIR)"' \
		-DCC_INCLUDE_DIR='"$(V7_INCLUDE_DIR)"' \
		-DCC_CRT_OBJECT='"libc_crt0.o"' -DCC_END_OBJECT='"libc_end.o"' \
		-DCC_MAXARG=2048 -DCC_WAIT_STATUS=int \
		-o "$(BIN_DIR)/cc" cc/ccdriver.c
	@cp -f cc/acklevels "$(BIN_DIR)/acklevels"
	@chmod 0755 "$(BIN_DIR)/cc" "$(BIN_DIR)/acklevels"
	@$(MAKE) --no-print-directory -f Makefile.libc
	@$(MAKE) --no-print-directory -f Makefile.libfp

clean-compiler:
	@$(MAKE) --no-print-directory -f Makefile.clang clean
	@rm -f "$(BIN_DIR)/cc" "$(BIN_DIR)/acklevels"

clean-runtime:
	@$(MAKE) --no-print-directory -f Makefile.libc clean
	@$(MAKE) --no-print-directory -f Makefile.libfp clean

clean: clean-compiler clean-runtime
