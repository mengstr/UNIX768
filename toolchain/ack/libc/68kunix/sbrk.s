.define _sbrk,_brk
.extern _end
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_sbrk:
	move.l	d2,-(sp)
	move.l	_nd,d2
	add.l	#3,d2
	and.l	#-4,d2
	move.l	8(sp),d1
	beq	1f
	add.l	d2,d1
	move.l	#17,d0
	trap	#0
	tst.l	d0
	bmi	2f
	move.l	d1,_nd
1:
	move.l	d2,d0
	move.l	(sp)+,d2
	rts
2:
	move.l	(sp)+,d2
	jmp	__cerror

_brk:
	move.l	4(sp),d1
	move.l	#17,d0
	trap	#0
	tst.l	d0
	bmi	1f
	move.l	d1,_nd
	clr.l	d0
	rts
1:
	jmp	__cerror

.sect .data
_nd:
	.data4	_end
