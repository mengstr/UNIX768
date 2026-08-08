.define _getgid

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_getgid:
	move.l	#47,d0
	trap	#0
	rts
