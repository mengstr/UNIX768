.define _pipe
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_pipe:
	sub.l	#8,sp
	move.l	sp,a0
	move.l	#42,d0
	trap	#0
	tst.l	d0
	bmi	1f
	move.l	12(sp),a0
	cmp.l	#0,a0
	beq	1f
	move.l	(sp),d1
	move.w	d1,(a0)
	move.l	4(sp),d1
	move.w	d1,2(a0)
1:
	add.l	#8,sp
	jmp	__cerror
