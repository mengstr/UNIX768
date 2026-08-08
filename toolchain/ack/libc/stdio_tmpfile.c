#include "stdio_private.h"

extern char *mktemp(char *);
extern i32 open(char *, i32);
extern i32 unlink(char *);
FILE *
tmpfile(void)
{
	char name[] = "/tmp/ctmpXXXXXX";
	i32 fd;
	FILE *fp;

	mktemp(name);
	fd = creat(name, 0600);
	if (fd < 0)
		return NULL;
	close(fd);
	fd = open(name, 2);
	if (fd < 0) {
		unlink(name);
		return NULL;
	}
	unlink(name);
	fp = fdopen(fd, "w+");
	if (fp == NULL)
		close(fd);
	return fp;
}
