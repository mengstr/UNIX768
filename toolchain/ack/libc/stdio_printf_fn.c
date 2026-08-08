#include <stdarg.h>
#include "stdio_private.h"

i32
printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_doprnt(fmt, args, stdout);
	va_end(args);
	return ferror(stdout) ? EOF : 0;
}

