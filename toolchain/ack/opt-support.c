/* V7 substitutes for the host-oriented system helpers used by opt. */
#include <stdio.h>

void
sys_setbinarymode(FILE *fp)
{
	(void)fp;
}

char *
sys_maketempfile(char *prefix, char *suffix)
{
	(void)prefix;
	(void)suffix;
	return "/tmp/ack.opt.tmp";
}
