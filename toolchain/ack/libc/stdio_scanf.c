#include <stdarg.h>
#include "stdio_private.h"

i32
scanf(char *fmt, ...)
{
	va_list args;
	i32 n;

	va_start(args, fmt);
	n = _doscan_file(stdin, fmt, args);
	va_end(args);
	return n;
}
