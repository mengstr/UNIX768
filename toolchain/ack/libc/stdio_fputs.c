#include "stdio_private.h"

i32
fputs(s, iop)
	register char *s;
	register FILE *iop;
{
	register i32 r;
	register i32 c;

	r = 0;
	while (c = *s++)
		r = putc(c, iop);
	return r;
}

