#ifndef V7_SYS_FILSYS_H
#define V7_SYS_FILSYS_H
/*
 * Structure of the super-block
 */

#include "inttypes.h"

struct	filsys {
	u16 	s_isize;	/* size in blocks of i-list */
	daddr_t	s_fsize;   	/* size in blocks of entire volume */
	i16  	s_nfree;   	/* number of addresses in s_free */
	daddr_t	s_free[NICFREE];/* free block list */
	i16  	s_ninode;  	/* number of i-nodes in s_inode */
	ino_t  	s_inode[NICINOD];/* free i-node list */
	char   	s_flock;   	/* lock during free list manipulation */
	char   	s_ilock;   	/* lock during i-list manipulation */
	char   	s_fmod;    	/* super block modified flag */
	char   	s_ronly;   	/* mounted read-only flag */
	time_t 	s_time;    	/* last super block update */
	/* remainder not maintained by this version of the system */
	daddr_t	s_tfree;   	/* total free blocks*/
	ino_t  	s_tinode;  	/* total free inodes */
	i16  	s_m;       	/* interleave factor */
	i16  	s_n;       	/* " " */
	char   	s_fname[6];	/* file system name */
	char   	s_fpack[6];	/* file system pack name */
	u8  	s_fill[72];	/* to make sizeof(struct filsys) 512 bytes */
};

/* The super-block is exactly one 512-byte disk block; if the field widths
 * (daddr_t/ino_t/time_t/...) ever drift, this fails to compile. */
STATIC_ASSERT(assert_filsys_512, sizeof(struct filsys) == 512);

#endif /* V7_SYS_FILSYS_H */
