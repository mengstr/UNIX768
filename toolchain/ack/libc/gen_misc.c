#include "types.h"

extern i32 errno;
extern i32 write(i32 fd, char *buf, i32 n);
extern usize strlen(char *s);
extern i32 access(char *path, i32 mode);
extern i32 getpid(void);
extern i32 execv(char *name, char **argv);

char *sys_errlist[] = {
	"Error 0",
	"Not owner",
	"No such file or directory",
	"No such process",
	"Interrupted system call",
	"I/O error",
	"No such device or address",
	"Arg list too long",
	"Exec format error",
	"Bad file number",
	"No children",
	"No more processes",
	"Not enough core",
	"Permission denied",
	"Bad address",
	"Block device required",
	"Mount device busy",
	"File exists",
	"Cross-device link",
	"No such device",
	"Not a directory",
	"Is a directory",
	"Invalid argument",
	"File table overflow",
	"Too many open files",
	"Not a typewriter",
	"Text file busy",
	"File too large",
	"No space left on device",
	"Illegal seek",
	"Read-only file system",
	"Too many links",
	"Broken pipe",
	"Argument too large",
	"Result too large",
};

i32 sys_nerr = sizeof sys_errlist / sizeof sys_errlist[0];

char *
mktemp(char *as)
{
	register char *s;
	register u32 pid;
	register i32 i;

	pid = getpid();
	s = as;
	while (*s++ != '\0')
		;
	s--;
	while (*--s == 'X') {
		*s = (pid % 10) + '0';
		pid /= 10;
	}
	s++;
	i = 'a';
	while (access(as, 0) != -1) {
		if (i == 'z')
			return "/";
		*s = i++;
	}
	return as;
}

void
perror(char *s)
{
	char *c;
	usize n;

	c = "Unknown error";
	if (errno >= 0 && errno < sys_nerr)
		c = sys_errlist[errno];
	n = strlen(s);
	if (n != 0) {
		write(2, s, n);
		write(2, ": ", 2);
	}
	write(2, c, strlen(c));
	write(2, "\n", 1);
}

char *
strerror(i32 err)
{
	if (err >= 0 && err < sys_nerr)
		return sys_errlist[err];
	return "Unknown error";
}
