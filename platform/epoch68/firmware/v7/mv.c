#include "v7io.h"

i32 cp(char *src, char *dst);

/* Move one regular file by copying it and unlinking the source. */
i32
mv(char *src, char *dst)
{
	if (cp(src, dst) < 0)
		return(-1);
	if (unlink(src) < 0)
		return(-1);
	return(0);
}
