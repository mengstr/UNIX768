#include <sys/inttypes.h>
#include "uart.h"

#define PAGER_LINES	20

static i32 pager_lines;

static i32
pager_pause(void)
{
	i32 c;

	uart_puts("--More--");
	c = uart_getc();
	uart_puts("\r        \r");
	if (c == 'q' || c == 'Q' || c == 0x1b)
		return(-1);
	pager_lines = 0;
	return(0);
}

void
pager_reset(void)
{
	pager_lines = 0;
}

i32
pager_putc(i32 c)
{
	uart_putc(c);
	if (c == '\n') {
		pager_lines++;
		if (pager_lines >= PAGER_LINES)
			return(pager_pause());
	}
	return(0);
}

i32
pager_write(char *buf, i32 count)
{
	i32 i;

	for (i = 0; i < count; i++) {
		if (pager_putc((unsigned char)buf[i]) < 0)
			return(-1);
	}
	return(count);
}
