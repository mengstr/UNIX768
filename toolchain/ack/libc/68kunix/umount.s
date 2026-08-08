.define _umount
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_umount:
	move.l	4(sp),a0
	move.l	#22,d0
	trap	#0
	jmp	__cerror
