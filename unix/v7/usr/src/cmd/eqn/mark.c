#include "e.h"

void
mark(int p1) {
	markline = 1;
	printf(".ds %d \\k(97\\*(%d\n", p1, p1);
	yyval = p1;
	if(dbg)printf(".\tmark %d\n", p1);
}

void
lineup(int p1) {
	markline = 1;
	if (p1 == 0) {
		yyval = oalloc();
		printf(".ds %d \\h'|\\n(97u'\n", YV);
	}
	if(dbg)printf(".\tlineup %d\n", p1);
}
