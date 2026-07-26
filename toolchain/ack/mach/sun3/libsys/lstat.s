#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _lstat
_lstat: pea (SYS_lstat); trap #0; bcc 1f; jmp (cerror)
1:
	move.l	(sp)+,(sp)
	jmp	(___statcvt)
