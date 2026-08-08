# platform/epoch68/emulator ROM monitor build
#
# This file owns only the firmware/monitor image build.

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(MAKEFILE_DIR)/../../..)
FIRMWARE_DIR := $(patsubst %/,%,$(MAKEFILE_DIR))

BIN_DIR ?= $(PROJECT_DIR)/build/bin

CC = $(BIN_DIR)/cc
AS = $(BIN_DIR)/as
LED ?= $(BIN_DIR)/led
ACKIMAGE ?= $(BIN_DIR)/ackimage

BUILD_DIR ?= $(PROJECT_DIR)/build
FIRMWARE_BUILD_DIR ?= $(BUILD_DIR)/firmware
FIRMWARE_OBJ_DIR ?= $(FIRMWARE_BUILD_DIR)/obj
FIRMWARE_ROM ?= $(FIRMWARE_BUILD_DIR)/bootrom.bin
FIRMWARE_OUT ?= $(FIRMWARE_BUILD_DIR)/bootrom.out
RUNTIME_LIBC_A ?= $(PROJECT_DIR)/build/toolchain/ack/68kunix/libc.a
RUNTIME_LIBC_END_O ?= $(PROJECT_DIR)/build/toolchain/ack/68kunix/libc_end.o

V7_ROOT := $(PROJECT_DIR)/unix/v7/usr/src
LIBC_GEN_DIR := $(V7_ROOT)/libc/gen

SRC_DIRS := \
	$(FIRMWARE_DIR) \
	$(FIRMWARE_DIR)/v7 \
	$(LIBC_GEN_DIR)

SRCS_C := \
	disk.c \
	uart.c \
	main.c \
	monitor_cmd.c \
	path.c \
	pager.c \
	string.c \
	ymodem.c \
	attach.c \
	boot.c \
	cat.c \
	cp.c \
	hd.c \
	ls.c \
	mkfs.c \
	mv.c \
	rm.c \
	rmdir.c \
	v7fs.c \
	v7io.c \
	prf.c \
	conf.c \
	rd.c \
	sd.c

SRCS_ASM := \
	crt0.s \
	handoff.s

FIRMWARE_C_OBJS := $(addprefix $(FIRMWARE_OBJ_DIR)/,$(SRCS_C:.c=.o))
FIRMWARE_ASM_OBJS := $(addprefix $(FIRMWARE_OBJ_DIR)/,$(SRCS_ASM:.s=.o))
FIRMWARE_OBJS := $(FIRMWARE_ASM_OBJS) $(FIRMWARE_C_OBJS)

FIRMWARE_CPPFLAGS ?= \
	-I$(FIRMWARE_DIR) \
	-I$(FIRMWARE_DIR)/v7 \
	-I$(PROJECT_DIR)/unix/v7/usr/sys/h \
	-I$(PROJECT_DIR)/unix/v7/usr/include
FIRMWARE_CFLAGS ?= -D__ACK__ $(FIRMWARE_CPPFLAGS)
FIRMWARE_LEDFLAGS ?= -a0:4 -a1:4 -a2:4 -a3:4 -b0:8388608 -b2:196608

vpath %.c $(SRC_DIRS)
vpath %.s $(SRC_DIRS)

.DEFAULT_GOAL := rebuild

.PHONY: help firmware rebuild clean

define announce
	@printf '%-6s %s\n' '$(1)' '$(2)'
endef

relpath = $(patsubst $(PROJECT_DIR)/%,%,$(1))

define summarize_firmware
	@size=$$(wc -c < "$@" | tr -d '[:space:]'); \
	printf 'DONE   %s size=%s\n' "$(call relpath,$@)" "$$size"
endef

help:
	@printf '%s\n' 'platform/epoch68/emulator ROM monitor targets:'
	@printf '  %-10s %s\n' 'firmware' 'compile, assemble, and link the ROM monitor'
	@printf '  %-10s %s\n' 'rebuild' 'clean and rebuild the ROM monitor'
	@printf '  %-10s %s\n' 'clean' 'remove monitor build outputs'

firmware: $(FIRMWARE_ROM)

rebuild: clean firmware

$(FIRMWARE_OUT): $(FIRMWARE_OBJS) $(RUNTIME_LIBC_A) $(RUNTIME_LIBC_END_O) | $(FIRMWARE_BUILD_DIR)
	$(call announce,LD,$(call relpath,$@))
	@$(LED) $(FIRMWARE_LEDFLAGS) -o $@ $(FIRMWARE_OBJS) $(RUNTIME_LIBC_A) $(RUNTIME_LIBC_A) $(RUNTIME_LIBC_END_O)

$(FIRMWARE_ROM): $(FIRMWARE_OUT) | $(FIRMWARE_BUILD_DIR)
	$(call announce,ROM,$(call relpath,$@))
	@$(ACKIMAGE) $(FIRMWARE_OUT) $@
	$(summarize_firmware)

$(FIRMWARE_OBJ_DIR)/%.o: %.s | $(FIRMWARE_OBJ_DIR)
	$(call announce,AS,$(call relpath,$<))
	@$(AS) - -o $@ $<

$(FIRMWARE_OBJ_DIR)/%.o: %.c | $(FIRMWARE_OBJ_DIR)
	$(call announce,CC,$(call relpath,$<))
	@$(CC) $(FIRMWARE_CFLAGS) -c -o $@ $<

$(FIRMWARE_BUILD_DIR) $(FIRMWARE_OBJ_DIR):
	@mkdir -p $@

clean:
	$(call announce,CLEAN,$(call relpath,$(FIRMWARE_BUILD_DIR)))
	@rm -rf "$(FIRMWARE_BUILD_DIR)"
