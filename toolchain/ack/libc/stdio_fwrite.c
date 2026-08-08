#include "stdio_private.h"

i32
fwrite(register char *ptr, usize size, usize count, register FILE *iop)
{
	register usize s;
	usize ndone;

	ndone = 0;
	if (size)
	for (; ndone < count; ndone++) {
		s = size;
		do {
			putc(*ptr++, iop);
		} while (--s);
		if (ferror(iop))
			break;
	}
	return ndone;
}
