#include "stdio_private.h"

i32
fclose(iop)
	register FILE *iop;
{
	register i32 r;

	r = EOF;
	if (iop->_flag & (_IOREAD | _IOWRT | _IORW)
	    && (iop->_flag & _IOSTRG) == 0) {
		r = fflush(iop);
		if (close(fileno(iop)) < 0)
			r = EOF;
		if (iop->_flag & _IOMYBUF) {
			free(iop->_base);
			iop->_base = NULL;
		} else if (iop->_flag & _IONBF)
			iop->_base = NULL;
	}
	iop->_base = NULL;
	iop->_ptr = NULL;
	iop->_flag &=
	    ~(_IOREAD | _IOWRT | _IONBF | _IOMYBUF | _IOERR | _IOEOF | _IOSTRG | _IORW);
	iop->_cnt = 0;
	return r;
}
