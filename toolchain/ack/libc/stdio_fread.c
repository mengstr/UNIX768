#include "stdio_private.h"

i32
fread(char *ptr, usize size, usize count, FILE *iop)
{
	register i32 c;
	usize ndone, s;

	ndone = 0;
	if (size)
	for (; ndone < count; ndone++) {
		s = size;
		do {
			if ((c = getc(iop)) >= 0)
				*ptr++ = c;
			else
				return ndone;
		} while (--s);
	}
	return ndone;
}

