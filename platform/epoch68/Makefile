# Epoch68 host-platform support.

PLATFORM_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(PLATFORM_DIR)/../..)
BIN_DIR ?= $(PROJECT_DIR)/build/bin
GO ?= go
HOST_CC ?= cc

.PHONY: ackimage bootstrap-tools monitor emulator generators unix

ackimage:
	@mkdir -p "$(BIN_DIR)"
	@cd "$(PROJECT_DIR)" && GO111MODULE=off $(GO) build -o "$(BIN_DIR)/ackimage" ./host/cmd/ackimage
	@chmod 0755 "$(BIN_DIR)/ackimage"

bootstrap-tools:
	@mkdir -p "$(BIN_DIR)"
	@cd "$(PROJECT_DIR)" && GO111MODULE=off $(GO) build -o "$(BIN_DIR)/v7tar" ./host/cmd/v7tar
	@$(HOST_CC) -std=gnu89 -w -o "$(BIN_DIR)/host-pack" \
		"$(PROJECT_DIR)/unix/v7/usr/src/cmd/pack/pack.c"
	@chmod 0755 "$(BIN_DIR)/v7tar" "$(BIN_DIR)/host-pack"

monitor: ackimage
	@$(MAKE) --no-print-directory -C firmware firmware

emulator:
	@$(MAKE) --no-print-directory -C emulator epoch68 ysend

generators:
	@$(MAKE) --no-print-directory -f generators.mk

unix:
	@$(MAKE) --no-print-directory -f host-bootstrap.mk build
