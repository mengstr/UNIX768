.define .mlu
.sect .text
.sect .rom
.sect .data
.sect .bss

! unsigned long multiply helper from ACK m68k2 libem.
.sect .text
.mlu:
	move.l	(sp)+,a1
	move.l	d3,a0
	movem.w	(sp)+,d0-d3
	tst.w	d0
	bne	1f
	tst.w	d2
	bne	2f
	mulu	d3,d1
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
	move.l	a0,d3
	jmp	(a1)
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
	move.l	a0,d3
	jmp	(a1)
