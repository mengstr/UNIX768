#include <stdio.h>
#include <stdarg.h>
#include "types.h"

#undef fgetc
#undef fputc
#undef getchar
#undef putchar

extern char _sibuf[];
extern char _sobuf[];
extern struct _iobuf _iob[];
extern struct _iobuf *_lastbuf;
extern i32 read(i32 fd, char *buf, i32 count);
extern i32 write(i32 fd, char *buf, i32 count);
extern i32 open(char *path, i32 mode);
extern i32 creat(char *path, i32 mode);
extern i32 close(i32 fd);
extern off32 lseek(i32 fd, off32 offset, i32 whence);
extern char *malloc(usize size);
extern i32 isatty(i32 fd);

i32 getchar(void);
i32 putchar(i32 c);
i32 puts(char *s);
static i32 _doprnt_isdigit(i32 c);
static void _doprnt_udivmod(u32 value, i32 base, u32 *qout, u32 *rout);
static void _doprnt_putnchars(FILE *stream, i32 ch, i32 count);
static i32 _doprnt_utoa(u32 value, i32 base, i32 upper, char *buf);
static i32 _doprnt_print_string(FILE *stream, char *str, i32 width, i32 precision, i32 left);
static i32 _doprnt_print_chars(FILE *stream, char *str, i32 len, i32 width, i32 left, i32 zero);
static i32 _doprnt_print_number(FILE *stream, u32 uvalue, i32 negative, i32 base, i32 upper, i32 width, i32 precision, i32 left, i32 zero);

FILE *
_findiop()
{
	register FILE *iop;

	for (iop = _iob; iop->_flag & (_IOREAD | _IOWRT | _IORW); iop++)
		if (iop >= _lastbuf)
			return NULL;
	return iop;
}

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

i32
_flsbuf(c, iop)
	i32 c;
	register FILE *iop;
{
	register char *base;
	register i32 n, rn;
	char c1;
	extern char _sobuf[];
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

void
_cleanup()
{
	register FILE *iop;
	register i32 i;

	for (i = 0; i < _NFILE; i++) {
		iop = &_iob[i];
		fclose(iop);
	}
}

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

i32
mkcreate(file, rw)
	register char *file;
	i32 rw;
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
_endopen(file, mode, iop)
	char *file, *mode;
	register FILE *iop;
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

FILE *
fopen(file, mode)
	char *file, *mode;
{
	return _endopen(file, mode, _findiop());
}

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

FILE *
freopen(file, mode, iop)
	char *file, *mode;
	register FILE *iop;
{
	if (file == NULL)
		return iop;
	fclose(iop);
	return _endopen(file, mode, iop);
}

i32
fgetc(fp)
	register FILE *fp;
{
	if (--fp->_cnt >= 0)
		return *fp->_ptr++ & 0377;
	return _filbuf(fp);
}

i32
fputc(c, fp)
	i32 c;
	register FILE *fp;
{
	if (--fp->_cnt >= 0)
		return (*fp->_ptr++ = c) & 0377;
	return _flsbuf(c, fp);
}

i32
fputs(s, iop)
	register char *s;
	register FILE *iop;
{
	register i32 r;
	register i32 c;

	r = 0;
	while (c = *s++)
		r = putc(c, iop);
	return r;
}

char *
fgets(char *s, usize n, FILE *iop)
{
	register i32 c;
	register char *cs;

	cs = s;
	while (--n > 0 && (c = getc(iop)) >= 0) {
		*cs++ = c;
		if (c == '\n')
			break;
	}
	if (c < 0 && cs == s)
		return NULL;
	*cs++ = '\0';
	return s;
}

i32
puts(s)
register char *s;
{
	register i32 c;

	while (c = *s++)
		putchar(c);
	return putchar('\n');
}

i32
getchar()
{
	return getc(stdin);
}

i32
putchar(c)
i32 c;
{
	return putc(c, stdout);
}

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

i32
fread(char *ptr, usize size, usize count, FILE *iop)
{
	register i32 c;
	usize ndone, s;

	ndone = 0;
	if (size)
	for (; ndone < count; ndone++) {
		s = size;
		do {
			if ((c = getc(iop)) >= 0)
				*ptr++ = c;
			else
				return ndone;
		} while (--s);
	}
	return ndone;
}

usize
fwrite(ptr, size, count, iop)
usize size, count;
register char *ptr;
register FILE *iop;
{
	register usize s;
	usize ndone;

	ndone = 0;
	if (size)
	for (; ndone < count; ndone++) {
		s = size;
		do {
			putc(*ptr++, iop);
		} while (--s);
		if (ferror(iop))
			break;
	}
	return ndone;
}

off32
ftell(iop)
FILE *iop;
{
	off32 tres;
	register off32 adjust;

	if (iop->_cnt < 0)
		iop->_cnt = 0;
	if (iop->_flag & _IOREAD)
		adjust = - iop->_cnt;
	else if (iop->_flag & (_IOWRT | _IORW)) {
		adjust = 0;
		if (iop->_flag & _IOWRT && iop->_base && (iop->_flag & _IONBF) == 0)
			adjust = iop->_ptr - iop->_base;
	} else
		return -1;
	tres = lseek(fileno(iop), 0L, 1);
	if (tres < 0)
		return tres;
	tres += adjust;
	return tres;
}

i32
fseek(iop, offset, ptrname)
		register FILE *iop;
		off32 offset;
		i32 ptrname;
{
	register i32 c;
	off32 p;

	iop->_flag &= ~_IOEOF;
	if (iop->_flag & _IOREAD) {
		if (ptrname < 2 && iop->_base && !(iop->_flag & _IONBF)) {
			c = iop->_cnt;
			p = offset;
			if (ptrname == 0)
				p += c - lseek(fileno(iop), 0L, 1);
			else
				offset -= c;
			if (!(iop->_flag & _IORW) && c > 0 && p <= c
			    && p >= iop->_base - iop->_ptr) {
				iop->_ptr += (i32)p;
				iop->_cnt -= (i32)p;
				return 0;
			}
		}
		if (iop->_flag & _IORW) {
			iop->_ptr = iop->_base;
			iop->_flag &= ~_IOREAD;
		}
		p = lseek(fileno(iop), offset, ptrname);
		iop->_cnt = 0;
	} else if (iop->_flag & (_IOWRT | _IORW)) {
		fflush(iop);
		if (iop->_flag & _IORW) {
			iop->_cnt = 0;
			iop->_flag &= ~_IOWRT;
			iop->_ptr = iop->_base;
		}
		p = lseek(fileno(iop), offset, ptrname);
	}
	return p == -1 ? -1 : 0;
}

void
rewind(iop)
	register struct _iobuf *iop;
{
	fflush(iop);
	lseek(fileno(iop), 0L, 0);
	iop->_cnt = 0;
	iop->_ptr = iop->_base;
	iop->_flag &= ~(_IOERR | _IOEOF);
	if (iop->_flag & _IORW)
		iop->_flag &= ~(_IOREAD | _IOWRT);
}

i32
_doprnt(fmt, argp, stream)
char *fmt;
char *argp;
register FILE *stream;
{
	register i32 c;
	i32 total;

	total = 0;
	while ((c = *fmt++) != '\0') {
		i32 left;
		i32 zero;
		i32 width;
		i32 precision;
		i32 longflag;
		i32 negative;
		u32 uvalue;
		i32 svalue;
		char chbuf[1];

		if (c != '%') {
			putc(c, stream);
			total++;
			continue;
		}
		left = 0;
		zero = 0;
		width = 0;
		precision = -1;
		longflag = 0;
		for (;;) {
			if (*fmt == '-') {
				left = 1;
				fmt++;
				continue;
			}
			if (*fmt == '0') {
				zero = 1;
				fmt++;
				continue;
			}
			break;
		}
		if (*fmt == '*') {
				width = *(i16 *)argp;
				argp += sizeof(i16);
			if (width < 0) {
				left = 1;
				width = -width;
			}
			fmt++;
		} else {
			while (_doprnt_isdigit(*fmt))
				width = width * 10 + (*fmt++ - '0');
		}
		if (*fmt == '.') {
			precision = 0;
			fmt++;
			if (*fmt == '*') {
					precision = *(i16 *)argp;
					argp += sizeof(i16);
				if (precision < 0)
					precision = -1;
				fmt++;
			} else {
				while (_doprnt_isdigit(*fmt))
					precision = precision * 10 + (*fmt++ - '0');
			}
		}
		while (*fmt == 'l' || *fmt == 'L' || *fmt == 'h') {
			if (*fmt == 'l' || *fmt == 'L')
				longflag = 1;
			fmt++;
		}
		c = *fmt++;
		if (c == '%') {
			putc('%', stream);
			total++;
		} else if (c == 'c') {
				chbuf[0] = *(i16 *)argp;
				argp += sizeof(i16);
			total += _doprnt_print_chars(stream, chbuf, 1, width, left, 0);
		} else if (c == 's') {
			total += _doprnt_print_string(stream, *(char **)argp, width, precision, left);
			argp += sizeof(char *);
		} else if (c == 'd' || c == 'i') {
			if (longflag) {
				svalue = *(i32 *)argp;
				argp += sizeof(i32);
			} else {
				svalue = (i32)*(i16 *)argp;
				argp += sizeof(i16);
			}
			negative = svalue < 0;
			if (negative)
				uvalue = (u32)(-svalue);
			else
				uvalue = (u32)svalue;
			total += _doprnt_print_number(stream, uvalue, negative, 10, 0,
			    width, precision, left, zero);
		} else if (c == 'u') {
			if (longflag) {
				uvalue = *(u32 *)argp;
				argp += sizeof(u32);
			} else {
				uvalue = (u32)(u16)*(i16 *)argp;
				argp += sizeof(i16);
			}
			total += _doprnt_print_number(stream, uvalue, 0, 10, 0,
			    width, precision, left, zero);
		} else if (c == 'o' || c == 'O') {
			if (longflag || c == 'O') {
				uvalue = *(u32 *)argp;
				argp += sizeof(u32);
			} else {
				uvalue = (u32)(u16)*(i16 *)argp;
				argp += sizeof(i16);
			}
			total += _doprnt_print_number(stream, uvalue, 0, 8, 0,
			    width, precision, left, zero);
		} else if (c == 'x') {
			if (longflag) {
				uvalue = *(u32 *)argp;
				argp += sizeof(u32);
			} else {
				uvalue = (u32)(u16)*(i16 *)argp;
				argp += sizeof(i16);
			}
			total += _doprnt_print_number(stream, uvalue, 0, 16, 0,
			    width, precision, left, zero);
		} else if (c == 'X') {
			if (longflag) {
				uvalue = *(u32 *)argp;
				argp += sizeof(u32);
			} else {
				uvalue = (u32)(u16)*(i16 *)argp;
				argp += sizeof(i16);
			}
			total += _doprnt_print_number(stream, uvalue, 0, 16, 1,
			    width, precision, left, zero);
		} else if (c == 'D' || c == 'U') {
			if (c == 'D')
				longflag = 1;
			if (c == 'U') {
				uvalue = *(u32 *)argp;
				argp += sizeof(u32);
				total += _doprnt_print_number(stream, uvalue, 0, 10, 0,
				    width, precision, left, zero);
			} else {
				svalue = *(i32 *)argp;
				argp += sizeof(i32);
				negative = svalue < 0;
				if (negative)
					uvalue = (u32)(-svalue);
				else
					uvalue = (u32)svalue;
				total += _doprnt_print_number(stream, uvalue, negative, 10, 0,
				    width, precision, left, zero);
			}
		} else {
			putc('%', stream);
			putc(c, stream);
			total += 2;
		}
	}
	return total;
}

static i32
_doprnt_isdigit(c)
i32 c;
{
	return c >= '0' && c <= '9';
}

static void
_doprnt_udivmod(value, base, qout, rout)
u32 value;
i32 base;
u32 *qout;
u32 *rout;
{
	u32 quotient;
	u32 remainder;
	u32 bit;
	u32 divisor;

	quotient = 0;
	remainder = 0;
	divisor = (u32)base;
	for (bit = 0; bit < 32; bit++) {
		remainder <<= 1;
		if (value & 0x80000000UL)
			remainder |= 1UL;
		value <<= 1;
		quotient <<= 1;
		if (remainder >= divisor) {
			remainder -= divisor;
			quotient |= 1UL;
		}
	}
	*qout = quotient;
	*rout = remainder;
}

static void
_doprnt_putnchars(stream, ch, count)
register FILE *stream;
i32 ch;
i32 count;
{
	while (count-- > 0)
		putc(ch, stream);
}

static i32
_doprnt_utoa(value, base, upper, buf)
u32 value;
i32 base;
i32 upper;
char *buf;
{
	register i32 digit;
	register i32 len;
	u32 quotient;
	u32 remainder;

	len = 0;
	do {
		_doprnt_udivmod(value, base, &quotient, &remainder);
		value = quotient;
		digit = (i32)remainder;
		if (digit < 10)
			buf[len++] = digit + '0';
		else if (upper)
			buf[len++] = digit - 10 + 'A';
		else
			buf[len++] = digit - 10 + 'a';
	} while (value != 0);
	return len;
}

static i32
_doprnt_print_string(stream, str, width, precision, left)
register FILE *stream;
register char *str;
i32 width;
i32 precision;
i32 left;
{
	register i32 len;
	register i32 pad;
	register i32 total;

	if (str == NULL)
		str = "(null)";
	len = 0;
	while (str[len] && (precision < 0 || len < precision))
		len++;
	total = len;
	if (width > total)
		total = width;
	pad = width - len;
	if (!left && pad > 0)
		_doprnt_putnchars(stream, ' ', pad);
	while (len-- > 0)
		putc(*str++, stream);
	if (left && pad > 0)
		_doprnt_putnchars(stream, ' ', pad);
	return total;
}

static i32
_doprnt_print_chars(stream, str, len, width, left, zero)
register FILE *stream;
register char *str;
i32 len;
i32 width;
i32 left;
i32 zero;
{
	register i32 pad;
	register i32 total;
	register i32 fill;

	total = len;
	if (width > total)
		total = width;
	pad = width - len;
	fill = zero ? '0' : ' ';
	if (!left && pad > 0)
		_doprnt_putnchars(stream, fill, pad);
	while (len-- > 0)
		putc(*str++, stream);
	if (left && pad > 0)
		_doprnt_putnchars(stream, ' ', pad);
	return total;
}

static i32
_doprnt_print_number(stream, uvalue, negative, base, upper, width, precision, left, zero)
register FILE *stream;
u32 uvalue;
i32 negative;
i32 base;
i32 upper;
i32 width;
i32 precision;
i32 left;
i32 zero;
{
	char digits[32];
	char prefix[2];
	register i32 len;
	register i32 preczeros;
	register i32 pad;
	register i32 total;
	register i32 i;

	len = _doprnt_utoa(uvalue, base, upper, digits);
	if (precision == 0 && uvalue == 0)
		len = 0;
	prefix[0] = 0;
	prefix[1] = 0;
	if (negative)
		prefix[0] = '-';
	preczeros = 0;
	if (precision > len)
		preczeros = precision - len;
	pad = width - len - preczeros - (prefix[0] ? 1 : 0);
	if (precision >= 0)
		zero = 0;
	total = len + preczeros + (prefix[0] ? 1 : 0);
	if (width > total)
		total = width;
	if (!left && !zero && pad > 0)
		_doprnt_putnchars(stream, ' ', pad);
	if (prefix[0])
		putc(prefix[0], stream);
	if (!left && zero && pad > 0)
		_doprnt_putnchars(stream, '0', pad);
	if (preczeros > 0)
		_doprnt_putnchars(stream, '0', preczeros);
	for (i = len - 1; i >= 0; i--)
		putc(digits[i], stream);
	if (left && pad > 0)
		_doprnt_putnchars(stream, ' ', pad);
	return total;
}

i32
printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_doprnt(fmt, args, stdout);
	va_end(args);
	return ferror(stdout) ? EOF : 0;
}

i32
fprintf(FILE *iop, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_doprnt(fmt, args, iop);
	va_end(args);
	return ferror(iop) ? EOF : 0;
}

char *
sprintf(char *str, char *fmt, ...)
{
	va_list args;
	struct _iobuf _strbuf;

	_strbuf._flag = _IOWRT + _IOSTRG;
	_strbuf._ptr = str;
	_strbuf._cnt = 32767;
	va_start(args, fmt);
	_doprnt(fmt, args, &_strbuf);
	va_end(args);
	putc('\0', &_strbuf);
	return str;
}

i32
vprintf(fmt, args)
char *fmt;
va_list args;
{
	return _doprnt(fmt, (char *)args, stdout);
}

i32
vfprintf(iop, fmt, args)
FILE *iop;
char *fmt;
va_list args;
{
	i32 count;

	count = _doprnt(fmt, (char *)args, iop);
	return ferror(iop) ? EOF : count;
}
