#include "stdio_private.h"

#undef getchar

i32
getchar(void)
{
	return getc(stdin);
}
