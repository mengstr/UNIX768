.define _execv
.extern _environ
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_execv:
	move.l	d2,-(sp)
	move.l	8(sp),a0
	move.l	12(sp),a1
	move.l	_environ,d2
	move.l	#11,d0
	trap	#0
	move.l	(sp)+,d2
	jmp	__cerror
