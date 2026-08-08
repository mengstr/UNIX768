.define _setgid
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_setgid:
	move.l	4(sp),d1
	move.l	#46,d0
	trap	#0
	jmp	__cerror
