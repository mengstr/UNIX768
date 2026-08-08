#include <stdarg.h>
#include "stdio_private.h"

static i32 _doprnt_isdigit(i32 c);
static void _doprnt_udivmod(u32 value, i32 base, u32 *qout, u32 *rout);
static void _doprnt_putnchars(FILE *stream, i32 ch, i32 count);
static i32 _doprnt_utoa(u32 value, i32 base, i32 upper, char *buf);
static i32 _doprnt_print_string(FILE *stream, char *str, i32 width, i32 precision, i32 left);
static i32 _doprnt_print_chars(FILE *stream, char *str, i32 len, i32 width, i32 left, i32 zero);
static i32 _doprnt_print_number(FILE *stream, u32 uvalue, i32 negative, i32 base, i32 upper, i32 width, i32 precision, i32 left, i32 zero);

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
		char floatbuf[512];

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
		} else if (c == 'f' || c == 'e' || c == 'E' ||
		    c == 'g' || c == 'G') {
			double value;
			char *end;
			i32 digits;
			i32 i;

			value = *(double *)argp;
			argp += sizeof(double);
			if (precision > 128)
				precision = 128;
			end = _fp_special(value, floatbuf);
			if (end == 0) {
				if (c == 'f')
					end = _pfloat(value, floatbuf,
					    precision < 0 ? 0 : precision,
					    precision >= 0);
				else if (c == 'e' || c == 'E')
					end = _pscien(value, floatbuf,
					    precision < 0 ? 0 : precision,
					    precision >= 0);
				else {
					digits = precision < 0 ? 6 : precision;
					if (digits == 0)
						digits = 1;
					(void)gcvt(value, digits, floatbuf);
					end = floatbuf;
					while (*end)
						end++;
				}
			}
			if (c == 'E' || c == 'G') {
				for (i = 0; &floatbuf[i] < end; i++)
					if (floatbuf[i] == 'e')
						floatbuf[i] = 'E';
			}
			total += _doprnt_print_chars(stream, floatbuf,
			    (i32)(end - floatbuf), width, left, zero);
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
