.define _setjmp
.define _longjmp

.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
_setjmp:
	move.l	4(sp),a0
	movem.l	d2-d7/a2-a6,(a0)
	lea	4(sp),a1
	move.l	a1,44(a0)
	move.l	(sp),48(a0)
	move.l	#0,d0
	rts

_longjmp:
	move.l	4(sp),a0
	move.w	8(sp),d0
	ext.l	d0
	bne	1f
	move.l	#1,d0
1:
	move.l	48(a0),a1
	movem.l	(a0),d2-d7/a2-a6
	move.l	44(a0),sp
	jmp	(a1)
