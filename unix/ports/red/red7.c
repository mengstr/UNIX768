/*
	RED prompt line module -- Full C version

	Source:  red7.c
	Version: October 22, 1983;  December 18, 1983

	Copyright (C) 1983 by Enteleki, Inc.
	All Rights Reserved

	Additions by Mark D. Lougheed APR-2020
*/
#include "red.h"

#ifdef HAS_STATIC
static int	pmt_zapped;
static int	pmt_hold;
static int	pmt_line;
static int	pmt_col;
static int	pmt_newfn;
static char	pmt_mode [MAXLEN];
static int	pmt_newmd;
#endif


/*
	Initialize this module.
*/

int
pmtclr (void)
{
	pmt_zapped = YES;
	pmt_hold   = NO;
	pmt_newmd  = YES;
	pmt_newfn  = YES;
}


/*
	Disable updates to the prompt line.
*/

int
pmthold (int flag)
{
	pmt_hold = flag;
}


/*
	Indicate that the prompt line has been overwritten.
*/

int
pmtzap (void)
{
	pmt_zapped = YES;
}


/*
	Update the prompt line as needed.
*/

int
pmtupd (void)
{
	int x, y;

	if (pmt_hold == YES) {
		return;
	}

	/* Save cursor postion. */
	x = outgetx();
	y = outgety();

	pmt1upd(x);

	/* Restore cursor position. */
	outxy(x, y);
}


int
pmt1upd (int x)
{
	int i;

	/* Clear the line if it may have been corrupted. */
	if (pmt_zapped == YES) {
		outxy(0, 0);
		outdelln();
	}

	/* Update the line field. */
	if (pmt_zapped == YES) {
		outxy(0, 0);
		fmtsout("line: ",0);
		putdec(bufln(), 5);
	}
	else if (bufln() != pmt_line) {
		outxy(6, 0);
		putdec(bufln(),5);
	}
	pmt_line = bufln();

	/* Update the column field. */
	if (hascol == YES && pmt_zapped == YES) {
		outxy(12, 0);
		fmtsout("column: ", 12);
		putdec(x, 3);
	}
	else if (hascol == YES &&  x != pmt_col) {
		outxy(20,0);
		putdec(x, 3);
	}
	pmt_col = x;

	/* Update the file name field. */
	if (pmt_zapped == YES || pmt_newfn == YES) {
		outxy(25,0);
		if (g_file [0] == EOS) {
			fmtsout(" ..no file.. ",25);
		}
		else {
			fmtsout(g_file,25);
			for (i = 0; i < 14 - strlen(g_file); i++) {
				fmtsout(" ", 0);
			}
		}
	}
	pmt_newfn = NO;

	/* Update the mode field. */
	if (pmt_zapped == YES || pmt_newmd == YES) {
		outxy(41,0);
		fmtsout(pmt_mode,41);
		outdeol();
	}
	pmt_newmd = NO;

	/* Clear the zapped flag. */
	pmt_zapped = NO;
}


/*
	Put error message on prompt line.
	Do NOT restore the cursor.
*/

int
pmtmess (char *s1, char *s2)
{
	/* Make sure line is correct. */
	pmt_zapped = YES;
	pmt1upd(outgetx());

	/* Output error messages. */
	outxy(41, 0);
	outdeol();
	fmtsout(" ", outgetx());
	fmtsout(s1,  outgetx());
	fmtsout(" ", outgetx());
	fmtsout(s2,  outgetx());

	/* Make sure the mode field gets restored. */
	pmt_newmd = YES;
}


/*
	Change the mode immediately.
*/

int
pmtmode (char *newmode)
{
	if (strcmp(newmode, pmt_mode) != 0) {
		strcpy(pmt_mode, newmode);
		pmt_newmd = YES;
		pmthold(NO);
	}
	pmtupd();	/* 12/18/83 */

}


/*
	Indicate that the global file name has been changed.
*/

int
pmtfn (void)
{
	pmt_newfn = YES;
}


/*
	Return the position of the next free spot on the line.
*/

int
pmtlast (void)
{
	return 41 + strlen(pmt_mode);
}
