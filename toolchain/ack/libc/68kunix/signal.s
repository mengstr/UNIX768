.define _signal
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_signal:
	move.l	d2,-(sp)
	! ACK int arguments are 16-bit. signal(int, handler) is laid out as
	! a word at 8(sp), followed by the 32-bit handler pointer at 10(sp).
	move.w	8(sp),d1
	ext.l	d1
	move.l	10(sp),a0
	move.l	#__sigtramp,d2
	move.l	#48,d0
	trap	#0
	move.l	(sp)+,d2
	jmp	__cerror

__sigtramp:
	! A signal may interrupt arbitrary user code, not just a normal call
	! boundary.  Preserve every data and address register before calling the
	! C handler.  The kernel frame below the saved registers is:
	! handler.l, signo.w, interrupted_pc.l.
	movem.l	d0-d7/a0-a6,-(sp)
	! movem saved 15 registers (60 bytes).  Pushing the signal number moves
	! the stack down another word, leaving the handler at 62(sp).
	move.w	64(sp),-(sp)
	move.l	62(sp),a0
	jsr	(a0)
	add.l	#2,sp
	movem.l	(sp)+,d0-d7/a0-a6
	add.l	#6,sp
	rts
