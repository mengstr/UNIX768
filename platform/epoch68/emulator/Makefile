# platform/epoch68/emulator emulator host build.

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(MAKEFILE_DIR)/../../..)
EPOCH68_DIR := $(patsubst %/,%,$(MAKEFILE_DIR))

BUILD_DIR ?= $(PROJECT_DIR)/build/user
BIN_DIR ?= $(PROJECT_DIR)/build/bin
TOOLS_DIR ?= $(BUILD_DIR)/tools
MUSASHI_DIR ?= $(PROJECT_DIR)/third_party/musashi
MUSASHI_CFLAGS ?= -O3 -Wall -Wextra -pedantic -g -fno-common -Wno-unused-variable -DM68K_INSTRUCTION_HOOK=1 -DM68K_ILLG_HAS_CALLBACK=1 -DM68K_TRAP_HAS_CALLBACK=1 -DM68K_EMULATE_FC=1
EPOCH68 ?= $(BIN_DIR)/Epoch68
MUSASHI_BUILD_DIR ?= $(BUILD_DIR)/vendor/musashi
YSEND ?= $(BIN_DIR)/ysend
GO ?= go
GOCACHE ?= $(TOOLS_DIR)/go-build-cache

.DEFAULT_GOAL := epoch68

.PHONY: help epoch68 ysend musashi clean

define announce
	@printf '%-6s %s\n' '$(1)' '$(2)'
endef

relpath = $(patsubst $(PROJECT_DIR)/%,%,$(1))

help:
	@printf '%s\n' 'platform/epoch68/emulator targets:'
	@printf '  %-10s %s\n' 'musashi' 'build the vendored Musashi library'
	@printf '  %-10s %s\n' 'epoch68' 'build the platform/epoch68/emulator emulator'
	@printf '  %-10s %s\n' 'ysend' 'build the TCP YMODEM sender helper'
	@printf '  %-10s %s\n' 'clean' 'remove build/user and build/bin binaries'

epoch68: musashi $(EPOCH68)
ysend: $(YSEND)

musashi: $(TOOLS_DIR)/lib/libmusashi.a $(TOOLS_DIR)/include/musashi/m68k.h

$(TOOLS_DIR)/lib/libmusashi.a: \
	$(MUSASHI_DIR)/m68kcpu.c \
	$(MUSASHI_DIR)/m68kdasm.c \
	$(MUSASHI_DIR)/m68kops.c \
	$(MUSASHI_DIR)/m68k_in.c \
	$(MUSASHI_DIR)/softfloat/softfloat.c \
	$(MAKEFILE_DIR)/Makefile | $(BUILD_DIR) $(TOOLS_DIR)
	@mkdir -p "$(TOOLS_DIR)/include/musashi" "$(TOOLS_DIR)/include/musashi/softfloat" "$(TOOLS_DIR)/lib"
	$(call announce,MAKE,third_party/musashi)
	@rm -rf "$(MUSASHI_BUILD_DIR)"
	@mkdir -p "$(dir $(MUSASHI_BUILD_DIR))"
	@cp -R "$(MUSASHI_DIR)" "$(MUSASHI_BUILD_DIR)"
	@cd "$(MUSASHI_BUILD_DIR)" && $(MAKE) --no-print-directory -s -B CFLAGS="$(MUSASHI_CFLAGS)"
	$(call announce,AR,$(call relpath,$(TOOLS_DIR)/lib/libmusashi.a))
	@ar rcs "$(TOOLS_DIR)/lib/libmusashi.a" "$(MUSASHI_BUILD_DIR)/m68kcpu.o" "$(MUSASHI_BUILD_DIR)/m68kdasm.o" "$(MUSASHI_BUILD_DIR)/m68kops.o" "$(MUSASHI_BUILD_DIR)/softfloat/softfloat.o"
	$(call announce,INSTALL,$(call relpath,$(TOOLS_DIR)/include/musashi))
	@cp -f "$(MUSASHI_DIR)/m68k.h" "$(MUSASHI_DIR)/m68kconf.h" "$(MUSASHI_DIR)/m68kcpu.h" "$(MUSASHI_DIR)/m68kmmu.h" "$(MUSASHI_DIR)/m68kops.h" "$(TOOLS_DIR)/include/musashi/"
	@cp -f "$(MUSASHI_DIR)/softfloat/softfloat.h" "$(TOOLS_DIR)/include/musashi/softfloat/"

$(TOOLS_DIR)/include/musashi/m68k.h: $(TOOLS_DIR)/lib/libmusashi.a
	@:

$(EPOCH68): $(wildcard $(EPOCH68_DIR)/cmd/epoch68/*.go) $(wildcard $(EPOCH68_DIR)/cmd/epoch68/*.[ch]) $(wildcard $(EPOCH68_DIR)/*.mod) $(PROJECT_DIR)/third_party/miniaudio/miniaudio.h $(TOOLS_DIR)/lib/libmusashi.a $(TOOLS_DIR)/include/musashi/m68k.h $(MAKEFILE_DIR)/Makefile | $(BUILD_DIR) $(BIN_DIR)
	$(call announce,GO,$(call relpath,$@))
	@cd $(EPOCH68_DIR) && CGO_LDFLAGS_ALLOW='-Wl,.*' GOCACHE=$(GOCACHE) $(GO) build -a -o $@ ./cmd/epoch68

$(YSEND): $(wildcard $(EPOCH68_DIR)/cmd/ysend/*.go) $(wildcard $(EPOCH68_DIR)/*.mod) | $(BUILD_DIR) $(BIN_DIR)
	$(call announce,GO,$(call relpath,$@))
	@cd $(EPOCH68_DIR) && GOCACHE=$(GOCACHE) $(GO) build -o $@ ./cmd/ysend

$(BUILD_DIR) $(BIN_DIR) $(TOOLS_DIR):
	@mkdir -p $@

clean:
	$(call announce,CLEAN,$(call relpath,$(EPOCH68)))
	$(call announce,CLEAN,$(call relpath,$(YSEND)))
	@rm -f "$(EPOCH68)" "$(YSEND)"
