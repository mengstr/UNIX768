/*
	RED buffer routines -- Full C version (with error file support)
	Part 1 -- goto, output and status routines.

	Source:  red10.c
	Version: February 4, 1985, November 19, 1985; May 9, 1986

	Copyright (C) 1983, 1984, 1985, 1986 by Enteleki, Inc.
	All Rights Reserved

	Additions by Mark D. Lougheed APR-2020
*/

#include "red.h"


/*
	Boundary conditions:

	1.  Only bufins() can extend the buffer, NOT
	    bufgo() and bufdn().

	2.  bufatbot() is true when the current line is
	    PASSED the last line of the buffer.  Both
	    bufgo() and bufdn() can cause bufatbot() to
	    become true.  bufgetln() returns a zero length
	    line if bufatbot() is true.

	3.  b_max_line is the number of lines in the buffer.
	    However, b_line == b_max_line + 1 is valid and
	    it means that b_line points at a null line.

	4.  All buffer routines assume that the variables
            b_bp, b_line and b_start describe the current
	    line when the routine is called.  Thus, any
	    routine which changes the current line must
	    update these variables.
*/


/*
	## bufatbot()
	Return YES if at bottom of buffer (past the last line).
*/

int
bufatbot (void)
{
	return (b_line > b_max_line);
}


/*
	## bufattop()
	Return YES if at top of buffer.
*/

int
bufattop (void)
{
	return (b_line == 1);
}


/*
	## bufchng()
	Return YES if the buffer has been changed.
*/

int
bufchng (void)
{
	return b_cflag;
}


/*
	## buf_clr()
	Clear the slot table.  This should be done after
	read_file() overwrites  the slot table.
*/

int
buf_clr (void)
{
	struct BLOCK *bp;
	int i;

	for (i = 0; i < DATA_RES; i++) {
		bp = b_bpp [i];
		bp -> d_back   = ERROR;
		bp -> d_next   = ERROR;
		bp -> d_lines  = 0;
		bp -> d_status = FREE;
		bp -> d_lru    = i;
		bp -> d_diskp  = ERROR;
	}
}


/*
	## bufdn()
	Move towards end of buffer.
*/

int
bufdn (void)
{
	if (bufatbot()) {
		return;
	}
	else {
		b_line++;
		buf_gofw();
	}
}


/*
	## bufend()
	Clean up any temporary files.
*/

int
bufend (void)
{
	if (b_data_fd != ERROR) {
		sysclose(b_data_fd);	/* Bug fix: 6/17/84 */
		b_data_fd = ERROR;	/* Bug fix: 6/17/84 */
		sysunlink(DATA_FILE);
	}
}


/*
	## bufgo(n)
	Go to line n.
	Set b_bp, b_line, b_start.
*/

int
bufgo (int n)
{
	int distance, oldline;

	/* Put the request in range. */
	oldline  = b_line;
	b_line   = min(n, b_max_line + 1);
	b_line   = max(1, b_line);
	distance = b_line - oldline;

	if (distance == 0) {

		/* We are already at the requested line. */
		return;
	}
	else if (distance == 1) {

		/* Go forward from here. */
		buf_gofw();
		return;
	}
	else if (distance == -1) {

		/* Go back from here. */
		buf_gobk();
		return;
	}
	else if (distance > 0) {
		if ( b_line >
		     oldline + ((b_max_line - oldline) / 2)
		   ) {

			/* Search back from end of file. */
			b_bp  = swap_in(b_tail);
			b_start =
			    1 + b_max_line - b_bp -> d_lines;
			buf_gobk();
			return;
		}
		else {

			/* Search forward from here. */
			buf_gofw();
			return;
		}
	}
	else {
		if (b_line < oldline / 2) {

			/* Search from start of file. */
			b_bp    = swap_in(b_head);
			b_start = 1;
			buf_gofw();
			return;
		}
		else {

			/* Search back from here. */
			buf_gobk();
			return;
		}
	}
}


/*
	## buf_gobk()
	Search backwards from block for b_line.
	The starting line number of the block is b_start.
	Set b_bp and b_start.
*/

int
buf_gobk (void)
{
	int diskp;

	if (b_bp == (struct BLOCK *)0 ||
	    b_start < 1 || b_start > b_max_line ||
	    b_line  < 1 || b_line  > b_max_line + 1) {

		cant_happen("buf_gobk 1");
	}

	/* Scan backward for the proper block. */
	while (b_start > b_line) {

		/* Get the previous block in memory. */
		diskp = b_bp -> d_back;
		if (diskp == ERROR) {
			cant_happen("buf_gobk 2");
		}
		b_bp = swap_in(diskp);

		/* Calculate the start of the next block. */
		b_start -= b_bp -> d_lines;
		if (b_start <= 0) {
			cant_happen("buf_gobk 3");
		}
	}
}


/*
	## buf_gofw()
	Search forward from parcel par for line n.
	Set b_bp and b_start.
*/

int
buf_gofw (void)
{
	int diskp;

	/* The last line is always null. */
	if (bufatbot()) {
		return;
	}

	if (b_bp == (struct BLOCK *)0 ||
	    b_start < b_start ||
	    b_start < 1 || b_start > b_max_line ||
	    b_line  < 1 || b_line  > b_max_line + 1) {

		cant_happen("buf_gofw 1");
	}

	/* Scan forward to the proper block. */
	while (b_start + b_bp -> d_lines <= b_line) {

		/* Get the start of the next block. */
		b_start += b_bp -> d_lines;

		/* Swap in the next block. */
		diskp = b_bp -> d_next;
		if (diskp == ERROR || b_start > b_max_line) {
			cant_happen("buf_gofw 2");
		}
		b_bp = swap_in(diskp);
	}
}


/*
	## bufinit()
	Initialize the buffer module.
	If the work file exists,  read block 0 into slot 0.
	Otherwise,  call bufnew to clear everything.
*/

int
bufinit (void)
{
	struct BLOCK *bp;
	int i;
	char *p;

	b_line = 1;

	/* The data file has not been opened yet. */
	b_data_fd = ERROR;

	/* Dynamically allocate all slots. */
	for (i = 0; i < MAX_RES; i++) {

		p = sysalloc (BLOCK_SIZE);
		if (p == 0) {
			break;
		}

#ifdef CAST
		b_bpp [i] = (struct BLOCK *) p;
#else
		b_bpp [i] = p;
#endif

	}

	/* Set pseudo constant. */
	DATA_RES = i;

	/* The code requires at least three buffers. */
	if (DATA_RES < 3) {
		error("Not enough room for buffers.");
		sysend();
		exit(1);
	}


	bufnew();

}


/*
	## bufln()
	Return the current line number.
*/

int
bufln (void)
{
	return b_line;
}


/*
	Return the maximum line number.
*/

int
bufmax (void)
{
	return b_max_line;
}


/*
	## bufnew()
	Clear the buffer module.
*/

int
bufnew (void)
{
	struct BLOCK *bp;

	/* Free all slots. */
	buf_clr();

	/* Allocate the first slot. */
	b_bp        = b_bpp [0];
	b_head      = 0;
	b_tail      = 0;
	b_max_diskp = 0;
	b_max_put   = 0;
	b_bp -> d_diskp  = 0;
	b_bp -> d_status = DIRTY;

	/* Make sure temp file is erased. */
	if (b_data_fd != ERROR) {
		sysclose(b_data_fd);
		b_data_fd = ERROR;
		sysunlink(DATA_FILE);
	}
	b_free = ERROR;

	/* Set the current and last line counts. */
	b_line     = 1;
	b_max_line = 0;
	b_start    = 1;

	/* Indicate that the buffer has not been changed */
	b_cflag = NO;

	/* Do not erase work file on disk error. */
	b_fatal = NO;
}


/*
	## bufnrbot()
	Return YES if buffer is near the bottom line.
*/

int
bufnrbot (void)
{
	return (b_line >= b_max_line);
}


/*
	## bufout(topline, topy, nlines)
	Put nlines lines from buffer starting with line topline at
	position topy of the screen.
*/

int
bufout (int topline, int topy, int nlines)
{
	int l, x, y;

	x = outgetx();
	y = outgety();
	l = b_line;

	while (nlines > 0) {
		outxy(0, topy++);
		bufoutln(topline++);
		nlines--;
		if (hasint == YES) {
			sysintr(topline,topy,nlines);
			break;
		}
	}
	outxy(x,y);
	bufgo(l);
}


/*
	## bufoutln(line)
	Print one line on screen.
*/

int
bufoutln (int line)
{
	char buffer [MAXLEN1];
	int n;

	bufgo(line);

	if ( (b_max_line == 0 && line == 2) ||
	     (b_max_line >  0 && line == b_max_line + 1)) {
		fmtsout("---------------- End of file. ----------------",0);
		outdeol();
	}
	else if (line > b_max_line) {
		outdeol();
	}
	else {
		n = bufgetln(buffer, MAXLEN);
		n = min(n, MAXLEN);
		buffer [n] = NEWLINE;
		fmtsout(buffer, 0);
		outdeol();
	}
}



/*
	#bufrepl(line, n)
	Replace current line with the line that p points to.
	The new line is of length n.
*/

int
bufrepl (char line[], int n)
{
	/* Do not replace null line.  Just insert. */
	if (bufatbot()) {
		bufins(line, n);
		return;
	}

	/* Do not combine blocks until after insertion. */
	buf_d(1);
	bufins(line, n);
	combine();
}


/*
	## bufsaved()
	Indicate that the file has been saved.
*/

int
bufsaved (void)
{
	b_cflag = NO;
}


/*
	## bufup()
	Move towards the head of the file.
*/

int
bufup (void)
{
	if (bufattop()) {
		return;
	}
	else {
		b_line--;
		buf_gobk();
	}
}
