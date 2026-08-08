#include "stdio_private.h"

char *
gets(char *str)
{
	char *p;
	i32 c;

	p = str;
	while ((c = fgetc(stdin)) != EOF && c != '\n')
		*p++ = (char)c;
	if (c == EOF && p == str)
		return NULL;
	*p = '\0';
	return str;
}
