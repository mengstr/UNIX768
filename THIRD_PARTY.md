# Third-Party Copyright and Provenance

This file records the origin, exact imported version, copyright notices, and
license material for source incorporated into UNIX768. It is an inventory, not
legal advice and not a replacement for notices retained in individual files.

The inventory must be updated whenever third-party material is added, removed,
or replaced. File-level notices take precedence over component-level summaries
below.

## Historical UNIX768 Repository

- Repository: <https://github.com/mengstr/UNIX768-archived>
- Final pre-reorganization commit:
  `50e165701505de457e511005d1a97b8909204435`
- Tag: `pre-reorg-final`
- Role: provenance and engineering-history reference only; no Git history was
  imported into this repository.

## UNIX Seventh Edition

- Local path: `unix/v7`
- Source archive:
  <https://www.tuhs.org/Archive/Distributions/Research/Henry_Spencer_v7/v7.tar.gz>
- Archive identity: Henry Spencer V7 distribution tar file, 3,595,769 bytes
- SHA-256:
  `e0fd7bdd046e17de177f5d997acf8123fa43c57b46b032c7337f7666ab69a07e`
- TUHS provenance:
  <https://www.tuhs.org/Archive/Distributions/Research/Henry_Spencer_v7/README>
- Pristine baseline: repository commit 1
- Curated port source: historical UNIX768 final commit
  `50e165701505de457e511005d1a97b8909204435`
- Copyright notice named by the applicable license:
  `Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.`
- License: the Caldera International letter dated January 23, 2002, covering
  16-bit UNIX Versions 1 through 7 and 32V. A copy is retained at
  `LICENSES/Caldera-2002.pdf`.

TUHS describes this archive as containing the original V7 contents repackaged
as a tar file, rather than an image of the original self-booting distribution
tape. The separate `v7.patches.tar.gz` was not applied.

The archive contains four yacc documentation files whose names collide in
pairs on the default case-insensitive macOS filesystem:

- `usr/doc/yacc/ssA` and `usr/doc/yacc/ssa`
- `usr/doc/yacc/ssB` and `usr/doc/yacc/ssb`

UNIX768 retains `ssA` and `ssB`, matching the historical UNIX768 import, and
omits `ssa` and `ssb`. No other archive-path omission is intended.

## Amsterdam Compiler Kit

- Local path: `toolchain/ack`
- Upstream repository: <https://github.com/davidgiven/ack>
- Imported revision:
  `cd8805a0ad462c120ef394225c73ce725d271740`
- Revision page:
  <https://github.com/davidgiven/ack/commit/cd8805a0ad462c120ef394225c73ce725d271740>
- Upstream description at that revision: Amsterdam Compiler Kit V6.2+
- Historical UNIX768 extraction point:
  `bcac46043a286a990546c14930d4177004b66b12`, initially under `cc/`
- Primary copyright:
  `Copyright (c) 1987, 1990, 1993, 2005 Vrije Universiteit, Amsterdam, The Netherlands.`
- Primary license: BSD-style three-condition license retained in
  `toolchain/ack/Copyright` and copied to `LICENSES/ACK.txt`.

Repository commit 1 contains the complete checkout of the stated revision.
The working tree after commit 2 contains the subset adopted and modified by
the historical UNIX768 port. Files absent from that curated subset remain
recoverable from commit 1. The primary license does not replace more specific
notices embedded in subdirectories or individual files.

### Separately Noticed ACK Material

| Scope | Copyright or provenance | Notice |
| --- | --- | --- |
| `lang/a68s` | Dr. C. H. Lindsey, University of Manchester; individual files also name Yavuz Onder and other contributors | `toolchain/ack/lang/a68s/COPYRIGHT` and file headers |
| `lang/fortran/comp`, `lang/fortran/lib/libF77`, `lang/fortran/lib/libI77` | AT&T Bell Laboratories and Bellcore, 1990–1991 | The `Notice` file in each listed directory |
| `plat/linux68k/emu/musashi` | Karl Stenerud, 1998–2002; some MMU material names Nicola Salmoria and the MAME Team | `toolchain/ack/plat/linux68k/emu/musashi/readme.txt` and file headers |
| `plat/pc86/emu/x86emu` | X.Org/XFree86-derived x86 emulator with multiple MIT/X11 and legacy notice holders | `toolchain/ack/plat/pc86/emu/x86emu/COPYING` and file headers |
| `util/mcgg` | David R. Hanson, 1993–1996 | `toolchain/ack/util/mcgg/LICENSE` |
| `util/LLgen` | Vrije Universiteit, under the ACK license | `toolchain/ack/util/LLgen/COPYING` |
| `lang/cem/libcc.ansi/core/misc/termcap.c`, `lang/cem/libcc/stdio/termcap.c` | Joypace Ltd, 1987; freely distributable with notice retained | File headers |
| Minix-derived material, including files under `plat/minix` and `plat/minix68k` | Some files carry Prentice-Hall, Inc. notices restricting redistribution to educational or research purposes without separate permission for other use | Individual file headers; this scope requires a file-level audit before a release |
| Files naming the ACK Project, David Given, TK Chia, Bart Oldeman, and other later contributors | Later ACK work and contributions | Individual file headers, generally referring back to `toolchain/ack/Copyright` where stated |

The embedded-notice inventory is intentionally conservative and should be
reviewed whenever the curated ACK subset changes.

## Musashi

- Local path: `third_party/musashi`
- Upstream: <https://github.com/kstenerud/Musashi>
- Imported revision: `313ebf1bd9f4d0d93341eb5ce21fd8a119e9dbdd`
- Historical import commit:
  `a177906f9223060d4bfce2664b10d7c8eed3430a`
- Local change: initialization of instruction and cycle tables in `m68k_in.c`
- Copyright and license notices: `third_party/musashi/readme.txt` and
  individual file headers; SoftFloat carries its own notices under
  `third_party/musashi/softfloat`

## miniaudio

- Local path: `third_party/miniaudio`
- Upstream: <https://github.com/mackron/miniaudio>
- Historical import commit:
  `ad7874d39b711e0b2b08e0465b1d6e0607afeead`
- Role: optional Epoch68 audio output
- Copyright and license: retained in `third_party/miniaudio/miniaudio.h`

## UNIX768-Authored Material

Copyright (c) 2026 Mats Engström.

The project-authored portions of UNIX768 are licensed under the BSD 2-Clause
License, reproduced in the repository-root `LICENSE` file. This includes
original port-specific sources, build files, documentation, host tools, and
other original project material.

That license applies only to rights held in project-authored material. It does
not replace or broaden the terms for UNIX V7, ACK, Musashi, miniaudio,
or other imported material; their retained notices and licenses continue to
apply to the relevant files and to modifications of those files.
