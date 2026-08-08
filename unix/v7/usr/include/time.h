#include "sys/inttypes.h"
#include "sys/types.h"

#ifndef _TIME_H_
#define _TIME_H_

struct tm { /* see ctime(3) */
	i32	tm_sec;
	i32	tm_min;
	i32	tm_hour;
	i32	tm_mday;
	i32	tm_mon;
	i32	tm_year;
	i32	tm_wday;
	i32	tm_yday;
	i32	tm_isdst;
};

char	*asctime(struct tm *);
char	*ctime(time_t *);
struct	tm *gmtime(time_t *);
struct	tm *localtime(time_t *);
i32	time(time_t *);

#endif
