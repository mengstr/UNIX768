#include "types.h"

char *
index(char *sp, i32 c)
{
	do {
		if (*sp == (char)c)
			return sp;
	} while (*sp++ != '\0');
	return (char *)0;
}

char *
rindex(char *sp, i32 c)
{
	char *r;

	r = (char *)0;
	do {
		if (*sp == (char)c)
			r = sp;
	} while (*sp++ != '\0');
	return r;
}
