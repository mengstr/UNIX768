/* v7fs.h - V7 filesystem helpers for monitor path resolution */

#include <sys/inttypes.h>

i32 attach(char *devspec);
i32 v7fs_isdir(char *path);
i32 v7fs_resolve_path(char *dst, i32 dstsz, char *attached, char *cwd,
	char *path);
