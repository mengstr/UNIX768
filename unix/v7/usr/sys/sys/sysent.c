#include "../h/param.h"
#include "../h/systm.h"

/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */
i32	alarm(void);
i32	mpxchan(void);
i32	chdir(void);
i32	chmod(void);
i32	chown(void);
i32	chroot(void);
i32	close(void);
i32	creat(void);
i32	dup(void);
i32	exec(void);
i32	exece(void);
i32	fork(void);
i32	fstat(void);
i32	getgid(void);
i32	getpid(void);
i32	getuid(void);
i32	gtime(void);
i32	gtty(void);
i32	ioctl(void);
i32	kill(void);
i32	link(void);
i32	mknod(void);
i32	nice(void);
i32	nosys(void);
i32	nullsys(void);
i32	open(void);
i32	pause(void);
i32	pipe(void);
i32	profil(void);
i32	ptrace(void);
i32	read(void);
i32	rexit(void);
i32	saccess(void);
i32	sbreak(void);
i32	seek(void);
i32	setgid(void);
i32	setuid(void);
i32	smount(void);
i32	ssig(void);
i32	stat(void);
i32	stime(void);
i32	stty(void);
i32	sumount(void);
i32	ftime(void);
i32	sync(void);
i32	sysacct(void);
i32	syslock(void);
i32	times(void);
i32	umask(void);
i32	unlink(void);
i32	utime(void);
i32	wait(void);
i32	write(void);

struct sysent sysent[64] =
{
	0, 0, nullsys,			/*  0 = indir */
	1, 1, rexit,			/*  1 = exit */
	0, 0, fork,			/*  2 = fork */
	3, 1, read,			/*  3 = read */
	3, 1, write,			/*  4 = write */
	2, 0, open,			/*  5 = open */
	1, 1, close,			/*  6 = close */
	0, 0, wait,			/*  7 = wait */
	2, 0, creat,			/*  8 = creat */
	2, 0, link,			/*  9 = link */
	1, 0, unlink,			/* 10 = unlink */
	2, 0, exec,			/* 11 = exec */
	1, 0, chdir,			/* 12 = chdir */
	0, 0, gtime,			/* 13 = time */
	3, 0, mknod,			/* 14 = mknod */
	2, 0, chmod,			/* 15 = chmod */
	3, 0, chown,			/* 16 = chown; now 3 args */
	1, 0, sbreak,			/* 17 = break */
	2, 0, stat,			/* 18 = stat */
	4, 1, seek,			/* 19 = seek; now 3 args */
	0, 0, getpid,			/* 20 = getpid */
	3, 0, smount,			/* 21 = mount */
	1, 0, sumount,			/* 22 = umount */
	1, 1, setuid,			/* 23 = setuid */
	0, 0, getuid,			/* 24 = getuid */
	2, 2, stime,			/* 25 = stime */
	4, 1, ptrace,			/* 26 = ptrace */
	1, 1, alarm,			/* 27 = alarm */
	2, 1, fstat,			/* 28 = fstat */
	0, 0, pause,			/* 29 = pause */
	2, 0, utime,			/* 30 = utime */
	2, 1, stty,			/* 31 = stty */
	2, 1, gtty,			/* 32 = gtty */
	2, 0, saccess,			/* 33 = access */
	1, 1, nice,			/* 34 = nice */
	1, 0, ftime,			/* 35 = ftime; formerly sleep */
	0, 0, sync,			/* 36 = sync */
	2, 1, kill,			/* 37 = kill */
	0, 0, nullsys,			/* 38 = switch; inoperative */
	0, 0, nullsys,			/* 39 = setpgrp (not in yet) */
	1, 1, nosys,			/* 40 = tell (obsolete) */
	2, 2, dup,			/* 41 = dup */
	0, 0, pipe,			/* 42 = pipe */
	1, 0, times,			/* 43 = times */
	4, 0, profil,			/* 44 = prof */
	0, 0, nosys,			/* 45 = unused */
	1, 1, setgid,			/* 46 = setgid */
	0, 0, getgid,			/* 47 = getgid */
	2, 0, ssig,			/* 48 = sig */
	0, 0, nosys,			/* 49 = reserved for USG */
	0, 0, nosys,			/* 50 = reserved for USG */
	1, 0, sysacct,			/* 51 = turn acct off/on */
	0, 0, nosys,			/* 52 = set user physical addresses */
	1, 0, syslock,			/* 53 = lock user in core */
	3, 0, ioctl,			/* 54 = ioctl */
	0, 0, nosys,			/* 55 = readwrite (in abeyance) */
	4, 0, mpxchan,			/* 56 = creat mpx comm channel */
	0, 0, nosys,			/* 57 = reserved for USG */
	0, 0, nosys,			/* 58 = reserved for USG */
	3, 0, exece,			/* 59 = exece */
	1, 0, umask,			/* 60 = umask */
	1, 0, chroot,			/* 61 = chroot */
	0, 0, nosys,			/* 62 = x */
	0, 0, nosys			/* 63 = used internally */
};
