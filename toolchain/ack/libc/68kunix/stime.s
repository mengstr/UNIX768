.define _stime
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_stime:
	move.l	4(sp),a0
	move.l	(a0),d1
	move.l	#25,d0
	trap	#0
	jmp	__cerror
