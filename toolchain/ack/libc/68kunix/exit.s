.define __exit,__exit_raw

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
__exit:
	! Historical _exit(int) callers pass the 16-bit ACK int width.
	move.w	4(sp),d1
	ext.l	d1
	bra	1f
__exit_raw:
	move.l	4(sp),d1
1:
	move.l	#1,d0
	trap	#0
2:
	bra	2b
