#include "stdio_private.h"

extern i32 _popen_pid[];

i32
pclose(fp)
FILE *fp;
{
	i32 idx;
	i32 pid;
	i16 status;
	i32 r;
	i32 w;

	if (fp == NULL)
		return -1;
	idx = fp - _iob;
	if (idx < 0 || idx >= _NFILE)
		return -1;
	pid = _popen_pid[idx];
	if (pid <= 0)
		return -1;
	_popen_pid[idx] = 0;

	r = fclose(fp);
	do {
		w = wait(&status);
	} while (w >= 0 && w != pid);
	if (w < 0)
		return -1;
	if (r == EOF)
		return -1;
	return status;
}
