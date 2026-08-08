#include "sys/inttypes.h"

#ifndef _SETJMP_H_
#define _SETJMP_H_

/*
 * UNIX V7's original PDP-11 jmp_buf held r5, sp, and pc.  The 68000 port
 * needs room for the 68000 callee-preserved registers plus the saved return
 * stack pointer and return PC.
 */
typedef u8 jmp_buf[13 * 4];

i32 setjmp(jmp_buf);
#ifdef _V7_SH_VOID_COMPAT
typedef i32 sh_void;
#endif
void longjmp(jmp_buf, i32);

#endif
