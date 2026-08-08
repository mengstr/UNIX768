/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef STMT_H_
#define STMT_H_

/* $Id$ */
/*	S T A T E M E N T - B L O C K   D E F I N I T I O N S	*/

struct stmt_block
{
	struct stmt_block* next;
	label st_break;
	label st_continue;
};


/* allocation definitions of struct stmt_block */
extern struct stmt_block *h_stmt_block;
#ifdef DEBUG
extern int cnt_stmt_block;
extern char *std_alloc();
#define new_stmt_block() ((struct stmt_block *) std_alloc((char **)&h_stmt_block, sizeof(struct stmt_block), 5, &cnt_stmt_block))
#else
#define	new_stmt_block() ((struct stmt_block *) st_alloc((char **)&h_stmt_block, sizeof(struct stmt_block), 5))
#endif
#define	free_stmt_block(p) st_free(p, &h_stmt_block, sizeof(struct stmt_block))


#endif