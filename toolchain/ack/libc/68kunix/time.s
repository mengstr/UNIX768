.define _time
.define _ftime
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_time:
	move.l	4(sp),a0
	move.l	a0,-(sp)
	move.l	#13,d0
	trap	#0
	move.l	(sp)+,a0
	tst.l	d0
	bmi	2f
	cmp.l	#0,a0
	beq	1f
	move.l	d0,(a0)
1:
	rts
2:
	jmp	__cerror

_ftime:
	move.l	4(sp),a0
	move.l	#35,d0
	trap	#0
	jmp	__cerror
