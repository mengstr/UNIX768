#include "types.h"

extern void *malloc(usize n);

usize
strlen(const char *s)
{
	const char *p;

	for (p = s; *p != '\0'; p++)
		;
	return (usize)(p - s);
}

char *
strcpy(char *dst, const char *src)
{
	char *start;

	start = dst;
	while ((*dst++ = *src++) != '\0')
		;
	return start;
}

char *
strdup(const char *s)
{
	char *p;

	p = (char *)malloc(strlen(s) + 1);
	if (p == (char *)0)
		return (char *)0;
	return strcpy(p, s);
}

char *
strncpy(char *dst, const char *src, usize n)
{
	char *start;

	start = dst;
	while (n != 0 && *src != '\0') {
		*dst++ = *src++;
		n--;
	}
	while (n != 0) {
		*dst++ = '\0';
		n--;
	}
	return start;
}

char *
strcat(char *dst, const char *src)
{
	strcpy(dst + strlen(dst), src);
	return dst;
}

i32
strcmp(const char *a, const char *b)
{
	while (*a != '\0' && *a == *b) {
		a++;
		b++;
	}
	return ((u8)*a) - ((u8)*b);
}

i32
strncmp(const char *a, const char *b, usize n)
{
	while (n != 0 && *a != '\0' && *a == *b) {
		a++;
		b++;
		n--;
	}
	if (n == 0)
		return 0;
	return ((u8)*a) - ((u8)*b);
}

char *
strchr(const char *s, i32 c)
{
	while (*s != '\0') {
		if (*s == (char)c)
			return (char *)s;
		s++;
	}
	if (c == 0)
		return (char *)s;
	return (char *)0;
}

char *
strrchr(const char *s, i32 c)
{
	const char *last;

	last = 0;
	do {
		if (*s == (char)c)
			last = s;
	} while (*s++ != '\0');
	return (char *)last;
}
