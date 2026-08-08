.define _fstat
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_fstat:
	move.l	4(sp),d1
	move.l	8(sp),a0
	move.l	#28,d0
	trap	#0
	jmp	__cerror
