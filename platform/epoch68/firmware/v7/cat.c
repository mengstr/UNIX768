#include "pager.h"
#include "v7io.h"

/* Copy a file to the monitor console. */
i32
cat(char *path)
{
	char buf[512];
	i32 fd;
	i32 n;

	fd = open(path, 0);
	if (fd < 0)
		return(-1);

	pager_reset();
	for (;;) {
		n = read(fd, buf, sizeof(buf));
		if (n <= 0)
			break;
		if (pager_write(buf, n) < 0) {
			close(fd);
			return(0);
		}
	}

	close(fd);
	return(0);
}
