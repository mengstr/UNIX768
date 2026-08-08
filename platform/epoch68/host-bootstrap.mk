# Host projection of the self-hosting V7 build scripts.
#
# The canonical recipes remain unix/v7/usr/src/build.*, install.*, and
# clean.*.  This makefile copies the V7 tree to a disposable root and writes
# host variants of those scripts with only target-absolute tool paths changed.
# It deliberately does not modify the checked-out V7 sources.

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR := $(abspath $(MAKEFILE_DIR)/../..)
UNIX_DIR := $(PROJECT_DIR)/unix
V7_ROOT := $(UNIX_DIR)/v7
ACK_DIR := $(PROJECT_DIR)/toolchain/ack

BUILD_DIR ?= $(PROJECT_DIR)/build
HOST_BOOTSTRAP_DIR ?= $(BUILD_DIR)/host-bootstrap
HOST_ROOT ?= $(BUILD_DIR)/ramdisk
HOST_SCRIPTS ?= $(HOST_BOOTSTRAP_DIR)/scripts
HOST_TOOL_BIN ?= $(HOST_BOOTSTRAP_DIR)/host-bin
HOST_BIN ?= $(BUILD_DIR)/bin
HOST_YACC ?= $(HOST_BIN)/yacc
HOST_LEX ?= $(HOST_BIN)/lex
HOST_AWK_PROCGEN ?= $(HOST_BIN)/awk-proc
HOST_CC ?= $(HOST_BIN)/cc
HOST_AS ?= $(HOST_BIN)/as
HOST_LED ?= $(HOST_BIN)/led
HOST_CV ?= $(HOST_BIN)/cv
HOST_NM ?= $(HOST_BIN)/ackimage
HOST_AR ?= $(HOST_BIN)/aal
HOST_PACK ?= $(HOST_BIN)/host-pack
HOST_LIBC ?= $(BUILD_DIR)/toolchain/ack/68kunix/libc.a
HOST_LIBM ?= $(BUILD_DIR)/toolchain/ack/68kunix/libm.a
HOST_SED ?= /usr/bin/sed

PACKAGES := base tools cmd comm text games dev editors kernel
SCRIPT_NAMES := $(foreach package,$(PACKAGES),build.$(package) install.$(package) clean.$(package))
HOST_SCRIPT_FILES := $(addprefix $(HOST_SCRIPTS)/,$(SCRIPT_NAMES))

.DEFAULT_GOAL := help
.PHONY: help prepare scripts generators runtime build packages loader clean $(addprefix build-,$(PACKAGES))

help:
	@printf '%s\n' 'host-bootstrap targets:'
	@printf '  %-14s %s\n' 'prepare' 'make a disposable V7 source/root tree'
	@printf '  %-14s %s\n' 'scripts' 'generate host-adapted build/install/clean scripts'
	@printf '  %-14s %s\n' 'generators' 'build host yacc and lex needed by generated sources'
	@printf '  %-14s %s\n' 'build' 'run all projected package scripts into the disposable root'
	@printf '  %-14s %s\n' 'clean' 'remove the disposable tree and generated scripts'

prepare: $(HOST_ROOT)/usr/src/.host-bootstrap-root

$(HOST_ROOT)/usr/src/.host-bootstrap-root: $(V7_ROOT) $(lastword $(MAKEFILE_LIST))
	@rm -rf "$(HOST_ROOT)"
	@mkdir -p "$(HOST_BOOTSTRAP_DIR)"
	@cp -R "$(V7_ROOT)" "$(HOST_ROOT)"
	@mkdir -p "$(HOST_ROOT)/bin" "$(HOST_ROOT)/etc" "$(HOST_ROOT)/lib" \
		"$(HOST_ROOT)/local/bin" "$(HOST_ROOT)/tmp" "$(HOST_ROOT)/usr/bin" \
		"$(HOST_ROOT)/usr/src/bin" "$(HOST_ROOT)/usr/src/lib/lex" \
		"$(HOST_ROOT)/usr/src/lib/uucp" "$(HOST_ROOT)/usr/src/ports/xargs" \
		"$(HOST_ROOT)/usr/src/cawf" "$(HOST_ROOT)/usr/src/ee" \
		"$(HOST_ROOT)/usr/src/68kasm" "$(HOST_ROOT)/usr/src/red" \
		"$(HOST_ROOT)/usr/src/games/data/lib" \
		"$(HOST_ROOT)/usr/src/games/data/quiz.k" \
		"$(HOST_ROOT)/usr/lib/cawf" "$(HOST_ROOT)/usr/lib/refer"
	@touch "$(HOST_ROOT)/etc/init" "$(HOST_ROOT)/bin/osh" \
		"$(HOST_ROOT)/bin/sh" "$(HOST_ROOT)/bin/cp"
	@cp "$(V7_ROOT)/etc/rc" "$(V7_ROOT)/etc/ttys" "$(V7_ROOT)/etc/passwd" \
		"$(V7_ROOT)/etc/group" "$(V7_ROOT)/etc/motd" "$(V7_ROOT)/etc/utmp" \
		"$(V7_ROOT)/etc/termcap" "$(HOST_ROOT)/usr/src/"
	@cp "$(UNIX_DIR)/ports/instar/instar.c" "$(HOST_ROOT)/usr/src/cmd/instar.c"
	@cp "$(UNIX_DIR)/ports/xargs/xargs.c" "$(HOST_ROOT)/usr/src/ports/xargs/xargs.c"
	@cp "$(UNIX_DIR)/ports/cawf/cawf_port.c" \
		"$(UNIX_DIR)/ports/cawf/man.mac" "$(UNIX_DIR)/ports/cawf/me.mac" \
		"$(UNIX_DIR)/ports/cawf/ms.mac" "$(UNIX_DIR)/ports/cawf/common" \
		"$(UNIX_DIR)/ports/cawf/dumb.dev" "$(UNIX_DIR)/ports/cawf/device.cf" \
		"$(UNIX_DIR)/ports/cawf/build" "$(UNIX_DIR)/ports/cawf/install" \
		"$(UNIX_DIR)/ports/cawf/clean" "$(HOST_ROOT)/usr/src/cawf/"
	@cp "$(UNIX_DIR)/ports/ee/ee.c" "$(UNIX_DIR)/ports/ee/new_curse.c" \
		"$(UNIX_DIR)/ports/ee/new_curse.h" "$(UNIX_DIR)/ports/ee/ee.1" \
		"$(UNIX_DIR)/ports/ee/build" "$(UNIX_DIR)/ports/ee/install" \
		"$(UNIX_DIR)/ports/ee/clean" "$(HOST_ROOT)/usr/src/ee/"
	@cp -R "$(UNIX_DIR)/ports/68kasm/." "$(HOST_ROOT)/usr/src/68kasm/"
	@cp -R "$(ACK_DIR)/." "$(HOST_ROOT)/usr/src/ack/"
	@cp "$(ACK_DIR)/shared/modules/src/system/setbinarymode.c" \
		"$(HOST_ROOT)/usr/src/ack/shared/modules/src/system/setbinmode.c"
	@cp "$(V7_ROOT)/usr/src/cmd/srec2aout.c" "$(HOST_ROOT)/usr/src/68kasm/srec2aout.c"
	@cp -R "$(UNIX_DIR)/ports/red/." "$(HOST_ROOT)/usr/src/red/"
	@cp "$(UNIX_DIR)/ports/banner/banner.c" "$(HOST_ROOT)/usr/src/games/banner.c"
	@cp "$(UNIX_DIR)/ports/moo/moo.c" "$(HOST_ROOT)/usr/src/games/moo.c"
	@cp "$(UNIX_DIR)/ports/maze/maze.c" "$(HOST_ROOT)/usr/src/games/maze.c"
	@cp -R "$(V7_ROOT)/usr/games/quiz.k/." "$(HOST_ROOT)/usr/src/games/data/quiz.k/"
	@cp "$(V7_ROOT)/usr/games/lib/fortunes" "$(HOST_ROOT)/usr/src/games/data/lib/fortunes"
	@cp "$(V7_ROOT)/bin/spell" "$(HOST_ROOT)/usr/src/cmd/spell/spell.sh"
	@cp "$(V7_ROOT)/etc/rc.stage3" "$(HOST_ROOT)/usr/src/rc.stage3"
	@cp "$(V7_ROOT)/usr/lib/crontab.stage3" "$(HOST_ROOT)/usr/src/crontab.stage3"
	@cp "$(V7_ROOT)/usr/lib/units" "$(HOST_ROOT)/usr/src/units"
	@cp "$(V7_ROOT)/usr/adm/wtmp" "$(HOST_ROOT)/usr/src/wtmp"
	@cp "$(V7_ROOT)/bin/diff3" "$(V7_ROOT)/bin/lorder" \
		"$(V7_ROOT)/bin/nohup" "$(V7_ROOT)/bin/true" "$(V7_ROOT)/bin/false" \
		"$(HOST_ROOT)/usr/src/bin/"
	@cp "$(V7_ROOT)/bin/calendar" "$(HOST_ROOT)/usr/src/bin/calendar"
	@cp "$(V7_ROOT)/usr/lib/crontab" "$(HOST_ROOT)/usr/src/lib/crontab"
	@cp -R "$(V7_ROOT)/usr/lib/uucp/." "$(HOST_ROOT)/usr/src/lib/uucp/"
	@cp "$(V7_ROOT)/etc/mksdroot" "$(HOST_ROOT)/usr/src/mksdroot"
	@cp "$(ACK_DIR)/shared/modules/src/object/rd.c" \
		"$(ACK_DIR)/shared/modules/src/object/rd_arhdr.c" \
		"$(ACK_DIR)/shared/modules/src/object/rd_bytes.c" \
		"$(ACK_DIR)/shared/modules/src/object/rd_int2.c" \
		"$(ACK_DIR)/shared/modules/src/object/rd_int4.c" \
		"$(ACK_DIR)/shared/modules/src/object/rd_unsig2.c" \
		"$(ACK_DIR)/shared/modules/src/object/wr_arhdr.c" \
		"$(ACK_DIR)/shared/modules/src/object/wr_bytes.c" \
		"$(ACK_DIR)/shared/modules/src/object/wr_int2.c" \
		"$(ACK_DIR)/shared/modules/src/object/wr_int4.c" \
		"$(ACK_DIR)/shared/modules/src/object/wr_ranlib.c" \
		"$(ACK_DIR)/shared/modules/src/object/object.h" \
		"$(ACK_DIR)/shared/modules/src/object/obj.h" \
		"$(HOST_ROOT)/usr/src/ar/"
	@cp "$(ACK_DIR)/shared/modules/src/system/basename.c" \
		"$(ACK_DIR)/shared/modules/src/system/system.h" \
		"$(HOST_ROOT)/usr/src/ar/"
	@cp "$(ACK_DIR)/shared/includes/ack-h/arch.h" \
		"$(ACK_DIR)/shared/includes/ack-h/out.h" \
		"$(ACK_DIR)/shared/includes/ack-h/ranlib.h" \
		"$(HOST_ROOT)/usr/src/ar/"
	@cp "$(ACK_DIR)/arch/archiver.c" "$(HOST_ROOT)/usr/src/ar/"
	@cp "$(V7_ROOT)/usr/src/cmd/ack_ar_support.c" \
		"$(HOST_ROOT)/usr/src/ar/ar_support.c"
	@cp "$(V7_ROOT)/usr/lib/yaccpar" "$(HOST_ROOT)/usr/src/lib/yaccpar"
	@cp "$(V7_ROOT)/usr/lib/lex/ncform" "$(HOST_ROOT)/usr/src/lib/lex/ncform"
	@touch "$@"

scripts: $(HOST_SCRIPT_FILES)
	@mkdir -p "$(HOST_TOOL_BIN)"
	@ln -sf "$(HOST_YACC)" "$(HOST_TOOL_BIN)/yacc"
	@ln -sf "$(HOST_LEX)" "$(HOST_TOOL_BIN)/lex"
	@ln -sf "$(HOST_AR)" "$(HOST_TOOL_BIN)/ar"
	@ln -sf "$(HOST_AR)" "$(HOST_TOOL_BIN)/ranlib"

$(HOST_SCRIPTS)/build.%: $(V7_ROOT)/usr/src/build.% $(lastword $(MAKEFILE_LIST)) | $(HOST_ROOT)/usr/src/.host-bootstrap-root
	@mkdir -p "$(HOST_SCRIPTS)"
	@printf '%s\n' '# Generated by platform/epoch68/host-bootstrap.mk; do not edit.' >$@
	@printf '%s\n' 'set -e' >>$@
	@sed \
		-e 's|^yacc |$(HOST_YACC) |' \
		-e 's|\$$1/bin/cc|$(HOST_CC)|g' \
		-e 's|\$$1/usr/lib/ack|$(HOST_BIN)|g' \
		-e 's|\$$1/usr/lib/ack/as|$(HOST_AS)|g' \
		-e 's|\$$1/usr/lib/ack/led|$(HOST_LED)|g' \
		-e 's|\$$1/usr/lib/ack/cv|$(HOST_CV)|g' \
		-e 's|\$$1/bin/nm|$(HOST_NM) -symbols|g' \
		-e 's|\$$1/bin/sed|$(HOST_SED)|g' \
		-e 's|\$$1/lib/libc.a|$(HOST_LIBC)|g' \
		$< >>$@
	@chmod 0755 $@

$(HOST_SCRIPTS)/install.%: $(V7_ROOT)/usr/src/install.% $(lastword $(MAKEFILE_LIST)) | $(HOST_ROOT)/usr/src/.host-bootstrap-root
	@mkdir -p "$(HOST_SCRIPTS)"
	@printf '%s\n' '# Copied by platform/epoch68/host-bootstrap.mk; do not edit.' >$@
	@printf '%s\n' 'set -e' >>$@
	@sed \
		-e 's|\$$1/bin/cc|$(HOST_CC)|g' \
		-e 's|\$$1/usr/lib/ack|$(HOST_BIN)|g' \
		$< >>$@
	@chmod 0755 $@

$(HOST_SCRIPTS)/clean.%: $(V7_ROOT)/usr/src/clean.% $(lastword $(MAKEFILE_LIST)) | $(HOST_ROOT)/usr/src/.host-bootstrap-root
	@mkdir -p "$(HOST_SCRIPTS)"
	@printf '%s\n' '# Copied by platform/epoch68/host-bootstrap.mk; do not edit.' >$@
	@printf '%s\n' 'set -e' >>$@
	@sed -n 'p' $< >>$@
	@chmod 0755 $@

# yacc and lex are target binaries during a self-hosted build, but must be
# native host executables when the same scripts are projected on the host.
# They are built by `make host`; a UNIX build never rebuilds host tools.
generators:
	@test -x "$(HOST_YACC)" || { echo "make: host yacc is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(HOST_LEX)" || { echo "make: host lex is missing; run 'make host' first" >&2; exit 1; }
	@test -x "$(HOST_AWK_PROCGEN)" || { echo "make: host AWK table generator is missing; run 'make host' first" >&2; exit 1; }

runtime: $(HOST_ROOT)/usr/src/.host-bootstrap-root
	@mkdir -p "$(HOST_ROOT)/lib"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libc_crt0.o" "$(HOST_ROOT)/lib/crt0.o"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libc.a" "$(HOST_ROOT)/lib/libc.a"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libfp.a" "$(HOST_ROOT)/lib/libfp.a"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libm.a" "$(HOST_ROOT)/lib/libm.a"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libc_end.o" "$(HOST_ROOT)/lib/end.o"

build: scripts generators runtime
	@$(MAKE) --no-print-directory -f "$(lastword $(MAKEFILE_LIST))" $(addprefix build-,$(PACKAGES))
	@$(MAKE) --no-print-directory -f "$(lastword $(MAKEFILE_LIST))" packages
	@$(MAKE) --no-print-directory -f "$(lastword $(MAKEFILE_LIST))" loader

$(addprefix build-,$(PACKAGES)): build-%: scripts
	@printf '%s\n' "=== host-bootstrap $* ==="
	@cd "$(HOST_ROOT)/usr/src" && \
		PATH="$(HOST_TOOL_BIN):$(HOST_BIN):$$PATH" \
		YACC="$(HOST_YACC)" LEX="$(HOST_LEX)" \
		AS="$(HOST_AS)" AR="$(HOST_AR)" PROCGEN="$(HOST_AWK_PROCGEN)" \
		LIBM="$(HOST_LIBM)" \
		/bin/sh "$(HOST_SCRIPTS)/build.$*" "$(HOST_ROOT)"
	@cd "$(HOST_ROOT)/usr/src" && /bin/sh "$(HOST_SCRIPTS)/install.$*" "$(HOST_ROOT)"
	@cd "$(HOST_ROOT)/usr/src" && /bin/sh "$(HOST_SCRIPTS)/clean.$*" "$(HOST_ROOT)"

build-dev: runtime

packages:
	@"$(PROJECT_DIR)/platform/epoch68/package-bootstrap" \
		"$(PROJECT_DIR)" "$(HOST_ROOT)" "$(BUILD_DIR)"

# Files consumed directly by the ROM monitor's `. bootstrap` loader.  The
# projected package builds install a replacement kernel as unix.new so that a
# running V7 system is not overwritten; a fresh monitor filesystem needs the
# same image under its initial /unix and /etc/ksyms names.
loader:
	@mkdir -p "$(HOST_ROOT)/dist" "$(HOST_ROOT)/usr/lib/ack" "$(HOST_ROOT)/lib"
	@cp "$(HOST_ROOT)/unix.new" "$(HOST_ROOT)/unix"
	@cp "$(HOST_ROOT)/etc/ksyms.new" "$(HOST_ROOT)/etc/ksyms"
	@cp "$(PROJECT_DIR)/unix/bootstrap/monitor" "$(HOST_ROOT)/bootstrap"
	@cp "$(PROJECT_DIR)/unix/bootstrap/system" "$(HOST_ROOT)/dist/bootstrap"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libc_crt0.o" "$(HOST_ROOT)/lib/crt0.o"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libc.a" "$(HOST_ROOT)/lib/libc.a"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libfp.a" "$(HOST_ROOT)/lib/libfp.a"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libm.a" "$(HOST_ROOT)/lib/libm.a"
	@cp "$(BUILD_DIR)/toolchain/ack/68kunix/libc_end.o" "$(HOST_ROOT)/lib/end.o"
	@chmod 0755 "$(HOST_ROOT)/bootstrap" "$(HOST_ROOT)/dist/bootstrap" \
		"$(HOST_ROOT)/unix" "$(HOST_ROOT)/bin/cc" \
		"$(HOST_ROOT)/usr/lib/ack/cpp" "$(HOST_ROOT)/usr/lib/ack/c1" \
		"$(HOST_ROOT)/usr/lib/ack/opt" "$(HOST_ROOT)/usr/lib/ack/opt2" \
		"$(HOST_ROOT)/usr/lib/ack/ncg" "$(HOST_ROOT)/usr/lib/ack/as" \
		"$(HOST_ROOT)/usr/lib/ack/led" "$(HOST_ROOT)/usr/lib/ack/cv"
	@chmod 0644 "$(HOST_ROOT)/etc/ksyms" "$(HOST_ROOT)/lib/crt0.o" \
		"$(HOST_ROOT)/lib/libc.a" "$(HOST_ROOT)/lib/libfp.a" \
		"$(HOST_ROOT)/lib/libm.a" "$(HOST_ROOT)/lib/end.o"

clean:
	@rm -rf "$(HOST_BOOTSTRAP_DIR)"
