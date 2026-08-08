#include "stdio_private.h"

i32
fflush(iop)
	register FILE *iop;
{
	register char *base;
	register i32 n;
	register FILE *fp;
	i32 r;

	if (iop == NULL) {
		r = 0;
		for (fp = _iob; fp < _lastbuf; fp++) {
			if (fflush(fp) == EOF)
				r = EOF;
		}
		return r;
	}

	if ((iop->_flag & (_IONBF | _IOWRT)) == _IOWRT
	    && (base = iop->_base) != NULL && (n = iop->_ptr - base) > 0) {
		iop->_ptr = base;
		iop->_cnt = BUFSIZ;
		if (write(fileno(iop), base, n) != n) {
			iop->_flag |= _IOERR;
			return EOF;
		}
	}
	return 0;
}

