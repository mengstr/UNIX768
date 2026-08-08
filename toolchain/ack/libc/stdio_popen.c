#include "stdio_private.h"

i32 _popen_pid[_NFILE];

FILE *
popen(cmd, mode)
char *cmd;
char *mode;
{
	i16 p[2];
	i32 pid;
	i32 parent_fd;
	i32 child_fd;
	FILE *fp;

	if (mode == NULL || mode[1] != '\0')
		return NULL;
	if (*mode != 'r' && *mode != 'w')
		return NULL;
	if (pipe(p) < 0)
		return NULL;

	if (*mode == 'r') {
		parent_fd = p[0];
		child_fd = p[1];
	} else {
		parent_fd = p[1];
		child_fd = p[0];
	}

	pid = fork();
	if (pid < 0) {
		close(p[0]);
		close(p[1]);
		return NULL;
	}
	if (pid == 0) {
		if (*mode == 'r') {
			close(1);
			dup(child_fd);
		} else {
			close(0);
			dup(child_fd);
		}
		close(p[0]);
		close(p[1]);
		execl("/bin/sh", "sh", "-c", cmd, (char *)0);
		execl("/usr/bin/sh", "sh", "-c", cmd, (char *)0);
		exit(127);
	}

	close(child_fd);
	fp = fdopen(parent_fd, mode);
	if (fp == NULL) {
		close(parent_fd);
		return NULL;
	}
	_popen_pid[fp - _iob] = pid;
	return fp;
}
