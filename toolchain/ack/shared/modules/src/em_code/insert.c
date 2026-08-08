/* $Id$ */

/* Implementation of C_insertpart, C_beginpart, and C_endpart.
   Basic methodology: place the parts either in memory or on a temporary
   file, in the order received, and remember this order. Then, in a second
   "pass", write out the code.
   An optimization is possible: as long as the order in which the parts
   are received corresponds to the order in which they must be written,
   they can be written immediately.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "em_private.h"
#include "system.h"
/*#include "em_path.h"*/
#include "alloc.h"
#include "insert.h"

#ifdef ACK_TARGET_C1_TRACE
extern void c1_trace(char*);
extern void c1_trace_value(char*, long);
#define C1_TRACE(s) c1_trace(s)
#define C1_TRACE_VALUE(s, v) c1_trace_value((s), (long)(v))
#else
#define C1_TRACE(s) ((void)0)
#define C1_TRACE_VALUE(s, v) ((void)0)
#endif

/* To make this portable, the initial value here is NULL
 * and system is used to retrieve the correct directory
 */
char		*C_tmpdir = NULL;
static Part	*C_stable[TABSIZ];

static char	*C_old_top;
static char	*C_old_opp;

#ifndef INCORE
static FILE	*C_old_ofp;

static int getbyte(long b)
{
	/*	Get the byte at offset "b" from the start of the
		temporary file, and try to do so in an efficient way.
	*/
	static long	start_core, curr_pos;

	if (b < start_core || b >= curr_pos) {
		/* the block wanted is not in core, so get it */
		long nb = (b & ~(BUFSIZ - 1));
		int n;

		C_flush();
		fflush(C_ofp);
		if (nb != curr_pos) {
			if (fseek(C_tfr, nb, 0) != 0) {
				C_failed();
			}
			curr_pos = nb;
		}
		if (! C_ibuf) {
			C_ibuf = malloc(BUFSIZ);
		}
		n = fread(C_ibuf, 1, BUFSIZ, C_tfr);
		if (n <= 0) {
			C_failed();
		}
		curr_pos += n;
		start_core = nb;
	}

	return C_ibuf[(int) (b - start_core)];
}
#endif

static void C_out_parts(PartOfPart *pp);
static Part *C_findpart(int part);

static int outpart(int id)
{
	/*	Output part "id", if present.
	*/
	Part *p = C_findpart(id);

	C1_TRACE_VALUE("c1x: outpart id ", id);
	C1_TRACE_VALUE("c1x: outpart found ", p != 0);
	if (p) {
		C_out_parts(p->p_parts);
		p->p_parts = 0;
	}
	return 0;
}

static void C_out_parts(PartOfPart *pp)
{
	/*	Output the list of chunks started by "pp".
		The list is build in reverse order, so this routine is
		recursive.
	*/
	PartOfPart *prev = 0, *next;

	while (pp) {
		next = pp->pp_next;
		pp->pp_next = prev;
		prev = pp;
		pp = next;
	}
	pp = prev;

	while (pp) {
		C1_TRACE_VALUE("c1x: out chunk type ", pp->pp_type);
		if (pp->pp_type == INSERT) {
			C1_TRACE_VALUE("c1x: out inserted part ", pp->pp_id);
			(*C_outpart)(pp->pp_id);
		}
		else {
			/* copy the chunk to output */
			long b = pp->pp_begin;

			C1_TRACE_VALUE("c1x: out chunk begin ", pp->pp_begin);
			C1_TRACE_VALUE("c1x: out chunk end ", pp->pp_end);
			while (b < pp->pp_end) {
#ifdef INCORE
				/* C_BASE is not constant, put() may
				   move C_BASE, so each iteration of
				   this loop must read C_BASE again.
				*/
				put(C_BASE[b++]);
#else
				put(getbyte(b++));
#endif
			}
		}
		prev = pp;
		pp = pp->pp_next;
		free((char *) prev);
	}
}

static Part *C_findpart(int part)
{
	/*	Look for part "part" in the table.
		Return 0 if not present,
	*/
	Part *p = C_stable[part % TABSIZ];

	while (p && p->p_id != part) {
		p = p->p_next;
	}
	return p;
}




static int swttmp(void)
{
#ifndef INCORE
	if (C_tmpfile == NULL) {

		static char tmpbuf[FILENAME_MAX];
		char *p = tmpbuf;
		extern char *mktemp(char *);

		if (C_tmpdir == NULL)
		{
#ifdef ACK_EM_CODE_FILE_BACKED
			C_tmpdir = "/tmp";
#else
			C_tmpdir = sys_gettmpdir();
			assert(C_tmpdir != NULL);
#endif
		}
		strcpy(p, C_tmpdir);
		strcat(p, "/CodeXXXXX");
		mktemp(p);
		C_tmpfile = p;
		C_old_ofp = fopen(p, "w+b");
		if (!C_old_ofp) {
			C_failed();
		}
		C_tfr = fopen(p, "rb");
		if (!C_tfr) {
			C_failed();
		}
	}
	if (! C_ontmpfile) {
		FILE* p = C_ofp;

		C1_TRACE("c1x: swttmp enter\n");
		C_flush();
		C_ofp = C_old_ofp;
		C_old_ofp = p;
		C_ontmpfile = 1;
		C1_TRACE("c1x: swttmp complete\n");
	}
#else
	if (! C_ontmpfile) {
		char *p;

		p = C_opp;
		C_opp = C_old_opp;
		C_old_opp = p;

		p = C_top;
		C_top = C_old_top;
		C_old_top = p;
		C_ontmpfile = 1;
	}
#endif
	return 0;
}

static int swtout(void)
{
#ifndef INCORE
	if (C_ontmpfile) {
		FILE* p = C_ofp;

		C1_TRACE("c1x: swtout enter\n");
		C_flush();
		C_ofp = C_old_ofp;
		C_old_ofp = p;
		C_ontmpfile = 0;
		C1_TRACE("c1x: swtout complete\n");
	}
#else
	if (C_ontmpfile) {
		char *p;

		p = C_opp;
		C_opp = C_old_opp;
		C_old_opp = p;

		p = C_top;
		C_top = C_old_top;
		C_old_top = p;
		C_ontmpfile = 0;
	}
#endif
	return 0;
}

static int available(int part)
{
	/*	See if part "part", and all the parts it consists of,
		are available. Return 1 if they are, 0 otherwize
	*/
	Part *p = C_findpart(part);
	PartOfPart *pp;
	int retval = 1;

	if (p == 0) return 0;

	if (p->p_flags & BUSY) {
		/* recursive call ends up here, and this just should
		   not happen. It is an error of the programmer using
		   this module.
		*/
		C_internal_error();
	}

	p->p_flags |= BUSY;

	pp = p->p_parts;
	while (pp) {
		if (pp->pp_type == INSERT && ! available(pp->pp_id)) {
			retval = 0;
			break;
		}
		else	pp = pp->pp_next;
	}
	p->p_flags &= ~BUSY;
	return retval;
}

static Part *mkpart(int part)
{
	/*	Create a Part structure with id "part", and return a
		pointer to it, after checking that is does not exist
		already.
	*/
	Part *p = C_findpart(part);
	int index = part % TABSIZ;

	if (p != 0) {
		/* multiple defined part ... */
		C_internal_error();
	}

	p = (Part *) malloc(sizeof(Part));
	p->p_id = part;
	p->p_next = C_stable[index];
	C_stable[index] = p;
	p->p_parts = 0;
	p->p_flags = 0;
	p->p_prevpart = 0;
	return p;
}

static void end_partofpart(Part *p)
{
	/*	End the current chunk of part *p.
	*/

	if (p) {
		PartOfPart *pp = p->p_parts;

		pp->pp_end = C_current_out - C_BASE;
		if (pp->pp_begin == pp->pp_end) {
			/* nothing in this chunk, so give it back */
			p->p_parts = pp->pp_next;
			free((char *) pp);
		}
	}
}

static void resume(Part *p)
{
	/*	Resume part "p", by creating a new PartOfPart structure
		for it.
	*/
	PartOfPart *pp = (PartOfPart *) malloc(sizeof(PartOfPart));

	swttmp();
	C_curr_part = p;
	pp->pp_next = p->p_parts;
	p->p_parts = pp;
	pp->pp_type = TEXT;
	pp->pp_begin = C_current_out - C_BASE;
}

void C_insertpart(int part)
{
	/*	Insert part "part" in the current part. If C_sequential is
		still set and the part to be inserted is available now,
		just write it out.
	*/
	Part *p;
	PartOfPart *pp;

	C_outpart = outpart;
	C_swttmp = swttmp;
	C_swtout = swtout;
	C1_TRACE_VALUE("c1x: insertpart id ", part);
	C1_TRACE_VALUE("c1x: insertpart sequential ", C_sequential);
	if (C_sequential && available(part)) {
		outpart(part);
		return;
	}

	if (C_sequential) {
		/* stop the sequential stuff, by creating a part */
		C_sequential = 0;
		p = mkpart(0);
		C_curr_part = p;
	}
	else {
		p = C_curr_part;
		end_partofpart(p);
	}

	/* Now, add the insertion of "part" to the current part.  */
	pp = (PartOfPart *) malloc(sizeof(PartOfPart));
	pp->pp_next = p->p_parts;
	p->p_parts = pp;
	pp->pp_type = INSERT;
	pp->pp_id = part;
	resume(p);
}

void C_beginpart(int part)
{
	/*	Now follows the definition for part "part".
		Suspend the current part, and add part "part" to the
		table.
	*/
	Part *p;

	C1_TRACE_VALUE("c1x: beginpart id ", part);
	p = mkpart(part);

	C_outpart = outpart;
	C_swttmp = swttmp;
	C_swtout = swtout;

	end_partofpart(C_curr_part);

	p->p_prevpart = C_curr_part;
	resume(p);
}

void C_endpart(int part)
{
	/*	End the current part. The parameter "part" is just there
		for the checking. Do we really need it ???
	*/
	Part *p = C_curr_part;

	C1_TRACE_VALUE("c1x: endpart id ", part);
	if (p->p_id != part) {
		/* illegal C_endpart ... */
		C_internal_error();
	}

	end_partofpart(p);
	if (p->p_prevpart) resume(p->p_prevpart);
	else {
		C_curr_part = 0;
		swtout();
	}
}
