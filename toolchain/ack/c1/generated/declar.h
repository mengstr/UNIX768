/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef DECLAR_H_
#define DECLAR_H_

/* $Id$ */
/* DEFINITION OF DECLARATOR DESCRIPTORS */

/*	A 'declarator' consists of an idf and a linked list of
    language-defined unary operations: *, [] and (), called
    decl_unary's.
*/

struct declarator
{
	/* struct declarator *next; */
	struct idf* dc_idf;
	struct decl_unary* dc_decl_unary;
	struct formal* dc_formal; /* params for function	*/
};

struct formal
{ /* list of formals */
	struct formal* next;
	struct idf* fm_idf;
};


/* allocation definitions of struct formal */
extern struct formal *h_formal;
#ifdef DEBUG
extern int cnt_formal;
extern char *std_alloc();
#define new_formal() ((struct formal *) std_alloc((char **)&h_formal, sizeof(struct formal), 5, &cnt_formal))
#else
#define	new_formal() ((struct formal *) st_alloc((char **)&h_formal, sizeof(struct formal), 5))
#endif
#define	free_formal(p) st_free(p, &h_formal, sizeof(struct formal))


#define NO_PARAMS ((struct formal*)0)

struct decl_unary
{
	struct decl_unary* next;
	int du_fund; /* POINTER, ARRAY or FUNCTION	*/
	int du_typequal; /* CONST, VOLATILE, or 0 */
	arith du_count; /* for ARRAYs only	*/
	struct proto* du_proto; /* params for function or prototype */
};


/* allocation definitions of struct decl_unary */
extern struct decl_unary *h_decl_unary;
#ifdef DEBUG
extern int cnt_decl_unary;
extern char *std_alloc();
#define new_decl_unary() ((struct decl_unary *) std_alloc((char **)&h_decl_unary, sizeof(struct decl_unary), 10, &cnt_decl_unary))
#else
#define	new_decl_unary() ((struct decl_unary *) st_alloc((char **)&h_decl_unary, sizeof(struct decl_unary), 10))
#endif
#define	free_decl_unary(p) st_free(p, &h_decl_unary, sizeof(struct decl_unary))


extern struct declarator null_declarator;

#endif
