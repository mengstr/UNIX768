#include <stdarg.h>
#include "stdio_private.h"

i32
sscanf(char *str, char *fmt, ...)
{
	va_list args;
	i32 n;

	va_start(args, fmt);
	n = _doscan_string(str, fmt, args);
	va_end(args);
	return n;
}
