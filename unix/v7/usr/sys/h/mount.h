#ifndef V7_SYS_MOUNT_H
#define V7_SYS_MOUNT_H

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
#include "param.h"

extern struct	mount
{
	dev_t	m_dev;		/* device mounted */
	struct buf *m_bufp;	/* pointer to superblock */
	struct inode *m_inodp;	/* pointer to mounted on inode */
} mount[NMOUNT];

#endif /* V7_SYS_MOUNT_H */
