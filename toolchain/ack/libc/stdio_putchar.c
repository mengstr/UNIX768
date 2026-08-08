#include "stdio_private.h"

#undef putchar

i32
putchar(i32 c)
{
	return putc(c, stdout);
}
