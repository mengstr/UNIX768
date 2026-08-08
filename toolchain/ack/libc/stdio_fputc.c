#include "stdio_private.h"

#undef fputc

i32
fputc(c, fp)
	i32 c;
	register FILE *fp;
{
	if (--fp->_cnt >= 0)
		return (*fp->_ptr++ = c) & 0377;
	return _flsbuf(c, fp);
}
