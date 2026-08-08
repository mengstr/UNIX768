/*
 * platform/epoch68/emulator/V7 libc compatibility for cawf.
 *
 * Keep these local to the imported formatter until the functions are needed
 * broadly enough to belong in libc.
 */

char *
strchr(s, c)
char *s;
int c;
{
	do {
		if (*s == c)
			return(s);
	} while (*s++ != '\0');
	return(0);
}

char *
strrchr(s, c)
char *s;
int c;
{
	char *last;

	last = 0;
	do {
		if (*s == c)
			last = s;
	} while (*s++ != '\0');
	return(last);
}

char *
memcpy(dst, src, n)
char *dst;
char *src;
unsigned n;
{
	char *ret;

	ret = dst;
	while (n-- != 0)
		*dst++ = *src++;
	return(ret);
}

static int
lower(c)
int c;
{
	if (c >= 'A' && c <= 'Z')
		return(c - 'A' + 'a');
	return(c);
}

int
strcasecmp(a, b)
char *a;
char *b;
{
	int ca, cb;

	for (;;) {
		ca = lower(*a++);
		cb = lower(*b++);
		if (ca != cb || ca == 0)
			return(ca - cb);
	}
}

int
strncasecmp(a, b, n)
char *a;
char *b;
unsigned n;
{
	int ca, cb;

	while (n-- != 0) {
		ca = lower(*a++);
		cb = lower(*b++);
		if (ca != cb || ca == 0)
			return(ca - cb);
	}
	return(0);
}

unsigned
strcspn(s, reject)
char *s;
char *reject;
{
	char *p;
	unsigned n;

	n = 0;
	for (; *s != '\0'; s++, n++) {
		for (p = reject; *p != '\0'; p++)
			if (*s == *p)
				return(n);
	}
	return(n);
}
