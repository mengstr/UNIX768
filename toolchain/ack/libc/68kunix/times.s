.define _times
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_times:
	move.l	4(sp),a0
	move.l	#43,d0
	trap	#0
	jmp	__cerror
