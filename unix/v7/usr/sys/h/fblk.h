#ifndef V7_SYS_FBLK_H
#define V7_SYS_FBLK_H

#include "inttypes.h"

struct fblk
{
	i16    	df_nfree;
	daddr_t	df_free[NICFREE];
	u8 		fill[310];
};

STATIC_ASSERT(assert_fblk_512, sizeof(struct fblk) == 512);

#endif /* V7_SYS_FBLK_H */
