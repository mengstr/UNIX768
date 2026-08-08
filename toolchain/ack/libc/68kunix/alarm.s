.define _alarm

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_alarm:
	move.l	4(sp),d1
	move.l	#27,d0
	trap	#0
	rts
