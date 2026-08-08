#include "sys/inttypes.h"

#ifndef _UNISTD_H_
#define _UNISTD_H_

struct stat;

i32	close(i32 fd);
i32	chdir(char *path);
i32	chmod(char *path, i32 mode);
i32	chown(char *path, i32 uid, i32 gid);
i32	access(char *path, i32 mode);
i32	acct(char *path);
i32	creat(char *path, i32 mode);
i32	dup(i32 fd);
i32	execl(char *path, char *arg0, ...);
i32	execv(char *path, char **argv);
i32	execve(char *path, char **argv, char **envp);
i32	execvp(char *path, char **argv);
void	exit(int status);
void	_exit(int status);
i32	fstat(i32 fd, struct stat *sb);
i32	fork(void);
i32	alarm(i32 seconds);
i32	getpid(void);
i32	getuid(void);
i32	getgid(void);
i32	geteuid(void);
i32	getegid(void);
i32	ioctl(i32 fd, i32 cmd, void *arg);
i32	kill(i32 pid, i32 signo);
i32	link(char *target, char *linkname);
i32	lseek(i32 fd, i32 off, i32 sbase);
i32	mount(char *spec, char *dir, i32 ro);
i32	mknod(char *path, i32 mode, i32 dev);
i32	nice(i32 niceness);
i32	open(char *path, i32 mode);
i32	pause(void);
i32	pipe(i16 *fds);
i32	read(i32 fd, void *buf, i32 count);
i32	setgid(i32 gid);
i32	setuid(i32 uid);
i32	stat(char *path, struct stat *sb);
char	*sbrk(i32 incr);
i32	brk(char *addr);
u32	sleep(u32 seconds);
i32	stime(i32 *tp);
i32	sync(void);
i32	time(i32 *tp);
i32	umask(i32 mask);
i32	umount(char *spec);
i32	unlink(char *path);
i32	utime(char *path, void *times);
i32	wait(i16 *status);
i32	write(i32 fd, void *buf, i32 count);
i32	isatty(i32 fd);
char	*ttyname(i32 fd);
char	*getlogin(void);
char	*mktemp(char *template);

extern char *optarg;
extern int opterr;
extern int optind;
extern int optopt;
int	getopt(int argc, char **argv, char *options);

#endif
