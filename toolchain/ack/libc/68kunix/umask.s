.define _umask

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_umask:
	move.l	4(sp),d1
	move.l	#60,d0
	trap	#0
	rts
