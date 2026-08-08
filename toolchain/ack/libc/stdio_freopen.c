#include "stdio_private.h"

FILE *
freopen(file, mode, iop)
	char *file, *mode;
	register FILE *iop;
{
	if (file == NULL)
		return iop;
	fclose(iop);
	return _endopen(file, mode, iop);
}

