#ifndef V7_SYS_CALLO_H
#define V7_SYS_CALLO_H

/*
 * The callout structure is for
 * a routine arranging
 * to be called by the clock interrupt
 * (clock.c) with a specified argument,
 * in a specified amount of time.
 * Used, for example, to time tab
 * delays on typewriters.
 */

struct	callo
{
	i32	c_time;		/* incremental time */
	caddr_t	c_arg;		/* argument to routine */
	i32	(*c_func)(caddr_t);	/* routine */
};
struct	callo	callout[NCALL];

#endif /* V7_SYS_CALLO_H */
