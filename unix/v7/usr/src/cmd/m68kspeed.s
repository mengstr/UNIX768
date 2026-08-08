.define _m68kspeed_chunk

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
_m68kspeed_chunk:
	move.l	d3,-(sp)
	move.l	#0x13579bdf,d0
	move.l	#0x2468ace1,d1
	move.l	#0x0f0f0f0f,d2
	move.w	#0x4b1c,d3
loop:
	add.l	d1,d0
	eor.l	d2,d0
	rol.l	#1,d0
	sub.l	d2,d0
	swap	d0
	add.w	d1,d0
	dbra	d3,loop
	move.l	(sp)+,d3
	nop
	rts
