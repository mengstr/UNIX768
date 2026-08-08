.define .csa2
ECASE	= 20

.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csa2:
	move.l	(a0)+,a1
	sub	(a0)+,d0
	blt	1f
	cmp	(a0)+,d0
	bgt	1f
	asl	#2,d0
	add	d0,a0
	move.l	(a0),d1
	move.l	d1,d0
	beq	1f
	move.l	d1,a1
	bra	3f
1:
	move.l	a1,d0
	beq	2f
3:
	jmp	(a1)
2:
	move.w	#ECASE,-(sp)
	jmp	.fat
