.define _sync

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_sync:
	move.l	#36,d0
	trap	#0
	rts
