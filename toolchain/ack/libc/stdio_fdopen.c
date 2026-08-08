#include "stdio_private.h"

FILE *
fdopen(fd, mode)
i32 fd;
	register char *mode;
{
	register FILE *iop;

	if ((iop = _findiop()) == NULL)
		return NULL;

	iop->_cnt = 0;
	iop->_base = NULL;
	iop->_ptr = NULL;
	iop->_file = fd;
	if (*mode == 'r') {
		iop->_flag |= _IOREAD;
	} else if (*mode == 'a') {
		lseek(fd, 0L, 2);
		iop->_flag |= _IOWRT;
	} else if (*mode == 'w') {
		iop->_flag |= _IOWRT;
	} else {
		return NULL;
	}

	if (mode[1] == '+') {
		iop->_flag &= ~(_IOREAD | _IOWRT);
		iop->_flag |= _IORW;
	}

	return iop;
}
