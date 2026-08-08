.define _utime
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_utime:
	move.l	4(sp),a0
	move.l	8(sp),a1
	move.l	#30,d0
	trap	#0
	jmp	__cerror
