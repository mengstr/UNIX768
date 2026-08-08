.define _chmod
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_chmod:
	move.l	4(sp),a0
	move.l	8(sp),d1
	move.l	#15,d0
	trap	#0
	jmp	__cerror
