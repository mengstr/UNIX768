.define _getpid

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_getpid:
	move.l	#20,d0
	trap	#0
	rts
