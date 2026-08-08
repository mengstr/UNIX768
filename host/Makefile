# UNIX768 host support tools.

HOST_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(HOST_DIR)/..)
BIN_DIR ?= $(PROJECT_DIR)/build/bin
TOOLS_DIR ?= $(PROJECT_DIR)/build/tools
GOCACHE ?= $(TOOLS_DIR)/go-build-cache
GO ?= go

UARTBRIDGE := $(BIN_DIR)/uartbridge
UUCP_TCP_GATEWAY := $(BIN_DIR)/uucp-tcp-gateway

.DEFAULT_GOAL := all
.PHONY: all uartbridge uucp-tcp-gateway clean

all: uartbridge uucp-tcp-gateway

uartbridge: $(UARTBRIDGE)

uucp-tcp-gateway: $(UUCP_TCP_GATEWAY)

$(UARTBRIDGE): $(wildcard $(HOST_DIR)/cmd/uartbridge/*.go) | $(BIN_DIR) $(TOOLS_DIR)
	@printf '%-6s %s\n' 'GO' 'build/bin/uartbridge'
	@cd "$(HOST_DIR)" && GOCACHE="$(GOCACHE)" GO111MODULE=off \
		$(GO) build -o "$@" ./cmd/uartbridge

$(UUCP_TCP_GATEWAY): $(wildcard $(HOST_DIR)/cmd/uucp-tcp-gateway/*.go) | $(BIN_DIR) $(TOOLS_DIR)
	@printf '%-6s %s\n' 'GO' 'build/bin/uucp-tcp-gateway'
	@cd "$(HOST_DIR)" && GOCACHE="$(GOCACHE)" GO111MODULE=off \
		$(GO) build -o "$@" ./cmd/uucp-tcp-gateway

$(BIN_DIR) $(TOOLS_DIR):
	@mkdir -p "$@"

clean:
	@rm -f "$(UARTBRIDGE)" "$(UUCP_TCP_GATEWAY)"
