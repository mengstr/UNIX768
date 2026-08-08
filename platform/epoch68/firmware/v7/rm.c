#include "v7io.h"

/* Remove one non-directory path. */
i32
rm(char *path)
{
	return(unlink(path));
}
