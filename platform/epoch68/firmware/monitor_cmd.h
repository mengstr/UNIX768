#ifndef EPOCH68_MONITOR_CMD_H
#define EPOCH68_MONITOR_CMD_H

#include <sys/inttypes.h>

i32 monitor_run_command(char *cmdbuf, char *attached, i32 attachedsz,
	char *cwd, i32 cwdsz);
i32 monitor_run_script(char *script, char *attached, i32 attachedsz,
	char *cwd, i32 cwdsz);

#endif
