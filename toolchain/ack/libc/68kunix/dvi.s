.define .dvi
.sect .text
.sect .rom
.sect .data
.sect .bss

! signed long divide helper.
! ACK generated code passes divisor then dividend on the stack and expects
! quotient in d1, remainder in d2.  The helper consumes both arguments.
.sect .text
.dvi:
	move.l	(sp)+,a1
	move.l	(sp)+,d0
	move.l	(sp)+,d2
	movem.l	d3-d7,-(sp)
	clr.l	d1
	clr.l	d6
	clr.l	d7

	tst.l	d2
	bpl	1f
	neg.l	d2
	not.l	d6
	not.l	d7
1:
	tst.l	d0
	bpl	2f
	neg.l	d0
	not.l	d6
2:
	tst.l	d0
	beq	9f
	cmp.l	d0,d2
	bcs	8f

	move.l	#1,d3
	move.l	d0,d4
3:
	btst	#31,d4
	bne	4f
	move.l	d4,d5
	lsl.l	#1,d5
	cmp.l	d2,d5
	bhi	4f
	move.l	d5,d4
	lsl.l	#1,d3
	bra	3b

4:
	cmp.l	d4,d2
	bcs	5f
	sub.l	d4,d2
	or.l	d3,d1
5:
	lsr.l	#1,d4
	lsr.l	#1,d3
	bne	4b

8:
	tst.l	d6
	bpl	6f
	neg.l	d1
6:
	tst.l	d7
	bpl	7f
	neg.l	d2
7:
	movem.l	(sp)+,d3-d7
	jmp	(a1)

9:
	move.w	#1,-(sp)
	jsr	_exit
1:
	bra	1b
