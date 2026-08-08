#include <stdarg.h>
#include "stdio_private.h"

i32
vprintf(char *fmt, va_list args)
{
	return _doprnt(fmt, (char *)args, stdout);
}
