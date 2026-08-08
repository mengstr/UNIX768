.define _unlink
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_unlink:
	move.l	4(sp),a0
	move.l	#10,d0
	trap	#0
	jmp	__cerror
