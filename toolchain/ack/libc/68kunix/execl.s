.define _execl
.extern _environ
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_execl:
	! The caller's stack is: return address, name, arg0, ..., NULL.
	! Keep that original arg0 vector intact; a C wrapper's local frame is not
	! a valid substitute for a variable-length argument vector on 68k ACK.
	move.l	d2,-(sp)
	move.l	8(sp),a0
	lea	12(sp),a1
	move.l	_environ,d2
	move.l	#11,d0
	trap	#0
	move.l	(sp)+,d2
	jmp	__cerror
