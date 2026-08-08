
/*
RED terminal output module
Source:  red6.c

Customizations for VT-100 terminal by Mark D. Lougheed NOV-2020
*/

#include "red.h"


/*
Define the current coordinates of the cursor.
*/

#ifdef HAS_STATIC
static
int outx;
static
int outy;
#endif


/*
Return the current coordinates of the cursor.
*/

int
outgetx (void)
{
	return outx;
}

int
outgety (void)
{
	return outy;
}


/*
Initialize the globals which describe the terminal.
*/

int
outinit (void)
{
	hasdn   = YES;
	hasup   = YES;
	hasins  = YES;
	hasdel  = YES;
	hasint  = NO;
	hascol  = YES;
	haswrap = NO;
}


/*
Output one printable character to the screen.
*/

int
outchar (int c)
{
	syscout(c);
	outx++;
	return c;
}


/*
Position cursor to position x,y on screen.
0,0 is the top left corner.
*/

int
outxy (int x, int y)
{
	outx = x;
	outy = y;
	syscout(27);
	syscout('[');

	syscout('0'+((y+1)/10));  /* row tens */
	syscout('0'+((y+1)%10));  /* row units */

	syscout(';');

	syscout('0'+((x+1)/10));  /* column tens */
	syscout('0'+((x+1)%10));  /* column units */

	syscout('H');
}


/*
Erase the entire screen.
Make sure the rightmost column is erased.
*/

int
outclr (void)
{
	int i;

    for (i = 0; i < SCRNL; i++) {
		outxy(0, i);
		outdelln();
	}
	outxy(0,0);
}


/*
Delete the line on which the cursor rests.
Leave the cursor at the left margin.
*/

int
outdelln (void)
{
	outxy(0,outy);
	outdeol();
}


/*
Delete to end of line.
Assume the last column is blank.
*/

int
outdeol (void)
{
	syscout(27);
	syscout('[');
	syscout('K');
}


/*
Hardware insert line.
*/

int
outins (void)
{
	syscout(27);
	syscout('[');
	syscout('L');
}


/*
Hardware delete line.
*/

int
outdel (void)
{
	syscout(27);
	syscout('[');
	syscout('M');
}


/*
Scroll the screen up.
Assume the cursor is on the bottom line.
*/

int
outsup (void)
{
	/* auto scroll */
	outxy(0,SCRNL1);
	syscout(27);
	syscout('[');
	syscout('L');
}


/*
Scroll screen down.
Assume the cursor is on the top line.
*/

int
outsdn (void)
{
	/* auto scroll */
	outxy(0,0);
	syscout(27);
	syscout('[');
	syscout('M');
}
