.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text






.define _uarea
.define _ukstack
.define _proc

EPOCH68_KMETA_BASE = 0x00c00000
EPOCH68_UAREA_SIZE = 46368		
EPOCH68_UKSTACK_SIZE = 129024		
EPOCH68_PROC_SIZE = 2268		

_uarea = EPOCH68_KMETA_BASE
_ukstack = _uarea+EPOCH68_UAREA_SIZE
_proc = _ukstack+EPOCH68_UKSTACK_SIZE
