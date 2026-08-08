#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

char line[500];
char bline[500];

int
main(void)
{
	double p;
	register char *l;
	time_t t;
	FILE *f;

	f = fopen("/usr/games/lib/fortunes", "r");
	if (f == NULL) {
		printf("Memory fault -- core dumped\n");
		exit(1);
	}
	time(&t);
	srand(getpid() + (i32)((t >> 16) + t));
	p = 1.;
	for(;;) {
		l = fgets(line, 500, f);
		if(l == NULL)
			break;
		if(rand() < 32768./p)
			strcpy(bline, line);
		p += 1.;
	}
	fputs(bline, stdout);
	return(0);
}
