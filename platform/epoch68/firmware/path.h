#ifndef EPOCH68_PATH_H
#define EPOCH68_PATH_H

#include <sys/inttypes.h>

i32 cwd_push(char *cwd, i32 cwdsz, const char *segment);
void cwd_pop(char *cwd);
i32 normalize_cwd(char *dst, i32 dstsz, const char *cwd, char *path);

#endif
