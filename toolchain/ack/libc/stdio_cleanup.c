#include "stdio_private.h"

void
_cleanup(void)
{
	register FILE *iop;
	register i32 i;

	for (i = 0; i < _NFILE; i++) {
		iop = &_iob[i];
		fclose(iop);
	}
}
