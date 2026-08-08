/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef STACK_H_
#define STACK_H_

/* $Id$ */
/* IDENTIFIER STACK DEFINITIONS */

struct idf;

/*	The identifier stack is implemented as a stack of sets.
    The stack is implemented by a doubly linked list,
    the sets by singly linked lists.
*/

struct stack_level
{
	/* struct stack_level *next; */
	struct stack_level* sl_next; /* upward link		*/
	struct stack_level* sl_previous; /* downward link	*/
	struct stack_entry* sl_entry; /* sideward link	*/
	arith sl_local_offset; /* @ for first coming object	*/
	arith sl_max_block; /* maximum size of sub-block	*/
	int sl_level;
};


/* allocation definitions of struct stack_level */
extern struct stack_level *h_stack_level;
#ifdef DEBUG
extern int cnt_stack_level;
extern char *std_alloc();
#define new_stack_level() ((struct stack_level *) std_alloc((char **)&h_stack_level, sizeof(struct stack_level), 5, &cnt_stack_level))
#else
#define	new_stack_level() ((struct stack_level *) st_alloc((char **)&h_stack_level, sizeof(struct stack_level), 5))
#endif
#define	free_stack_level(p) st_free(p, &h_stack_level, sizeof(struct stack_level))


struct stack_entry
{
	struct stack_entry* next;
	struct idf* se_idf;
};


/* allocation definitions of struct stack_entry */
extern struct stack_entry *h_stack_entry;
#ifdef DEBUG
extern int cnt_stack_entry;
extern char *std_alloc();
#define new_stack_entry() ((struct stack_entry *) std_alloc((char **)&h_stack_entry, sizeof(struct stack_entry), 50, &cnt_stack_entry))
#else
#define	new_stack_entry() ((struct stack_entry *) st_alloc((char **)&h_stack_entry, sizeof(struct stack_entry), 50))
#endif
#define	free_stack_entry(p) st_free(p, &h_stack_entry, sizeof(struct stack_entry))


extern struct stack_level* local_level;
extern int level;

/*	A new level is added on top of the identifier stack. */
void stack_level(void);
/*	The identifier idf is inserted in the stack on level stl,
    but only if it is not already present at this level.
*/
void stack_idf(struct idf* idf, register struct stack_level* stl);
/*The stack_level corresponding to level lvl is returned.
  The stack should probably be an array, to be extended with
 realloc where needed.
*/
struct stack_level* stack_level_of(int lvl);
/* The top level of the identifier stack is removed. */
void unstack_level(void);
void unstack_world(void);

#ifdef GEN_NM_LIST
void open_name_list(void);
void namelist(char* nm);
#endif /* GEN_NM_LIST */

#endif
