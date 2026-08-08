.define _errno,__cerror,_epoch68_trace_full

.sect .text
__cerror:
	tst.l	d0
	bpl	1f
	neg.l	d0
	move.l	d0,_errno
	move.l	#-1,d0
1:
	rts

_epoch68_trace_full:
	rts

.sect .rom
.sect .data
.sect .bss

.sect .data
_errno:
	.data4	0
