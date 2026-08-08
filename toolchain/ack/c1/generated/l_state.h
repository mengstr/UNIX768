/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint state stack	*/

/* These datastructures are used to implement a stack on which the
 * state of automatic variables (including register variables) is
 * kept.
 * In this way it is possible to account for the flow of
 * control of the program.
 */

#define TEST_VAR   0 /* not a constant */
#define TEST_TRUE  1 /* always true */
#define TEST_FALSE 2 /* always false */

struct loop_state
{ /* used in lint_end_state only */
	int lps_test; /* is the test a constant? */
	struct state* lps_body;
	struct state* lps_loop;
};

struct switch_state
{ /* used in lint_end_state only */
	struct state* sws_case;
	struct state* sws_break;
	int sws_default_met;
};

/*	This union describes the (possibly incomplete) state at the end of the
    mentioned construct.
*/
union lint_end_state
{ /* used in lint_stack_entry only */
	struct state* ule_if;
	struct loop_state ule_loop;
	struct switch_state ule_switch;
};

struct lint_stack_entry
{
	struct lint_stack_entry* next;
	struct lint_stack_entry* ls_previous;
	int ls_level;
	struct state* ls_current; /* used by all classes */
	short ls_class; /* IF, WHILE, DO, FOR, SWITCH, CASE */
	union lint_end_state ls_end;
};


/* allocation definitions of struct lint_stack_entry */
extern struct lint_stack_entry *h_lint_stack_entry;
#ifdef DEBUG
extern int cnt_lint_stack_entry;
extern char *std_alloc();
#define new_lint_stack_entry() ((struct lint_stack_entry *) std_alloc((char **)&h_lint_stack_entry, sizeof(struct lint_stack_entry), 10, &cnt_lint_stack_entry))
#else
#define	new_lint_stack_entry() ((struct lint_stack_entry *) st_alloc((char **)&h_lint_stack_entry, sizeof(struct lint_stack_entry), 10))
#endif
#define	free_lint_stack_entry(p) st_free(p, &h_lint_stack_entry, sizeof(struct lint_stack_entry))


/* macros to access the union */
#define LS_IF          ls_end.ule_if
#define LS_TEST        ls_end.ule_loop.lps_test
#define LS_BODY        ls_end.ule_loop.lps_body
#define LS_LOOP        ls_end.ule_loop.lps_loop
#define LS_CASE        ls_end.ule_switch.sws_case
#define LS_BREAK       ls_end.ule_switch.sws_break
#define LS_DEFAULT_MET ls_end.ule_switch.sws_default_met

/* describes a branch in the program, with its local idfs */
struct state
{
	struct state* next; /* only used by memory allocator */
	struct auto_def* st_auto_list;
	int st_notreached; /* set if not reached */
	int st_warned; /* set if warning issued */
};


/* allocation definitions of struct state */
extern struct state *h_state;
#ifdef DEBUG
extern int cnt_state;
extern char *std_alloc();
#define new_state() ((struct state *) std_alloc((char **)&h_state, sizeof(struct state), 15, &cnt_state))
#else
#define	new_state() ((struct state *) st_alloc((char **)&h_state, sizeof(struct state), 15))
#endif
#define	free_state(p) st_free(p, &h_state, sizeof(struct state))


/* describes the state of a local idf in a given branch of the program */
struct auto_def
{
	struct auto_def* next;
	struct idf* ad_idf;
	struct def* ad_def;
	int ad_used;
	int ad_set;
	int ad_maybe_set;
};


/* allocation definitions of struct auto_def */
extern struct auto_def *h_auto_def;
#ifdef DEBUG
extern int cnt_auto_def;
extern char *std_alloc();
#define new_auto_def() ((struct auto_def *) std_alloc((char **)&h_auto_def, sizeof(struct auto_def), 20, &cnt_auto_def))
#else
#define	new_auto_def() ((struct auto_def *) st_alloc((char **)&h_auto_def, sizeof(struct auto_def), 20))
#endif
#define	free_auto_def(p) st_free(p, &h_auto_def, sizeof(struct auto_def))


/* describes the state of an idf during expression evaluation */
struct expr_state
{ /*actually concerns idfs only */
	struct expr_state* next;
	struct idf* es_idf; /* the idf with its offset */
	arith es_offset;
	int es_used; /* value has been used */
	int es_referred; /* address has been taken */
	int es_set; /* has been assigned to */
};


/* allocation definitions of struct expr_state */
extern struct expr_state *h_expr_state;
#ifdef DEBUG
extern int cnt_expr_state;
extern char *std_alloc();
#define new_expr_state() ((struct expr_state *) std_alloc((char **)&h_expr_state, sizeof(struct expr_state), 20, &cnt_expr_state))
#else
#define	new_expr_state() ((struct expr_state *) st_alloc((char **)&h_expr_state, sizeof(struct expr_state), 20))
#endif
#define	free_expr_state(p) st_free(p, &h_expr_state, sizeof(struct expr_state))

