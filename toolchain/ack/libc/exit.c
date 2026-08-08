#include "types.h"

extern void _cleanup(void);
extern void _exit_raw(i32 status);

void
exit(int status)
{
	_cleanup();
	_exit_raw((i32)status);
}
