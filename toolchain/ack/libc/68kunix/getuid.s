.define _getuid

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_getuid:
	move.l	#24,d0
	trap	#0
	rts
