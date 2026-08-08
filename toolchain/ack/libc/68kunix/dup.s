.define _dup
.define _dup2
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_dup2:
	move.l	d2,-(sp)
	move.l	8(sp),d1
	or.l	#64,d1
	move.l	12(sp),d2
	move.l	#41,d0
	trap	#0
	move.l	(sp)+,d2
	jmp	__cerror

_dup:
	move.l	d2,-(sp)
	move.l	8(sp),d1
	move.l	d1,d2
	and.l	#64,d2
	beq	1f
	move.l	12(sp),d2
	bra	2f
1:
	clr.l	d2
2:
	move.l	#41,d0
	trap	#0
	move.l	(sp)+,d2
	jmp	__cerror
