#ifndef YMODEM_H
#define YMODEM_H

#include <sys/inttypes.h>

i32 ymodem_receive_monitor(i32 channel, char *attached, char *cwd,
	char *request, char *path);
i32 ymodem_receive_monitor_quiet(i32 channel, char *attached, char *cwd,
	char *request, char *path);
i32 ymodem_receive_buffer(i32 channel, char *request, char *buffer, i32 bufsz);

#endif
