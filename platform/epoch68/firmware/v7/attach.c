#include "v7fs.h"
#include <sys/param.h>
#include <sys/filsys.h>
#include <sys/inode.h>
#include "../string.h"
#include "conf.h"
#include "v7io.h"

/* Attach to a filesystem by probing its root directory. */
i32 attach(char *devspec)
{
	i32 fd;
	char path[100];

	if (devspec == 0 || *devspec == '\0') {
		printf("attach: missing devspec\n");
		return(-1);
	}
	strcpy(path, devspec);
	if (strlen(path) + 2 > sizeof(path)) {
		printf("attach: devspec too long\n");
		return(-1);
	}
	strcat(path, ".");
	fd = open(path, 0);
	if (fd < 0) {
		printf("warning: %s is not a readable V7 filesystem\n", devspec);
		return(-1);
	}
	close(fd);
	return(0);
}
