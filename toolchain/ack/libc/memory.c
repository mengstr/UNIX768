#include "types.h"

void *
memcpy(void *dst, const void *src, usize n)
{
	char *d;
	const char *s;

	d = (char *)dst;
	s = (const char *)src;
	while (n != 0) {
		*d++ = *s++;
		n--;
	}
	return dst;
}

void *
memmove(void *dst, const void *src, usize n)
{
	char *d;
	const char *s;

	d = (char *)dst;
	s = (const char *)src;
	if (d < s) {
		while (n != 0) {
			*d++ = *s++;
			n--;
		}
	} else {
		d += n;
		s += n;
		while (n != 0) {
			*--d = *--s;
			n--;
		}
	}
	return dst;
}

void *
memset(void *dst, i32 c, usize n)
{
	char *d;

	d = (char *)dst;
	while (n != 0) {
		*d++ = (char)c;
		n--;
	}
	return dst;
}

i32
memcmp(const void *a, const void *b, usize n)
{
	const u8 *pa;
	const u8 *pb;

	pa = (const u8 *)a;
	pb = (const u8 *)b;
	while (n != 0) {
		if (*pa != *pb)
			return (i32)*pa - (i32)*pb;
		pa++;
		pb++;
		n--;
	}
	return 0;
}
