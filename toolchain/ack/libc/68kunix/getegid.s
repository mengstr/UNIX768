.define _getegid

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_getegid:
	move.l	#47,d0
	trap	#0
	move.l	d1,d0
	rts
