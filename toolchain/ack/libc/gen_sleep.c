#include <sys/types.h>
#include "types.h"
#include <sys/dir.h>
#include <sys/stat.h>

typedef void (*sighandler_t)(i16);

extern i32 alarm(u32 n);
extern i32 close(i32 fd);
extern i32 fstat(i32 fd, struct stat *sb);
extern i32 isatty(i32 fd);
extern i32 open(char *path, i32 mode);
extern i32 pause(void);
extern i32 read(i32 fd, void *buf, i32 count);
extern sighandler_t signal(i16 sig, sighandler_t func);
extern i32 stat(char *path, struct stat *sb);
extern i32 time(i32 *tp);

static i32 sleep_done;
static void sleepx(i16 sig);

char *
ttyname(i32 fd)
{
	struct stat fsb;
	struct stat dsb;
	struct direct db;
	static char path[sizeof("/dev/") + DIRSIZ] = "/dev/";
	register i32 df;
	register i32 i;

	if (fd < 0 || !isatty(fd) || fstat(fd, &fsb) < 0)
		return (char *)0;
	if ((fsb.st_mode & S_IFMT) != S_IFCHR)
		return (char *)0;
	if (stat("/dev", &dsb) < 0 || fsb.st_dev != dsb.st_dev)
		return (char *)0;
	df = open("/dev", 0);
	if (df < 0)
		return (char *)0;
	while (read(df, &db, sizeof(db)) == sizeof(db)) {
		if (db.d_ino == 0 || db.d_ino != fsb.st_ino)
			continue;
		for (i = 0; i < DIRSIZ; i++)
			path[5 + i] = db.d_name[i];
		path[5 + DIRSIZ] = '\0';
		close(df);
		return path;
	}
	close(df);
	return (char *)0;
}

static void
sleepx(i16 sig)
{
	(void)sig;
	sleep_done = 1;
}

u32
sleep(u32 n)
{
	sighandler_t old;
	i32 now;
	i32 target;
	u32 altime;
	u32 delay;

	if (n == 0)
		return 0;
	altime = alarm(1000);
	if (altime) {
		if (altime > n)
			altime -= n;
		else {
			n = altime;
			altime = 1;
		}
	}
	target = time((i32 *)0) + (i32)n;
	old = signal(14, sleepx);
	while ((now = time((i32 *)0)) < target) {
		delay = (u32)(target - now);
		sleep_done = 0;
		alarm(delay);
		while (!sleep_done)
			pause();
	}
	signal(14, old);
	alarm(altime);
	return 0;
}
