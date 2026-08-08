.define .mli
.sect .text
.sect .rom
.sect .data
.sect .bss

! signed long multiply helper from ACK m68k2 libem.
.sect .text
.mli:
	move.l	(sp)+,a1
	move.l	d3,a0
	movem.w	(sp)+,d0-d3
	move.w	d5,-(sp)
	move.w	d2,d5
	bge	0f
	neg.w	d1
	negx.w	d0
0:	tst.w	d0
	bge	0f
	eor.w	d0,d5
	neg.w	d1
	negx.w	d0
0:	bne	1f
	tst.w	d2
	bne	2f
	mulu	d3,d1
9:
	tst.w	d5
	bpl	0f
	neg.l	d1
	negx.l	d0
0:	move.w	(sp)+,d5
	move.l	a0,d3
	jmp	(a1)
1:
	tst.w	d2
	bne	3f
	exg	d0,d2
	exg	d1,d3
2:
	mulu	d1,d2
	mulu	d3,d1
	swap	d2
	clr.l	d3
	move.w	d2,d3
	clr.w	d2
	add.l	d2,d1
	addx.l	d3,d0
	bra	9b
3:
	move.l	d4,-(sp)
	move.w	d2,d4
	mulu	d1,d4
	mulu	d3,d1
	mulu	d0,d3
	mulu	d2,d0
	swap	d1
	add.w	d4,d1
	clr.w	d4
	swap	d4
	addx.l	d4,d0
	add.w	d3,d1
	clr.w	d3
	swap	d3
	addx.l	d3,d0
	swap	d1
	move.l	(sp)+,d4
	bra	9b
