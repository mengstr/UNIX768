.define _write
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_write:
	move.l	d2,-(sp)
	move.l	8(sp),d1
	move.l	12(sp),a0
	move.l	16(sp),d2
	move.l	#4,d0
	trap	#0
	move.l	(sp)+,d2
	jmp	__cerror
