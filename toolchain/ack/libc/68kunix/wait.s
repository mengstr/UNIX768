.define _wait
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_wait:
	move.l	4(sp),a0
	move.l	#7,d0
	trap	#0
	jmp	__cerror
