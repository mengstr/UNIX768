#include "stdio_private.h"

i32
puts(s)
register char *s;
{
	register i32 c;

	while (c = *s++)
		putchar(c);
	return putchar('\n');
}

