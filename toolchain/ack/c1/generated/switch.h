/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef SWITCH_H_
#define SWITCH_H_

/* $Id$ */

#include "arith.h"

struct type;
struct expr;

/*		S W I T C H - T A B L E - S T R U C T U R E		*/

struct switch_hdr
{
	struct switch_hdr* next;
	label sh_break;
	label sh_default;
	label sh_table;
	int sh_nrofentries;
	struct type* sh_type;
	struct expr* sh_expr;
	writh sh_lowerbd;
	writh sh_upperbd;
	struct case_entry* sh_entries;
};


/* allocation definitions of struct switch_hdr */
extern struct switch_hdr *h_switch_hdr;
#ifdef DEBUG
extern int cnt_switch_hdr;
extern char *std_alloc();
#define new_switch_hdr() ((struct switch_hdr *) std_alloc((char **)&h_switch_hdr, sizeof(struct switch_hdr), 2, &cnt_switch_hdr))
#else
#define	new_switch_hdr() ((struct switch_hdr *) st_alloc((char **)&h_switch_hdr, sizeof(struct switch_hdr), 2))
#endif
#define	free_switch_hdr(p) st_free(p, &h_switch_hdr, sizeof(struct switch_hdr))


struct case_entry
{
	struct case_entry* next;
	label ce_label;
	writh ce_value;
};


/* allocation definitions of struct case_entry */
extern struct case_entry *h_case_entry;
#ifdef DEBUG
extern int cnt_case_entry;
extern char *std_alloc();
#define new_case_entry() ((struct case_entry *) std_alloc((char **)&h_case_entry, sizeof(struct case_entry), 20, &cnt_case_entry))
#else
#define	new_case_entry() ((struct case_entry *) st_alloc((char **)&h_case_entry, sizeof(struct case_entry), 20))
#endif
#define	free_case_entry(p) st_free(p, &h_case_entry, sizeof(struct case_entry))


void code_startswitch(struct expr** expp);
void code_endswitch(void);
void code_case(struct expr* expr);
void code_default(void);

#endif