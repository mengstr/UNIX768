# include <stdio.h>
# include <unistd.h>
# include "prep.h"

int	optr;

char	obuf[512];

int	nflush;

int
put(char *string, int n)
{
	int	i;
	char	*o;

	string--;

	if((i = optr + n - 512) >= 0) {
		n -= i;
		o = &obuf[optr] -1;
		while(--n >= 0)
			*++o = *++string;
		optr = 512;
		flsh();
		n = i;
	}

	o = &obuf[optr] - 1;
	optr += n;

	while(--n >= 0) {
		*++o = *++string;
	}
	return(0);
}

int
flsh(void)
{

	if(optr <= 0)	return(optr);

	nflush++;
	if(write(1,obuf,optr) != optr)
		return(-1);
	optr = 0;
	return(0);
}
