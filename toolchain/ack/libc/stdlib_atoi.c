#include "types.h"

i32 atol(const char *s);

i32
atoi(const char *s)
{
	return (i32)atol(s);
}

i32
atol(const char *s)
{
	i32 n;
	i32 neg;

	while (*s == ' ' || *s == '\t' || *s == '\n')
		s++;
	neg = 0;
	if (*s == '-') {
		neg = 1;
		s++;
	} else if (*s == '+') {
		s++;
	}
	n = 0;
	while (*s >= '0' && *s <= '9') {
		n = (n << 3) + (n << 1) + (*s - '0');
		s++;
	}
	return neg ? -n : n;
}
