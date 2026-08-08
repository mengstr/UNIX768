# include "refer.h"
int
hash (s)
	char *s;
{
int c, n;
for(n=0; c= *s; s++)
	n += (c*n+ c << (n%4));
return(n>0 ? n : -n);
}
void
err (char *s, ...)
{
va_list ap;
fprintf(stderr, "Error: ");
va_start(ap, s);
vfprintf(stderr, s, ap);
va_end(ap);
putc('\n', stderr);
exit(1);
}
int
prefix(t, s)
	char *t, *s;
{
int c, d;
while ( (c= *t++) == *s++)
	if (c==0) return(1);
return(c==0 ? 1: 0);
}
char *
mindex(s, c)
	char *s;
	int c;
{
register char *p;
for( p=s; *p; p++)
	if (*p ==c)
		return(p);
return(0);
}
void *
zalloc(m,n)
	unsigned m, n;
{
	void *t;
# if D1
fprintf(stderr, "calling calloc for %d*%d bytes\n",m,n);
# endif
t = calloc(m,n);
# if D1
fprintf(stderr, "calloc returned %p\n", t);
# endif
return(t);
}

int
ref_getw(f)
	FILE *f;
{
	int value;
	if (fread((char *)&value, sizeof(value), 1, f) != 1)
		return(-1);
	return(value);
}

void
ref_putw(value, f)
	int value;
	FILE *f;
{
	if (fwrite((char *)&value, sizeof(value), 1, f) != 1)
		err("write error");
}
