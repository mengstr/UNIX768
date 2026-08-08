.define _chdir
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_chdir:
	move.l	4(sp),a0
	move.l	#12,d0
	trap	#0
	jmp	__cerror
