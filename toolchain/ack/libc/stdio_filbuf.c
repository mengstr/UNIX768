#include "stdio_private.h"

i32
_filbuf(iop)
	register FILE *iop;
{
	static char smallbuf[_NFILE];
	register i32 idx;

	if (iop->_flag & _IORW)
		iop->_flag |= _IOREAD;

	if ((iop->_flag & _IOREAD) == 0 || iop->_flag & _IOSTRG)
		return EOF;

tryagain:
	if (iop->_base == NULL) {
		idx = fileno(iop);
		if (idx < 0 || idx >= _NFILE)
			return EOF;
		if (iop->_flag & _IONBF) {
			iop->_base = &smallbuf[idx];
			goto tryagain;
		}
		iop->_base = malloc(BUFSIZ);
		if (iop->_base == NULL)
			return EOF;
		iop->_flag |= _IOMYBUF;
	}
	iop->_ptr = iop->_base;
	iop->_cnt = read(fileno(iop), iop->_ptr, iop->_flag & _IONBF ? 1 : BUFSIZ);
	if (--iop->_cnt < 0) {
		if (iop->_cnt == -1) {
			iop->_flag |= _IOEOF;
			if (iop->_flag & _IORW)
				iop->_flag &= ~_IOREAD;
		} else
			iop->_flag |= _IOERR;
		iop->_cnt = 0;
		return EOF;
	}
	return *iop->_ptr++ & 0377;
}

