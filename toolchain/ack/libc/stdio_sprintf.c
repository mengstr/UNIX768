#include <stdarg.h>
#include "stdio_private.h"

char *
sprintf(char *str, char *fmt, ...)
{
	va_list args;
	struct _iobuf _strbuf;

	_strbuf._flag = _IOWRT + _IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	va_start(args, fmt);
	_doprnt(fmt, args, &_strbuf);
	va_end(args);
	putc('\0', &_strbuf);
	return str;
}

