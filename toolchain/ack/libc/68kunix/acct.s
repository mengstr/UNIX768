.define _acct
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_acct:
	move.l	4(sp),a0
	move.l	#51,d0
	trap	#0
	jmp	__cerror
