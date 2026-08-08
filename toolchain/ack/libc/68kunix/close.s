.define _close
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_close:
	move.l	4(sp),d1
	move.l	#6,d0
	trap	#0
	jmp	__cerror
