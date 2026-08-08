#include "../h/param.h"
#include "../h/systm.h"
#include "../h/seg.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../../include/stdarg.h"

#pragma dontwarn 213
void printn(long n, i32 b);
void panic(char *s);
void prdev(char *str, i32 dev);
void deverror(struct buf *bp, i32 o1, i32 o2);
static void prf_core(char *fmt, va_list args);
/*
 * In case console is off,
 * panicstr contains argument to last
 * call to panic.
 */

char	*panicstr;

/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much
 * suspended.
 * Printf should not be used for chit-chat.
 */
static void
prf_core(register char *fmt, va_list args)
{
	register i32 c;
	char *s;

loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)va_arg(args, i32), c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		s = va_arg(args, char *);
		while(c = *s++)
			putchar(c);
	} else if (c == 'D') {
		printn(va_arg(args, long), 10);
	}
	goto loop;
}

void
printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	prf_core(fmt, args);
	va_end(args);
}

/*
 * Print an unsigned integer in base b.
 */
void printn(n, b)
long n;
i32 b;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		putchar('-');
		n = -n;
	}
	if(a = n/b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(i32)(n%b)]);
}

/*
 * Panic is called on unresolvable
 * fatal errors.
 * It syncs, prints "panic: mesg" and
 * then loops.
 */
void panic(s)
char *s;
{
	panicstr = s;
	update();
	printf("panic: %s\n", s);
	for(;;)
		idle();
}

/*
 * prdev prints a warning message of the
 * form "mesg on dev x/y".
 * x and y are the major and minor parts of
 * the device argument.
 */
void prdev(str, dev)
char *str;
i32 dev;
{

	printf("%s on dev %u/%u\n", str, major(dev), minor(dev));
}

/*
 * deverr prints a diagnostic from
 * a device driver.
 * It prints the device, block number,
 * and an octal word (usually some error
 * status register) passed as argument.
 */
void deverror(bp, o1, o2)
register struct buf *bp;
i32 o1;
i32 o2;
{
	prdev("err", bp->b_dev);
	printf("bn=%D er=%o,%o\n", bp->b_blkno, o1, o2);
}
