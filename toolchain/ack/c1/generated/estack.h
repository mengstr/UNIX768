/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef ESTACK_H_
#define ESTACK_H_

/* $Id$ */
/* EXPRESSION STACK */
/* Used for global initializations */
struct e_stack
{
	struct e_stack* next;
	arith s_cnt1, s_cnt2;
	struct sdef* s_def;
	struct type** s_tpp;
	short s_nested;
};


/* allocation definitions of struct e_stack */
extern struct e_stack *h_e_stack;
#ifdef DEBUG
extern int cnt_e_stack;
extern char *std_alloc();
#define new_e_stack() ((struct e_stack *) std_alloc((char **)&h_e_stack, sizeof(struct e_stack), 5, &cnt_e_stack))
#else
#define	new_e_stack() ((struct e_stack *) st_alloc((char **)&h_e_stack, sizeof(struct e_stack), 5))
#endif
#define	free_e_stack(p) st_free(p, &h_e_stack, sizeof(struct e_stack))


#define bytes_upto_here s_cnt1
#define last_offset     s_cnt2
#define elem_count      s_cnt1
#define nelem           s_cnt2

#endif