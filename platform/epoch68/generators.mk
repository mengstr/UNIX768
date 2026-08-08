# Native generators needed while projecting the V7 build scripts on the host.

PLATFORM_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(PLATFORM_DIR)/../..)
V7_ROOT := $(PROJECT_DIR)/unix/v7
CMD_DIR := $(V7_ROOT)/usr/src/cmd
BUILD_DIR ?= $(PROJECT_DIR)/build/platform/epoch68/generators
BIN_DIR ?= $(PROJECT_DIR)/build/bin
HOST_CC ?= cc

YACC_DIR := $(CMD_DIR)/yacc
YACC_SRCS := $(addprefix $(YACC_DIR)/,y1.c y2.c y3.c y4.c)
YACCPAR := $(V7_ROOT)/usr/lib/yaccpar
HOST_YACC := $(BIN_DIR)/yacc

LEX_DIR := $(CMD_DIR)/lex
LEX_SRCS := $(addprefix $(LEX_DIR)/,lmain.c sub1.c sub2.c header.c)
LEX_PARSER := $(BUILD_DIR)/lex/y.tab.c
LEX_NCFORM := $(V7_ROOT)/usr/lib/lex/ncform
HOST_LEX := $(BIN_DIR)/lex

AWK_DIR := $(CMD_DIR)/awk
AWK_GEN_DIR := $(BUILD_DIR)/awk
AWK_GRAMMAR_C := $(AWK_GEN_DIR)/awk.g.c
AWK_HEADER := $(AWK_GEN_DIR)/awk.h
AWK_STAMP := $(AWK_GEN_DIR)/generated.stamp
HOST_AWK_PROC := $(BIN_DIR)/awk-proc

.DEFAULT_GOAL := all
.PHONY: all clean

all: $(HOST_YACC) $(HOST_LEX) $(HOST_AWK_PROC)

$(BIN_DIR) $(BUILD_DIR)/lex $(AWK_GEN_DIR):
	@mkdir -p $@

$(HOST_YACC): $(YACC_SRCS) $(YACC_DIR)/dextern $(YACC_DIR)/files $(YACCPAR) | $(BIN_DIR)
	@echo HOSTCC yacc
	@$(HOST_CC) -std=gnu89 -w -Di32=int \
		'-DPARSER="$(YACCPAR)"' -I$(YACC_DIR) -o $@ $(YACC_SRCS)

$(LEX_PARSER): $(LEX_DIR)/parser.y $(HOST_YACC) | $(BUILD_DIR)/lex
	@echo YACC lex/parser.y
	@cd $(BUILD_DIR)/lex && $(HOST_YACC) $(LEX_DIR)/parser.y

$(HOST_LEX): $(LEX_SRCS) $(LEX_DIR)/ldefs.c $(LEX_DIR)/once.c $(LEX_PARSER) $(LEX_NCFORM) | $(BIN_DIR)
	@echo HOSTCC lex
	@$(HOST_CC) -std=gnu89 -w -include stdint.h -Di32=intptr_t -Dunix \
		'-DLEX_NCFORM="$(LEX_NCFORM)"' -I$(LEX_DIR) \
		-o $@ $(LEX_SRCS) $(LEX_PARSER)

$(AWK_STAMP): $(AWK_DIR)/awk.g.y $(AWK_DIR)/awk.def $(HOST_YACC) | $(AWK_GEN_DIR)
	@echo YACC awk/awk.g.y
	@cd $(AWK_GEN_DIR) && $(HOST_YACC) -d $(AWK_DIR)/awk.g.y
	@mv $(AWK_GEN_DIR)/y.tab.c $(AWK_GRAMMAR_C)
	@mv $(AWK_GEN_DIR)/y.tab.h $(AWK_HEADER)
	@touch $@

$(AWK_GRAMMAR_C) $(AWK_HEADER): $(AWK_STAMP)
	@test -f $@

$(HOST_AWK_PROC): $(AWK_DIR)/proc.c $(AWK_DIR)/token.c $(AWK_DIR)/awk.def $(AWK_HEADER) | $(BIN_DIR)
	@echo HOSTCC awk-proc
	@$(HOST_CC) -std=gnu89 -w -D_POSIX_C_SOURCE=200809L -I$(AWK_GEN_DIR) \
		-o $@ $(AWK_DIR)/proc.c $(AWK_DIR)/token.c

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(HOST_YACC) $(HOST_LEX) $(HOST_AWK_PROC)
