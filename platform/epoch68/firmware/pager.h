#ifndef EPOCH68_PAGER_H
#define EPOCH68_PAGER_H

#include <sys/inttypes.h>

void pager_reset(void);
i32 pager_putc(i32 c);
i32 pager_write(char *buf, i32 count);

#endif
