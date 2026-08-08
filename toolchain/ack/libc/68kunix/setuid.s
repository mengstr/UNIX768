.define _setuid
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_setuid:
	move.l	4(sp),d1
	move.l	#23,d0
	trap	#0
	jmp	__cerror
