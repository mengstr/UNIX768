#ifndef V7_SYS_DIR_H
#define V7_SYS_DIR_H

#include "inttypes.h"		/* for STATIC_ASSERT */

#define	V7_DIRSIZ	14
#ifndef	DIRSIZ
#define	DIRSIZ	V7_DIRSIZ
#endif
struct	direct
{
	ino_t	d_ino;
	char	d_name[V7_DIRSIZ];
};
/* On-disk directory entry: ino_t (2) + DIRSIZ (14) = 16 bytes.  Compiled by
 * the kernel, userland, and the monitor firmware, so all three agree. */
STATIC_ASSERT(assert_direct_16, sizeof(struct direct) == 16);

#endif /* V7_SYS_DIR_H */
