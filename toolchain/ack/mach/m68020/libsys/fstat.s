.define ___fstat
.extern ___fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
___fstat:	move.l	#0x1C,d0
		jmp (callc)
