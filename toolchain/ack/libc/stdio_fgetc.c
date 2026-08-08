#include "stdio_private.h"

#undef fgetc

i32
fgetc(fp)
	register FILE *fp;
{
	if (--fp->_cnt >= 0)
		return *fp->_ptr++ & 0377;
	return _filbuf(fp);
}

