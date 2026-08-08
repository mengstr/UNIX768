/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef FIELD_H_
#define FIELD_H_

#include "arith.h"

/* $Id$ */
/* FIELD DESCRIPTOR */

struct field
{ /* for field specifiers	*/
	arith fd_mask;
	int fd_shift;
	int fd_width;
	struct sdef* fd_sdef; /* upward pointer	*/
};


/* allocation definitions of struct field */
extern struct field *h_field;
#ifdef DEBUG
extern int cnt_field;
extern char *std_alloc();
#define new_field() ((struct field *) std_alloc((char **)&h_field, sizeof(struct field), 10, &cnt_field))
#else
#define	new_field() ((struct field *) st_alloc((char **)&h_field, sizeof(struct field), 10))
#endif
#define	free_field(p) st_free(p, &h_field, sizeof(struct field))


#ifndef LINT

#ifndef NOBITFIELD

struct expr;

void store_field(
    register struct field* fd, int uns, int code, register struct expr* leftop, arith tmpvar);
void eval_field(struct expr* expr, int code);

#endif /* NOBITFIELD */

#endif /* LINT */

#endif /* FIELD_H_ */