#include "saio_str.h"

/*
 * header file for standalone package
 */

struct devsw devsw[];

char	b[NBUFS][512];
daddr_t	blknos[NBUFS];
struct	iob iob[NFILES];

/*
 * Set to which 32Kw segment the code is physically running in.
 * Must be set by the users main (or there abouts).
 */
i32	segflag;
