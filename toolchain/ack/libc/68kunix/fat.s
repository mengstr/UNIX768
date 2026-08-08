.define .fat
ECASE	= 20

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.fat:
	add	#2,sp
	move.l	#1,-(sp)
	jsr	_exit
