#include "stdio_private.h"

i32
ungetc(c, iop)
i32 c;
register FILE *iop;
{
	if (c == EOF)
		return -1;
	if ((iop->_flag & _IOREAD) == 0)
		return -1;
	if (iop->_ptr <= iop->_base) {
		if (iop->_ptr != iop->_base || iop->_cnt != 0)
			return -1;
		iop->_ptr++;
	}
	iop->_cnt++;
	*--iop->_ptr = c;
	return c;
}
