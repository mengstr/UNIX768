#ifndef V7_SYS_PRIM_H
#define V7_SYS_PRIM_H

#define	NOSLEEP	0400
#define	FORCE	01000
#define	NORM	02000
#define	KEEP	04000
#define	CLR	010000

i32	bwaiting,wcount;

char *getepack();

#endif /* V7_SYS_PRIM_H */
