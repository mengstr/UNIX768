#include "v7fs.h"
#include "v7io.h"
#include "../string.h"

/* Copy one regular file, replacing an existing non-directory destination. */
i32
cp(char *src, char *dst)
{
	char buf[512];
	i32 sfd;
	i32 dfd;
	i32 n;

	n = v7fs_isdir(src);
	if (n != 0)
		return(-1);

	if (exists(dst) > 0) {
		if (strcmp(src, dst) == 0)
			return(-1);
		if (unlink(dst) < 0)
			return(-1);
	}

	sfd = open(src, 0);
	if (sfd < 0)
		return(-1);
	dfd = open(dst, 1);
	if (dfd < 0) {
		close(sfd);
		return(-1);
	}

	for (;;) {
		n = read(sfd, buf, sizeof(buf));
		if (n <= 0)
			break;
		if (write(dfd, buf, n) != n) {
			close(sfd);
			close(dfd);
			unlink(dst);
			return(-1);
		}
	}

	close(sfd);
	close(dfd);
	return(0);
}
