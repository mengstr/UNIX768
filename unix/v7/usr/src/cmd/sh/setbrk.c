#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"

BYTPTR		sbrk(i32 incr);

BYTPTR
setbrk(incr)
	i32 incr;
{
	REG BYTPTR	a=sbrk(incr);
	brkend=a+incr;
	return(a);
}
