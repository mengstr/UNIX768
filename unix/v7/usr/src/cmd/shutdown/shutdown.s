.sect .text

.define _epoch68_shutdown

_epoch68_shutdown:
	move.l	#65501,d0
	trap	#0
	tst.l	d0
	bpl	.done
	move.l	#-1,d0
.done:
	rts
