#include "stdio_private.h"

i32
_flsbuf(c, iop)
	i32 c;
	register FILE *iop;
{
	register char *base;
	register i32 n, rn;
	char c1;
	register i32 idx;

	if (iop->_flag & _IORW) {
		iop->_flag |= _IOWRT;
		iop->_flag &= ~_IOEOF;
	}

tryagain:
	rn = 0;
	n = 0;
	if (iop->_flag & _IONBF) {
		c1 = c;
		rn = 1;
		n = write(fileno(iop), &c1, rn);
		iop->_cnt = 0;
	} else {
		if ((base = iop->_base) == NULL) {
			if (iop == stdout) {
				if (isatty(fileno(stdout))) {
					iop->_flag |= _IONBF;
					goto tryagain;
				}
				iop->_base = base = _sobuf;
				iop->_ptr = base;
				rn = n = 0;
				goto havebase;
			}
			idx = fileno(iop);
			if (idx < 0 || idx >= _NFILE) {
				iop->_flag |= _IONBF;
				goto tryagain;
			}
			iop->_base = base = malloc(BUFSIZ);
			if (base == NULL) {
				iop->_flag |= _IONBF;
				goto tryagain;
			}
			iop->_flag |= _IOMYBUF;
			rn = n = 0;
		} else if ((rn = n = iop->_ptr - base) > 0) {
			iop->_ptr = base;
			n = write(fileno(iop), base, n);
		}
havebase:
		iop->_cnt = BUFSIZ - 1;
		*base++ = c;
		iop->_ptr = base;
	}
	if (rn != n) {
		iop->_flag |= _IOERR;
		return EOF;
	}
	return c;
}
