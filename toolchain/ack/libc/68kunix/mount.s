.define _mount
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_mount:
	move.l	4(sp),a0
	move.l	8(sp),a1
	move.l	12(sp),d1
	move.l	#21,d0
	trap	#0
	jmp	__cerror
