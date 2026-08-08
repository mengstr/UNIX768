#include "stdio_private.h"

void
setbuf(iop, buf)
register struct _iobuf *iop;
char *buf;
{
	iop->_flag &= ~(_IOMYBUF | _IONBF);
	if ((iop->_base = buf) == NULL)
		iop->_flag |= _IONBF;
	else
		iop->_ptr = iop->_base;
	iop->_cnt = 0;
}

