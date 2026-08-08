#include "sys/inttypes.h"

#define	ARMAG	0177545
#define AALMAG 0177454
struct	ar_hdr {
	char	ar_name[14];
	i32	ar_date;
	u8	ar_uid;
	u8	ar_gid;
	u16	ar_mode;
	i32	ar_size;
};
