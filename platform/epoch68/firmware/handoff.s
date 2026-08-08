! ACK monitor handoff helpers.
!
! C calling convention:
!   kernel_handoff(src, dst, count, entry)
!   enter_user(pc, usp, sr)

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.define _kernel_handoff
.define _enter_user

HANDOFF_STACK_TOP = 0x00c3fff0

_kernel_handoff:
	move.l 4(sp),a0
	move.l 8(sp),a1
	move.l 12(sp),d0
	move.l 16(sp),a2
	move.l #HANDOFF_STACK_TOP,sp
1:
	tst.l d0
	beq 2f
	move.b (a0)+,(a1)+
	sub.l #1,d0
	bra 1b
2:
	jmp (a2)

_enter_user:
	move.l 4(sp),a0
	move.l 8(sp),a1
	move.l a1,usp
	move.l a0,-(sp)
	move.w 18(sp),-(sp)
	rte
