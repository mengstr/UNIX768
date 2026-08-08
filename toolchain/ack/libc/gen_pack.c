#include "types.h"

void
ltol3(char *cp, i32 *lp, i32 n)
{
	register i32 i;
	u8 *a;
	u8 *b;

	a = (u8 *)cp;
	for (i = 0; i < n; i++) {
		b = (u8 *)&lp[i];
		*a++ = b[1];
		*a++ = b[2];
		*a++ = b[3];
	}
}

void
l3tol(i32 *lp, char *cp, i32 n)
{
	register i32 i;
	u8 *a;
	u8 *b;

	b = (u8 *)cp;
	for (i = 0; i < n; i++) {
		a = (u8 *)&lp[i];
		a[0] = 0;
		a[1] = *b++;
		a[2] = *b++;
		a[3] = *b++;
	}
}
