#include "stdio_private.h"

char *
fgets(char *s, usize n, FILE *iop)
{
	register i32 c;
	register char *cs;

	cs = s;
	while (--n > 0 && (c = getc(iop)) >= 0) {
		*cs++ = c;
		if (c == '\n')
			break;
	}
	if (c < 0 && cs == s)
		return NULL;
	*cs++ = '\0';
	return s;
}

