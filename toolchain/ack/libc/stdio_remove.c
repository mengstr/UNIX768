#include "types.h"

extern i32 unlink(char *);

i32
remove(char *path)
{
	return unlink(path);
}
