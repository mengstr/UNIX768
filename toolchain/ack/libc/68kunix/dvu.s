.define .dvu
.sect .text
.sect .rom
.sect .data
.sect .bss

! unsigned long divide helper.
! ACK generated code passes divisor then dividend on the stack and expects
! quotient in d1, remainder in d2.  The helper consumes both arguments.
.sect .text
.dvu:
	move.l	(sp)+,a1
	move.l	(sp)+,d0
	move.l	(sp)+,d2
	movem.l	d3-d5,-(sp)
	clr.l	d1
	tst.l	d0
	beq	9f
	cmp.l	d0,d2
	bcs	8f

	move.l	#1,d3
	move.l	d0,d4
1:
	btst	#31,d4
	bne	2f
	move.l	d4,d5
	lsl.l	#1,d5
	cmp.l	d2,d5
	bhi	2f
	move.l	d5,d4
	lsl.l	#1,d3
	bra	1b

2:
	cmp.l	d4,d2
	bcs	3f
	sub.l	d4,d2
	or.l	d3,d1
3:
	lsr.l	#1,d4
	lsr.l	#1,d3
	bne	2b

8:
	movem.l	(sp)+,d3-d5
	jmp	(a1)

9:
	move.w	#1,-(sp)
	jsr	_exit
1:
	bra	1b
