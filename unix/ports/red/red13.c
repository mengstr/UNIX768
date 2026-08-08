/*
 * RED internal consistency checks.
 */

#include "red.h"

int
cant_happen(char *message)
{
	pmtmess(message, ": can't happen");
	syswait();
	sysend();
	exit(1);
	return ERROR;
}

int
check_block(char *message)
{
	int avail;
	int i;
	int nlines;
	int total;

	if (b_bp == (struct BLOCK *)0 ||
	    b_line < 0 || b_line > b_max_line + 1)
		return cant_happen(message);

	nlines = b_bp->d_lines;
	avail = b1_avail();
	if (nlines < 0 || nlines >= BUFF_SIZE || avail < 0)
		return cant_happen(message);

	for (i = 0; i < nlines; i++) {
		total = b1_tab(i);
		if (total < 0 || total > BUFF_SIZE - avail)
			return cant_happen(message);
	}
	return OK;
}
