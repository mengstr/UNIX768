#include "prf.h"
#include <sys/inttypes.h>
#include <stdarg.h>

static void format(i32 (*emit)(void *cookie, i32 c), void *cookie,
	char *fmt, va_list args);
static i32 emit_uart(void *cookie, i32 c);
static i32 emit_buf(void *cookie, i32 c);

static i32
emit_uart(cookie, c)
void *cookie;
i32 c;
{
	(void)cookie;
	putchar(c);
	return(0);
}

static i32
emit_buf(cookie, c)
void *cookie;
i32 c;
{
	char **dstp;

	dstp = (char **)cookie;
	*(*dstp)++ = c;
	return(0);
}

static void
format(emit, cookie, fmt, adx)
i32 (*emit)(void *cookie, i32 c);
void *cookie;
register char *fmt;
va_list adx;
{
	register char *p;
	i32 c, left, width, prec, islong, len;
	i32 val;
	u32 uval;
	char buf[12]; /* Large enough for 32-bit decimal + sign */
	char *s;

loop:
    while((c = *fmt++) != '%') {
        if(c == '\0') return;
        (*emit)(cookie, c);
    }

    /* Reset flags */
    left = width = islong = 0;
    prec = -1;

    if (*fmt == '-') { left++; fmt++; }
    while (*fmt >= '0' && *fmt <= '9')
        width = width * 10 + (*fmt++ - '0');
    if (*fmt == '.') {
        fmt++; prec = 0;
        while (*fmt >= '0' && *fmt <= '9')
            prec = prec * 10 + (*fmt++ - '0');
    }
    if (*fmt == 'l') { islong++; fmt++; }

    c = *fmt++;
    if(c == 'd' || c == 'D' || c == 'u' || c == 'o' || c == 'x') {
        i32 base = (c=='o'? 8: (c=='x'? 16: 10));

        /*
         * On this target, default argument promotion yields 16-bit int.
         * Only explicit long conversions consume a full 32-bit argument.
         */
        if (islong || c == 'D') {
            val = va_arg(adx, i32);
        } else {
            val = (i32)va_arg(adx, int);
        }

        /* Convert to string in local buffer (backwards) */
        p = buf;
        if ((c == 'd' || c == 'D') && val < 0) uval = -val;
        else uval = val;

        do {
            *p++ = "0123456789abcdef"[uval % base];
        } while (uval /= base);

        if ((c == 'd' || c == 'D') && val < 0) *p++ = '-';

        len = p - buf;

        /* Padding and printing */
        if (!left) while (width-- > len) (*emit)(cookie, ' ');
        while (p > buf) (*emit)(cookie, *--p);
        if (left) while (width-- > len) (*emit)(cookie, ' ');

    } else if(c == 's') {
        s = va_arg(adx, char *);
        len = 0;
        while (s[len] && (prec == -1 || len < prec)) len++;

        if (!left) while (width-- > len) (*emit)(cookie, ' ');
        for (c = 0; c < len; c++) (*emit)(cookie, s[c]);
        if (left) while (width-- > len) (*emit)(cookie, ' ');

    } else if (c == 'c') {
        c = (char)va_arg(adx, int);
        if (!left) while (width-- > 1) (*emit)(cookie, ' ');
        (*emit)(cookie, c);
        if (left) while (width-- > 1) (*emit)(cookie, ' ');
    }
    goto loop;
}

void
printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	format(emit_uart, (void *)0, fmt, args);
	va_end(args);
}

char *
sprintf(char *dst, char *fmt, ...)
{
	char *out;
	va_list args;

	out = dst;
	va_start(args, fmt);
	format(emit_buf, (void *)&dst, fmt, args);
	va_end(args);
	*dst = '\0';
	return(out);
}


#define UART0 ((volatile unsigned char *)0x00a00100)
#define UART_RBR_THR_DLL 0
#define UART_LSR         5
#define UART_LSR_DR      0x01
#define UART_LSR_THRE    0x20

void
putchar(c)
i32 c;
{
	if(c == '\n') {
		putchar('\r');
	}
	while((UART0[UART_LSR] & UART_LSR_THRE) == 0);
	UART0[UART_RBR_THR_DLL] = c;
}

i32
getchar(void)
{
	register i32 c;

	while((UART0[UART_LSR] & UART_LSR_DR) == 0);
	c = UART0[UART_RBR_THR_DLL] & 0177;
	if (c=='\r')
		c = '\n';
	putchar(c);
	return(c);
}

void
gets(buf)
char	*buf;
{
register char *lp;
register i32 c;

	lp = buf;
	for (;;) {
		c = getchar() & 0177;
		if (c>='A' && c<='Z')
			c -= 'A' - 'a';
		if (lp != buf && *(lp-1) == '\\') {
			lp--;
			if (c>='a' && c<='z') {
				c += 'A' - 'a';
				goto store;
			}
			switch ( c) {
			case '(':
				c = '{';
				break;
			case ')':
				c = '}';
				break;
			case '!':
				c = '|';
				break;
			case '^':
				c = '~';
				break;
			case '\'':
				c = '`';
				break;
			}
		}
	store:
		switch(c) {
		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
		case '\b':
		case '#':
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case '@':
			lp = buf;
			putchar('\n');
			continue;
		default:
			*lp++ = c;
		}
	}
}
