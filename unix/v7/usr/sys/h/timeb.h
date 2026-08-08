#ifndef V7_SYS_TIMEB_H
#define V7_SYS_TIMEB_H

/*
 * Structure returned by ftime system call
 */
struct timeb {
	time_t	time;
	u16	millitm;
	i16	timezone;
	i16	dstflag;
};

#endif /* V7_SYS_TIMEB_H */
