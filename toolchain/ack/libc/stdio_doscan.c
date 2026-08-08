#include <stdarg.h>
#include "stdio_private.h"

#define SCAN_FILE	0
#define SCAN_STRING	1

struct scan_input {
	i32 type;
	FILE *fp;
	char *sp;
	i32 nread;
};

static i32 scan_get(struct scan_input *in);
static void scan_unget(struct scan_input *in, i32 c);
static i32 scan_isspace(i32 c);
static i32 scan_isdigit(i32 c);
static i32 scan_digit(i32 c);
static i32 scan_in_base(i32 c, i32 base);
static i32 scan_number(struct scan_input *in, i32 base, i32 width, i32 sign,
    i32 *negp, u32 *valuep);
static i32 scan_string(struct scan_input *in, char *out, i32 width);
static i32 scan_chars(struct scan_input *in, char *out, i32 width);
static i32 scan_scanset(struct scan_input *in, char *fmt, char **endp,
    char *out, i32 width);
static i32 scan_float(struct scan_input *in, i32 width, double *valuep);
static double scan_float_value(char *text);
static void scan_store_number(char *argp, i32 longflag, i32 sign, i32 neg,
    u32 value);
static i32 _doscan(struct scan_input *in, char *fmt, va_list args);

i32
_doscan_file(fp, fmt, args)
FILE *fp;
char *fmt;
va_list args;
{
	struct scan_input in;

	in.type = SCAN_FILE;
	in.fp = fp;
	in.sp = (char *)0;
	in.nread = 0;
	return _doscan(&in, fmt, args);
}

i32
_doscan_string(str, fmt, args)
char *str;
char *fmt;
va_list args;
{
	struct scan_input in;

	in.type = SCAN_STRING;
	in.fp = (FILE *)0;
	in.sp = str;
	in.nread = 0;
	return _doscan(&in, fmt, args);
}

static i32
_doscan(in, fmt, args)
struct scan_input *in;
char *fmt;
va_list args;
{
	i32 assigned;
	i32 c;

	assigned = 0;
	while (*fmt) {
		i32 suppress;
		i32 width;
		i32 longflag;
		i32 neg;
		i32 base;
		u32 value;
		char *argp;

		if (scan_isspace(*fmt)) {
			while (scan_isspace(*fmt))
				fmt++;
			do {
				c = scan_get(in);
			} while (scan_isspace(c));
			scan_unget(in, c);
			continue;
		}
		if (*fmt != '%') {
			c = scan_get(in);
			if (c != *fmt) {
				scan_unget(in, c);
				break;
			}
			fmt++;
			continue;
		}
		fmt++;
		if (*fmt == '%') {
			c = scan_get(in);
			if (c != '%') {
				scan_unget(in, c);
				break;
			}
			fmt++;
			continue;
		}

		suppress = 0;
		width = 0;
		longflag = 0;
		if (*fmt == '*') {
			suppress = 1;
			fmt++;
		}
		while (scan_isdigit(*fmt)) {
			width = width * 10 + (*fmt - '0');
			fmt++;
		}
		while (*fmt == 'l' || *fmt == 'L' || *fmt == 'h') {
			if (*fmt == 'l' || *fmt == 'L')
				longflag = 1;
			fmt++;
		}

		c = *fmt++;
		if (c != 'c' && c != '[') {
			do {
				base = scan_get(in);
			} while (scan_isspace(base));
			scan_unget(in, base);
		}

		argp = (char *)0;
		if (!suppress && c != '\0') {
			argp = *(char **)args;
			args += sizeof(char *);
		}

		if (c == 'd' || c == 'u' || c == 'o' || c == 'x' || c == 'X' ||
		    c == 'i' || c == 'D' || c == 'O') {
			if (c == 'D' || c == 'O')
				longflag = 1;
			if (c == 'o' || c == 'O')
				base = 8;
			else if (c == 'x' || c == 'X')
				base = 16;
			else if (c == 'i')
				base = 0;
			else
				base = 10;
			if (!scan_number(in, base, width, 1, &neg, &value))
				break;
			if (!suppress) {
				scan_store_number(argp, longflag,
				    c == 'd' || c == 'i' || c == 'D', neg, value);
				assigned++;
			}
		} else if (c == 's') {
			if (!scan_string(in, suppress ? (char *)0 : argp, width))
				break;
			if (!suppress)
				assigned++;
		} else if (c == 'c') {
			if (!scan_chars(in, suppress ? (char *)0 : argp,
			    width ? width : 1))
				break;
			if (!suppress)
				assigned++;
		} else if (c == '[') {
			if (!scan_scanset(in, fmt, &fmt, suppress ? (char *)0 : argp,
			    width))
				break;
			if (!suppress)
				assigned++;
		} else if (c == 'e' || c == 'E' || c == 'f' || c == 'F' ||
		    c == 'g' || c == 'G') {
			double fvalue;

			if (!scan_float(in, width, &fvalue))
				break;
			if (!suppress) {
				if (longflag || c == 'E' || c == 'F' || c == 'G')
					*(double *)argp = fvalue;
				else
					*(float *)argp = (float)fvalue;
				assigned++;
			}
		} else if (c == 'n') {
			if (!suppress) {
				if (longflag)
					*(i32 *)argp = in->nread;
				else
					*(i16 *)argp = (i16)in->nread;
			}
		} else {
			break;
		}
	}
	if (assigned == 0) {
		c = scan_get(in);
		if (c == EOF)
			return EOF;
		scan_unget(in, c);
	}
	return assigned;
}

static i32
scan_get(in)
struct scan_input *in;
{
	i32 c;

	if (in->type == SCAN_FILE)
		c = fgetc(in->fp);
	else {
		c = *in->sp;
		if (c != '\0')
			in->sp++;
		else
			c = EOF;
	}
	if (c != EOF)
		in->nread++;
	return c;
}

static void
scan_unget(in, c)
struct scan_input *in;
i32 c;
{
	if (c == EOF)
		return;
	if (in->type == SCAN_FILE)
		ungetc(c, in->fp);
	else
		in->sp--;
	in->nread--;
}

static i32
scan_isspace(c)
i32 c;
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
	    c == '\f' || c == '\v';
}

static i32
scan_isdigit(c)
i32 c;
{
	return c >= '0' && c <= '9';
}

static i32
scan_digit(c)
i32 c;
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static i32
scan_in_base(c, base)
i32 c;
i32 base;
{
	i32 d;

	d = scan_digit(c);
	return d >= 0 && d < base;
}

static i32
scan_number(in, base, width, sign, negp, valuep)
struct scan_input *in;
i32 base;
i32 width;
i32 sign;
i32 *negp;
u32 *valuep;
{
	i32 c;
	i32 ndigit;
	i32 limit;
	u32 value;

	limit = width ? width : 32767;
	*negp = 0;
	value = 0;
	ndigit = 0;
	if (limit > 0) {
		c = scan_get(in);
		if (sign && (c == '-' || c == '+')) {
			*negp = c == '-';
			limit--;
		} else
			scan_unget(in, c);
	}
	if (base == 0) {
		base = 10;
		c = scan_get(in);
		if (c == '0') {
			limit--;
			c = scan_get(in);
			if (limit > 0 && (c == 'x' || c == 'X')) {
				base = 16;
				limit--;
			} else {
				base = 8;
				scan_unget(in, c);
				limit++;
				scan_unget(in, '0');
			}
		} else
			scan_unget(in, c);
	}
	while (limit-- > 0) {
		c = scan_get(in);
		if (!scan_in_base(c, base)) {
			scan_unget(in, c);
			break;
		}
		value = value * (u32)base + (u32)scan_digit(c);
		ndigit++;
	}
	if (ndigit == 0)
		return 0;
	*valuep = value;
	return 1;
}

static i32
scan_string(in, out, width)
struct scan_input *in;
char *out;
i32 width;
{
	i32 c;
	i32 n;

	n = 0;
	if (width == 0)
		width = 32767;
	while (width-- > 0) {
		c = scan_get(in);
		if (c == EOF || scan_isspace(c)) {
			scan_unget(in, c);
			break;
		}
		if (out)
			*out++ = c;
		n++;
	}
	if (n == 0)
		return 0;
	if (out)
		*out = '\0';
	return 1;
}

static i32
scan_chars(in, out, width)
struct scan_input *in;
char *out;
i32 width;
{
	i32 c;
	i32 n;

	n = 0;
	while (width-- > 0) {
		c = scan_get(in);
		if (c == EOF)
			break;
		if (out)
			*out++ = c;
		n++;
	}
	return n > 0;
}

static i32
scan_scanset(in, fmt, endp, out, width)
struct scan_input *in;
char *fmt;
char **endp;
char *out;
i32 width;
{
	i32 invert;
	i32 c;
	i32 n;
	char set[128];

	for (n = 0; n < 128; n++)
		set[n] = 0;
	invert = 0;
	if (*fmt == '^') {
		invert = 1;
		fmt++;
	}
	if (*fmt == ']')
		set[(i32)*fmt++ & 0177] = 1;
	while (*fmt && *fmt != ']') {
		i32 first;
		i32 last;

		first = (i32)*fmt++ & 0177;
		if (*fmt == '-' && fmt[1] != '\0' && fmt[1] != ']' &&
		    first <= ((i32)fmt[1] & 0177)) {
			last = (i32)fmt[1] & 0177;
			fmt += 2;
			while (first <= last)
				set[first++] = 1;
		} else
			set[first] = 1;
	}
	if (*fmt == ']')
		fmt++;
	*endp = fmt;

	n = 0;
	if (width == 0)
		width = 32767;
	while (width-- > 0) {
		c = scan_get(in);
		if (c == EOF)
			break;
		if (!!set[c & 0177] == !!invert) {
			scan_unget(in, c);
			break;
		}
		if (out)
			*out++ = c;
		n++;
	}
	if (n == 0)
		return 0;
	if (out)
		*out = '\0';
	return 1;
}

static i32
scan_float(in, width, valuep)
struct scan_input *in;
i32 width;
double *valuep;
{
	char text[128];
	i32 limit;
	i32 digits;
	i32 exponent_digits;
	i32 c;
	char *p;
	char *exponent;

	limit = width ? width : 127;
	if (limit > 127)
		limit = 127;
	p = text;
	digits = 0;
	c = scan_get(in);
	if (limit > 0 && (c == '+' || c == '-')) {
		*p++ = c;
		limit--;
		c = scan_get(in);
	}
	while (limit > 0 && scan_isdigit(c)) {
		*p++ = c;
		digits++;
		limit--;
		c = scan_get(in);
	}
	if (limit > 0 && c == '.') {
		*p++ = c;
		limit--;
		c = scan_get(in);
		while (limit > 0 && scan_isdigit(c)) {
			*p++ = c;
			digits++;
			limit--;
			c = scan_get(in);
		}
	}
	if (digits == 0) {
		scan_unget(in, c);
		while (p > text)
			scan_unget(in, *--p);
		return 0;
	}
	if (limit > 0 && (c == 'e' || c == 'E')) {
		exponent = p;
		*p++ = c;
		limit--;
		c = scan_get(in);
		if (limit > 0 && (c == '+' || c == '-')) {
			*p++ = c;
			limit--;
			c = scan_get(in);
		}
		exponent_digits = 0;
		while (limit > 0 && scan_isdigit(c)) {
			*p++ = c;
			exponent_digits++;
			limit--;
			c = scan_get(in);
		}
		if (exponent_digits == 0) {
			scan_unget(in, c);
			while (p > exponent)
				scan_unget(in, *--p);
			c = scan_get(in);
		}
	}
	scan_unget(in, c);
	*p = '\0';
	*valuep = scan_float_value(text);
	return 1;
}

static double
scan_float_value(text)
char *text;
{
	double value;
	double scale;
	i32 negative;
	i32 exponent;
	i32 exponent_negative;

	negative = 0;
	if (*text == '+' || *text == '-') {
		negative = *text == '-';
		text++;
	}
	value = 0.0;
	while (scan_isdigit(*text))
		value = value * 10.0 + (*text++ - '0');
	if (*text == '.') {
		text++;
		scale = 0.1;
		while (scan_isdigit(*text)) {
			value += (*text++ - '0') * scale;
			scale *= 0.1;
		}
	}
	exponent = 0;
	exponent_negative = 0;
	if (*text == 'e' || *text == 'E') {
		text++;
		if (*text == '+' || *text == '-') {
			exponent_negative = *text == '-';
			text++;
		}
		while (scan_isdigit(*text)) {
			if (exponent < 1024) {
				exponent = exponent * 10 + (*text - '0');
				if (exponent > 1024)
					exponent = 1024;
			}
			text++;
		}
	}
	while (exponent-- > 0) {
		if (exponent_negative)
			value /= 10.0;
		else
			value *= 10.0;
	}
	return negative ? -value : value;
}

static void
scan_store_number(argp, longflag, sign, neg, value)
char *argp;
i32 longflag;
i32 sign;
i32 neg;
u32 value;
{
	i32 svalue;

	if (!sign && neg)
		value = (u32)0 - value;

	if (longflag) {
		if (sign) {
			svalue = neg ? -(i32)value : (i32)value;
			*(i32 *)argp = svalue;
		} else
			*(u32 *)argp = value;
	} else {
		if (sign) {
			svalue = neg ? -(i32)value : (i32)value;
			*(i16 *)argp = (i16)svalue;
		} else
			*(u16 *)argp = (u16)value;
	}
}
