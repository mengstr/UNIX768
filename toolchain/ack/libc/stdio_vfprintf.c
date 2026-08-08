#include <stdarg.h>
#include "stdio_private.h"

i32
vfprintf(FILE *iop, char *fmt, va_list args)
{
	i32 count;

	count = _doprnt(fmt, (char *)args, iop);
	return ferror(iop) ? EOF : count;
}
