#include <stdarg.h>
#include "stdio_private.h"

i32
fprintf(FILE *iop, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_doprnt(fmt, args, iop);
	va_end(args);
	return ferror(iop) ? EOF : 0;
}

