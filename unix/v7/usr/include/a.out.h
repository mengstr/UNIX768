#include "sys/inttypes.h"

struct	exec {	/* a.out header */
	u32     	a_magic;	/* magic number */
	u32	a_text; 	/* size of text segment */
	u32	a_data; 	/* size of initialized data */
	u32	a_bss;  	/* size of unitialized data */
	u32	a_syms; 	/* size of symbol table */
	u32	a_entry;	/* entry point */
	u32	a_unused;	/* text relocation size (compatibility name) */
	u32	a_flag;		/* data relocation size (compatibility name) */
};

#define	A_MAGIC1	0407       	/* normal */
#define	A_MAGIC2	0410       	/* read-only text */
#define	A_MAGIC3	0411       	/* separated I&D */
#define	A_MAGIC4	0405       	/* overlay */
#define	A_MAGIC5	0413       	/* Sun demand-paged executable */

#define	A_MID_SUN010	1
#define	A_GETMAGIC(x)	((x).a_magic & 0xffffUL)
#define	A_GETMID(x)	(((x).a_magic >> 16) & 0x3ffUL)

struct	nlist {	/* symbol table entry */
	u32	n_strx;		/* index into string table */
	u8	n_type;		/* type flag */
	u8	n_other;	/* unused */
	i16	n_desc;		/* debugger description */
	u32	n_value;	/* value */
};

		/* values for type flag */
#define	N_UNDF	0x00	/* undefined */
#define	N_ABS	0x02	/* absolute */
#define	N_TEXT	0x04	/* text symbol */
#define	N_DATA	0x06	/* data symbol */
#define	N_BSS	0x08	/* bss symbol */
#define	N_TYPE	0x1e
#define	N_FN	0x1f	/* file name symbol */
#define	N_EXT	0x01	/* external bit, or'ed in */
#define	N_STAB	0xe0	/* debugger symbol bits */
#define	FORMAT	"%08lx"	/* to print a value */
