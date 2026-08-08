.define _geteuid

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_geteuid:
	move.l	#24,d0
	trap	#0
	move.l	d1,d0
	rts
