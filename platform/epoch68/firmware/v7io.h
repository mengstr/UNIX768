#ifndef EPOCH68_V7IO_H
#define EPOCH68_V7IO_H

#include <sys/inttypes.h>
#include <sys/param.h>
#include <sys/inode.h>
#include <saio_str.h>

/*
 * header file for the V7 monitor I/O layer
 *
 * Layout is kept aligned with unix/v7/usr/include/saio.h, but this header
 * uses explicit declarations instead of tentative definitions so it remains
 * safe to include across the monitor sources.
 */

void printf(char *fmt, ...);
char *sprintf(char *dst, char *fmt, ...);

extern struct devsw devsw[];

i32 open(char *path, i32 how);
i32 read(i32 fdesc, char *buf, i32 count);
i32 write(i32 fdesc, char *buf, i32 count);
i32 close(i32 fdesc);
i32 lseek(i32 fdesc, off_t addr, i32 ptr);
void v7io_close_all(void);
i32 atol(char *s);
void _stop(char *s);
void l3tol(i32 *lp, char *cp, i32 n);
void ltol3(char *cp, i32 *lp, i32 n);
i32 chgrp(char *path, i32 gid);
i32 chown(char *path, i32 uid);
i32 chmod(char *path, i32 mode);
i32 devread(struct iob *io);
i32 devwrite(struct iob *io);
void devopen(struct iob *io);
void devclose(struct iob *io);
i32 exists(char *path);
i32 mkdir(char *path);
i32 mknod(char *path, i32 mode, dev_t dev);
i32 link(char *src, char *dst);
i32 rmdir(char *path);
i32 unlink(char *path);

extern char	b[NBUFS][512];
extern daddr_t	blknos[NBUFS];
extern struct	iob iob[NFILES];

/*
 * Set to which 32Kw segment the code is physically running in.
 * Must be set by the users main (or there abouts).
 */
extern i32	segflag;

#endif
