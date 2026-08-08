.define _nice
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_nice:
	move.l	4(sp),d1
	move.l	#34,d0
	trap	#0
	jmp	__cerror
