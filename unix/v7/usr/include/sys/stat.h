/*
 * stat()/inode status structure -- shared with kernel <stat.h>.
 */
#include "types.h"
#include "../../sys/h/stat.h"

i32	stat(char *, struct stat *);
i32	fstat(i32, struct stat *);
