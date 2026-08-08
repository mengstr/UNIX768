#include "stdio_private.h"

i32
mkcreate(register char *file, i32 rw)
{
	register i32 f;

	f = creat(file, 0666);
	if (rw && f >= 0) {
		close(f);
		f = open(file, 2);
	}
	return f;
}

FILE *
_endopen(char *file, char *mode, register FILE *iop)
{
	register i32 rw, f;

	if (iop == NULL)
		return NULL;

	rw = mode[1] == '+';

	if (*mode == 'w') {
		f = mkcreate(file, rw);
	} else if (*mode == 'a') {
		if ((f = open(file, rw ? 2 : 1)) < 0) {
			f = mkcreate(file, rw);
		}
		lseek(f, 0L, 2);
	} else if (*mode == 'r') {
		f = open(file, rw ? 2 : 0);
	} else {
		return NULL;
	}

	if (f < 0)
		return NULL;

	iop->_cnt = 0;
	iop->_base = NULL;
	iop->_ptr = NULL;
	iop->_file = f;

	if (rw)
		iop->_flag |= _IORW;
	else if (*mode == 'r')
		iop->_flag |= _IOREAD;
	else
		iop->_flag |= _IOWRT;

	return iop;
}
