#include <fcntl.h>
#include <unistd.h>

char *mktemp(char *);

char *sys_maketempfile(const char *prefix, const char *suffix)
{
	static char name[] = "/tmp/aalXXXXX";
	int fd;

	(void)prefix;
	(void)suffix;
	mktemp(name);
	fd = creat(name, 0600);
	if (fd >= 0)
		close(fd);
	return name;
}
