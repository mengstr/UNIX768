#ifndef V7_SYS_INO_H
#define V7_SYS_INO_H

#include "inttypes.h"		/* for STATIC_ASSERT */

/*
 * Inode structure as it appears on
 * a disk block.
 */
struct dinode
{
	unsigned short	di_mode;     	/* mode and type of file */
	short	di_nlink;    	/* number of links to file */
	short	di_uid;      	/* owner's user id */
	short	di_gid;      	/* owner's group id */
	off_t	di_size;     	/* number of bytes in file */
	char  	di_addr[40];	/* disk block addresses */
	time_t	di_atime;   	/* time last accessed */
	time_t	di_mtime;   	/* time last modified */
	time_t	di_ctime;   	/* time created */
};
/* On-disk inode: exactly INOPB (8) per 512-byte block.  If a field width
 * drifts (di_size/di_*time are off_t/time_t), this stops compiling -- and it
 * is compiled by the kernel, userland, AND the monitor firmware, so all three
 * are held to the same disk layout. */
STATIC_ASSERT(assert_dinode_64, sizeof(struct dinode) == 64);
#define	INOPB	8	/* 8 inodes per block */
/*
 * the 40 address bytes:
 *	39 used; 13 addresses
 *	of 3 bytes each.
 */

#endif
