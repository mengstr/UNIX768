.define _pause
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_pause:
	move.l	#29,d0
	trap	#0
	jmp	__cerror
