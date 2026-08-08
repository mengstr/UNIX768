.define begtext,begdata,begbss
.define hol0,.lino,.filn,.reghp,.limhp,.trppc,.trpim

.extern _main
.extern __cleanup
.extern _environ
.extern _end

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
begtext:
	move.l	sp,a0
	! Epoch68 userland ABI: exec builds argc/argv/envp as 32-bit stack slots.
	move.l	(a0)+,d0
	move.l	d0,d1
	add.l	#1,d1
	asl.l	#2,d1
	move.l	a0,a1
	add.l	d1,a1
	move.l	a1,_environ
	move.l	a1,-(sp)
	move.l	a0,-(sp)
	! The exec stack uses 32-bit slots, but ACK C int arguments are 16-bit.
	move.w	d0,-(sp)
	jsr	_main
	add.l	#10,sp
	move.w	d0,-(sp)
	jsr	__cleanup
	move.w	(sp)+,d1
	move.l	#1,d0
	trap	#0
1:
	bra	1b

.sect .data
begdata:
hol0:
.lino:
	.data2	0,0
.filn:
	.data4	0
.reghp:
	.data4	_end
.limhp:
	.data4	_end
.trppc:
	.data4	0
.trpim:
	.data2	0

.sect .bss
begbss:
