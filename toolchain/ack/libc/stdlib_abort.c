#include "types.h"

extern void exit(int status);

void
abort(void)
{
	exit(1);
	for (;;)
		;
}
