#include "v7fs.h"
#include "../string.h"
#include "v7io.h"
#include <sys/param.h>
#include <sys/inode.h>

static i32 has_devspec(char *path);
static i32 path_append(char *dst, i32 dstsz, char *src);
static i32 path_is_rootish(char *path);
static i32 path_join(char *dst, i32 dstsz, char *src);

/* Resolve a monitor path to the standalone V7 path syntax. */
i32
v7fs_resolve_path(char *dst, i32 dstsz, char *attached, char *cwd, char *path)
{
	char *tail;

	if (dst == 0 || dstsz <= 0)
		return(-1);

	if (path == 0 || *path == '\0' || path_is_rootish(path))
		tail = 0;
	else
		tail = path;

	if (tail != 0 && has_devspec(tail)) {
		char *slash;

		if ((i32)strlen(tail) + 1 > dstsz)
			return(-1);
		strcpy(dst, tail);
		if (strchr(tail, '/') == 0 && tail[strlen(tail) - 1] == ')')
			return(path_append(dst, dstsz, "."));
		slash = (char *)strchr(tail, '/');
		if (slash == tail + strlen(tail) - 1 &&
		    tail[strlen(tail) - 2] == ')') {
			dst[strlen(dst) - 1] = '.';
			dst[strlen(dst)] = '\0';
		}
		return(0);
	}

	if (attached == 0 || *attached == '\0')
		return(-1);
	if ((i32)strlen(attached) + 1 > dstsz)
		return(-1);
	strcpy(dst, attached);

	if (tail == 0) {
		if (cwd == 0 || *cwd == '\0' || path_is_rootish(cwd))
			return(path_append(dst, dstsz, "."));
		return(path_append(dst, dstsz, cwd));
	}

	if (*tail == '/') {
		tail++;
		if (*tail == '\0')
			return(path_append(dst, dstsz, "."));
		return(path_append(dst, dstsz, tail));
	}

	if (cwd != 0 && *cwd != '\0' && !path_is_rootish(cwd)) {
		if (path_append(dst, dstsz, cwd) < 0)
			return(-1);
		return(path_join(dst, dstsz, tail));
	}

	return(path_append(dst, dstsz, tail));
}

/* Return 1 if path names a directory, 0 if not, -1 on open failure. */
i32
v7fs_isdir(char *path)
{
	i32 fd;
	struct iob *io;

	fd = open(path, 0);
	if (fd < 0)
		return(-1);
	io = &iob[fd - 3];
	if ((io->i_ino.i_mode & IFMT) == IFDIR) {
		close(fd);
		return(1);
	}
	close(fd);
	return(0);
}

/* Append a suffix string to an existing path buffer. */
static i32
path_append(char *dst, i32 dstsz, char *src)
{
	if (strlen(dst) + strlen(src) + 1 > dstsz)
		return(-1);
	strcat(dst, src);
	return(0);
}

/* Append one path component with a separating slash when needed. */
static i32
path_join(char *dst, i32 dstsz, char *src)
{
	if (dst[0] == '\0' || dst[strlen(dst) - 1] == '/')
		return(path_append(dst, dstsz, src));
	if (path_append(dst, dstsz, "/") < 0)
		return(-1);
	return(path_append(dst, dstsz, src));
}

/* Treat empty, ".", and "/" as logical root paths. */
static i32
path_is_rootish(char *path)
{
	return(path == 0 || strcmp(path, ".") == 0 || strcmp(path, "/") == 0);
}

/* Detect an explicit standalone device specification prefix. */
static i32
has_devspec(char *path)
{
	register char *cp;

	for (cp = path; *cp != '\0'; cp++) {
		if (*cp == '(')
			return(1);
		if (*cp == '/')
			break;
	}
	return(0);
}
