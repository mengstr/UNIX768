#include "sys/types.h"

struct utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[8];		/* user id */
	time_t	ut_time;		/* time on */
};
