#ifndef V7_SYS_INODE_H
#define V7_SYS_INODE_H

/*
 * The I node is the focus of all
 * file activity in unix. There is a unique
 * inode allocated for each active file,
 * each current directory, each mounted-on
 * file, text file, and the root. An inode is 'named'
 * by its dev/inumber pair. (iget/iget.c)
 * Data, from mode on, is read in
 * from permanent inode on volume.
 */

#include "inttypes.h"
#include "inode_str.h"	/* struct inode/group + codes -- shared with userland <sys/inode.h> */

#endif
