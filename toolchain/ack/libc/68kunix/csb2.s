.define .csb2
ECASE	= 20

.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csb2:
	move.l	(a0)+,a1
	move.w	(a0)+,d1
	beq	2f
1:
	cmp	(a0)+,d0
	beq	3f
	tst.l	(a0)+
	sub	#1,d1
	bgt	1b
2:
	move.l	a1,d1
	bne	4f
	move.w	#ECASE,-(sp)
	jmp	.fat
3:
	move.l	(a0)+,a1
4:
	jmp	(a1)
