.define _fork
.extern __cerror

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_fork:
	move.l	#2,d0
	trap	#0
	jmp	__cerror
