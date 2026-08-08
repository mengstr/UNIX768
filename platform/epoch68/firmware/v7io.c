#include <sys/param.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <sys/dir.h>
#include "prf.h" /* IWYU pragma: keep */
#include "string.h"
#include "v7time.h"
#include "v7io.h"


static void openi(ino_t n, struct iob *io);
static i32 find(char *path, struct iob *file);
static ino_t create(char *path, struct iob *file);
static ino_t ialloc(struct iob *io, i32 mode, dev_t dev);
static ino_t maknode(char *path, struct iob *file, i32 mode, dev_t dev);
static daddr_t alloc(struct iob *io);
static i32 adddir(struct iob *io, char *name, ino_t ino);
static daddr_t sbmap(struct iob *io, daddr_t bn);
static daddr_t wbmap(struct iob *io, daddr_t bn);
static i32 writei(struct iob *file, char *buf, i32 count);
static void blkclr(struct iob *io, daddr_t bn);
static void bfree(struct iob *io, daddr_t bno);
static void itrunc(struct iob *io);
static void readsb(struct iob *io);
static void tloop(struct iob *io, daddr_t bn, i32 f1, i32 f2);
static void writeino(struct iob *io);
static void writesb(struct iob *io);
static i32 allocslot(void);
i32 atol(char *s);
void _stop(char *s);

i32	segflag = 0;
char	b[NBUFS][512];
daddr_t	blknos[NBUFS];
struct	iob iob[NFILES];

static ino_t dlook(char *s, struct iob *io);
static i32 path_parent(char *path, struct iob *file, char **name);
static i32 setupiob(char *str, struct iob *file, char **path);
void l3tol(i32 *lp, char *cp, i32 n);
void ltol3(char *cp, i32 *lp, i32 n);

#define DISK_DIRENT_SIZE 16

static struct filsys sblock;

static
i32 allocslot()
{
	register i32 fdesc;

	for (fdesc = 0; fdesc < NFILES; fdesc++)
		if (iob[fdesc].i_flgs == 0)
			return(fdesc);
	_stop("No more file slots");
}

static void
openi(ino_t n, register struct iob *io)
{
	register struct dinode *dp;

	io->i_offset = 0;
	io->i_bn = (daddr_t)((n+15)/INOPB) + io->i_boff;
	io->i_cc = 512;
	io->i_ma = io->i_buf;
	devread(io);

	dp = (struct dinode *)io->i_buf;
	dp += (n - 1) % INOPB;
	io->i_ino.i_number = n;
	io->i_ino.i_mode = dp->di_mode;
	io->i_ino.i_nlink = dp->di_nlink;
	io->i_ino.i_uid = dp->di_uid;
	io->i_ino.i_gid = dp->di_gid;
	io->i_ino.i_size = dp->di_size;
	l3tol(io->i_ino.i_un.i_file.i_addr, dp->di_addr, NADDR);
	io->i_ino.i_un.i_file.i_lastr = 0;
}


static
i32 find(path, file)
register char *path;
struct iob *file;
{
	register char *q;
	char c;
	i32 n;

	if (path==NULL || *path=='\0') {
		printf("null path\n");
		return(0);
	}

	openi((ino_t) 2, file);
	while (*path) {
		while (*path == '/')
			path++;
		q = path;
		while(*q != '/' && *q != '\0')
			q++;
		c = *q;
		*q = '\0';

		if ((n=dlook(path, file))!=0) {
			if (c=='\0')
				break;
			openi(n, file);
			*q = c;
			path = q;
			continue;
		} else {
			printf("%s not found\n",path);
			return(0);
		}
	}
	return(n);
}

static
i32 path_parent(path, file, name)
register char *path;
register struct iob *file;
char **name;
{
	register char *cp, *last;
	ino_t ino;

	while (*path == '/')
		path++;
	if (*path == '\0') {
		printf("null path\n");
		return(-1);
	}
	last = 0;
	for (cp = path; *cp; cp++)
		if (*cp == '/')
			last = cp;
	if (last != 0) {
		*last = '\0';
		cp = last + 1;
		while (*cp == '/')
			cp++;
		if (*cp == '\0') {
			printf("null path\n");
			return(-1);
		}
		if ((ino = find(path, file)) == 0)
			return(-1);
		openi(ino, file);
		*name = cp;
	} else {
		openi((ino_t)ROOTINO, file);
		*name = path;
	}
	if ((file->i_ino.i_mode&IFMT) != IFDIR) {
		printf("not a directory\n");
		return(-1);
	}
	return(0);
}

static
i32 setupiob(str, file, path)
char *str;
register struct iob *file;
char **path;
{
	register char *cp;
	register struct devsw *dp;

	for (cp = str; *cp && *cp != '('; cp++)
			;
	if (*cp != '(') {
		printf("Bad device\n");
		return(-1);
	}
	*cp++ = '\0';
	for (dp = devsw; dp->dv_name; dp++) {
		if (strncmp(str, dp->dv_name, DIRSIZ) == 0)
			goto gotdev;
	}
	printf("Unknown device\n");
	return(-1);
gotdev:
	*(cp-1) = '(';
	file->i_ino.i_dev = dp-devsw;
	file->i_unit = *cp++ - '0';
	if (file->i_unit < 0 || file->i_unit > 7) {
		printf("Bad unit specifier\n");
		return(-1);
	}
	if (*cp++ != ',') {
badoff:
		printf("Missing offset specification\n");
		return(-1);
	}
	file->i_boff = atol(cp);
	for (;;) {
		if (*cp == ')')
			break;
		if (*cp++)
			continue;
		goto badoff;
	}
	devopen(file);
	*path = cp + 1;
	return(0);
}

static
void readsb(io)
register struct iob *io;
{

	io->i_bn = SUPERB + io->i_boff;
	io->i_ma = (char *)&sblock;
	io->i_cc = 512;
	devread(io);
}

static
void writesb(io)
register struct iob *io;
{

	sblock.s_fmod = 1;
	io->i_bn = SUPERB + io->i_boff;
	io->i_ma = (char *)&sblock;
	io->i_cc = 512;
	devwrite(io);
}

static
void blkclr(io, bn)
register struct iob *io;
daddr_t bn;
{
	register char *cp;

	cp = b[0];
	memset(cp, 0, 512);
	io->i_bn = bn + io->i_boff;
	io->i_ma = b[0];
	io->i_cc = 512;
	devwrite(io);
}

static daddr_t
alloc(io)
register struct iob *io;
{
	register struct fblk *fp;
	daddr_t bno;

	readsb(io);
	do {
		if (sblock.s_nfree <= 0)
			goto nospace;
		if (sblock.s_nfree > NICFREE)
			goto nospace;
		bno = sblock.s_free[--sblock.s_nfree];
		if (bno == (daddr_t)0)
			goto nospace;
	} while (bno < sblock.s_isize || bno >= sblock.s_fsize);
	if (sblock.s_nfree <= 0) {
		io->i_bn = bno + io->i_boff;
		io->i_ma = b[0];
		io->i_cc = 512;
		devread(io);
		fp = (struct fblk *)b[0];
		sblock.s_nfree = fp->df_nfree;
		memcpy((char *)sblock.s_free, (char *)fp->df_free,
		    sizeof(sblock.s_free));
	}
	sblock.s_tfree--;
	writesb(io);
	blkclr(io, bno);
	return(bno);

nospace:
	sblock.s_nfree = 0;
	writesb(io);
	printf("alloc failed\n");
	return((daddr_t)0);
}

static
void bfree(io, bno)
register struct iob *io;
daddr_t bno;
{
	register struct fblk *fp;

	if (bno == (daddr_t)0)
		return;
	if (bno < sblock.s_isize || bno >= sblock.s_fsize) {
		printf("bad block %D\n", bno);
		return;
	}
	if (sblock.s_nfree <= 0) {
		sblock.s_nfree = 1;
		sblock.s_free[0] = 0;
	}
	if (sblock.s_nfree >= NICFREE) {
		fp = (struct fblk *)b[0];
		fp->df_nfree = sblock.s_nfree;
		memcpy((char *)fp->df_free, (char *)sblock.s_free,
		    sizeof(sblock.s_free));
		io->i_bn = bno + io->i_boff;
		io->i_ma = b[0];
		io->i_cc = 512;
		devwrite(io);
		sblock.s_nfree = 0;
	}
	sblock.s_free[sblock.s_nfree++] = bno;
	sblock.s_tfree++;
}

static
void writeino(io)
register struct iob *io;
{
	register struct dinode *dp;
	register struct inode *ip;

	ip = &io->i_ino;
	io->i_bn = (daddr_t)((ip->i_number+15)/INOPB) + io->i_boff;
	io->i_ma = b[0];
	io->i_cc = 512;
	devread(io);
	dp = (struct dinode *)b[0];
	dp += (ip->i_number - 1) % INOPB;
	dp->di_mode = ip->i_mode;
	dp->di_nlink = ip->i_nlink;
	dp->di_uid = ip->i_uid;
	dp->di_gid = ip->i_gid;
	dp->di_size = ip->i_size;
	dp->di_atime = EPOCH68_V7_FILE_TIME;
	dp->di_mtime = EPOCH68_V7_FILE_TIME;
	dp->di_ctime = EPOCH68_V7_FILE_TIME;
	ltol3(dp->di_addr, ip->i_un.i_file.i_addr, NADDR);
	io->i_bn = (daddr_t)((ip->i_number+15)/INOPB) + io->i_boff;
	io->i_ma = b[0];
	io->i_cc = 512;
	devwrite(io);
}

static ino_t
ialloc(register struct iob *io, i32 mode, dev_t dev)
{
	register struct dinode *dp;
	ino_t ino;
	daddr_t adr;
	i32 i;

	readsb(io);
	for (ino = ROOTINO+1; itod(ino) < sblock.s_isize; ino++) {
		adr = itod(ino);
		io->i_bn = adr + io->i_boff;
		io->i_ma = b[0];
		io->i_cc = 512;
		devread(io);
		dp = (struct dinode *)b[0];
		dp += itoo(ino);
		if (dp->di_mode != 0)
			continue;
		sblock.s_tinode--;
		writesb(io);
		io->i_ino.i_number = ino;
		io->i_ino.i_mode = mode;
		io->i_ino.i_nlink = ((mode&IFMT) == IFDIR) ? 2 : 1;
		io->i_ino.i_uid = 0;
		io->i_ino.i_gid = 0;
		io->i_ino.i_size = 0;
		for (i = 0; i < NADDR; i++)
			io->i_ino.i_un.i_file.i_addr[i] = (daddr_t)0;
		if ((mode&IFMT) == IFCHR || (mode&IFMT) == IFBLK)
			io->i_ino.i_un.i_file.i_addr[0] = dev;
		io->i_ino.i_un.i_file.i_lastr = 0;
		writeino(io);
		return(ino);
	}
	printf("out of inodes\n");
	return((ino_t)0);
}

static i32
adddir(register struct iob *io, char *name, ino_t ino)
{
	struct direct d;
	register i32 i;
	i32 n, dc;
	daddr_t bn, dbn;
	struct direct *dp;
	off_t off;

	d.d_ino = ino;
	memset(d.d_name, 0, DIRSIZ);
	for (i = 0; i < DIRSIZ; i++)
		if ((d.d_name[i] = name[i]) == 0)
			break;
	off = io->i_ino.i_size;
	n = io->i_ino.i_size/DISK_DIRENT_SIZE;
	dc = 512;
	bn = (daddr_t)0;
	while (n--) {
		if (++dc >= 512/DISK_DIRENT_SIZE) {
			dbn = sbmap(io, bn++);
			io->i_bn = dbn + io->i_boff;
			io->i_ma = io->i_buf;
			io->i_cc = 512;
			devread(io);
			dp = (struct direct *)io->i_buf;
			dc = 0;
		}
		if (dp->d_ino == 0) {
			off = ((bn - 1) * (off_t)512) +
			    (dc * DISK_DIRENT_SIZE);
			break;
		}
		dp++;
	}
	io->i_offset = off;
	io->i_cc = 0;
	if (writei(io, (char *)&d, sizeof(d)) != sizeof(d))
		return(-1);
	return(0);
}

static ino_t
create(path, file)
register char *path;
register struct iob *file;
{
	ino_t ino;

	if ((ino = maknode(path, file, IFREG|0666, (dev_t)0)) == 0)
		return((ino_t)0);
	return(ino);
}

static ino_t
maknode(register char *path, register struct iob *file, i32 mode, dev_t dev)
{
	char *name;
	ino_t ino;
	struct inode parentino, newino;
	i32 i;
	struct direct d[2];

	if (path_parent(path, file, &name) < 0)
		return((ino_t)0);
	if ((ino = dlook(name, file)) != 0) {
		if ((mode&IFMT) != IFREG) {
			printf("%s exists\n", name);
			return((ino_t)0);
		}
		return(ino);
	}

	parentino = file->i_ino;
	ino = ialloc(file, mode, dev);
	if (ino == (ino_t)0)
		return((ino_t)0);
	if ((mode&IFMT) == IFDIR) {
		d[0].d_ino = ino;
		d[1].d_ino = parentino.i_number;
		for (i = 0; i < DIRSIZ; i++) {
			d[0].d_name[i] = 0;
			d[1].d_name[i] = 0;
		}
		d[0].d_name[0] = '.';
		d[1].d_name[0] = '.';
		d[1].d_name[1] = '.';
		file->i_offset = 0;
		file->i_cc = 0;
		if (writei(file, (char *)d, sizeof(d)) != sizeof(d))
			return((ino_t)0);
	}
	newino = file->i_ino;
	file->i_ino = parentino;
	if ((mode&IFMT) == IFDIR)
		file->i_ino.i_nlink++;
	if (adddir(file, name, ino) < 0)
		return((ino_t)0);
	file->i_ino = newino;
	writeino(file);
	for (i = 0; i < NBUFS; i++)
		blknos[i] = (daddr_t)0;
	return(ino);
}

static
void tloop(io, bn, f1, f2)
register struct iob *io;
daddr_t bn;
i32 f1, f2;
{
	register i32 i;
	register daddr_t *bap;
	daddr_t nb;
	i32 level;

	if (bn == (daddr_t)0)
		return;
	level = 1 + f1 + f2;
	io->i_bn = bn + io->i_boff;
	io->i_ma = b[level];
	io->i_cc = 512;
	devread(io);
	bap = (daddr_t *)b[level];
	for (i = NINDIR-1; i >= 0; i--) {
		nb = bap[i];
		if (nb == (daddr_t)0)
			continue;
		if (f1)
			tloop(io, nb, f2, 0);
		else
			bfree(io, nb);
	}
	bfree(io, bn);
}

static
void itrunc(io)
register struct iob *io;
{
	register struct inode *ip;
	register i32 i;
	daddr_t bn;

	ip = &io->i_ino;
	i = ip->i_mode & IFMT;
	if (i != IFREG && i != IFDIR)
		return;
	/* Keep the free-list state in memory while releasing all file blocks. */
	readsb(io);
	for (i = NADDR-1; i >= 0; i--) {
		bn = ip->i_un.i_file.i_addr[i];
		if (bn == (daddr_t)0)
			continue;
		ip->i_un.i_file.i_addr[i] = (daddr_t)0;
		switch (i) {

		default:
			bfree(io, bn);
			break;

		case NADDR-3:
			tloop(io, bn, 0, 0);
			break;

		case NADDR-2:
			tloop(io, bn, 1, 0);
			break;

		case NADDR-1:
			tloop(io, bn, 1, 1);
		}
	}
	writesb(io);
	ip->i_size = 0;
	for (i = 0; i < NBUFS; i++)
		blknos[i] = (daddr_t)0;
	writeino(io);
}

static daddr_t
sbmap(io, bn)
register struct iob *io;
daddr_t bn;
{
	register i32 i;
	register struct inode *ip;
	i32 j, sh;
	daddr_t nb;
	register daddr_t *ibp;

	ip = &io->i_ino;;
	if(bn < 0) {
		printf("bn negative\n");
		return((daddr_t)0);
	}

	/*
	 * blocks 0..NADDR-4 are direct blocks
	 */
	if(bn < NADDR-3) {
		i = bn;
		nb = ip->i_un.i_file.i_addr[i];
		return(nb);
	}

	/*
	 * addresses NADDR-3, NADDR-2, and NADDR-1
	 * have single, double, triple indirect blocks.
	 * the first step is to determine
	 * how many levels of indirection.
	 */
	sh = 0;
	nb = 1;
	bn -= NADDR-3;
	for(j=3; j>0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if(bn < nb)
			break;
		bn -= nb;
	}
	if(j == 0) {
		printf("bn ovf %D\n",bn);
		return((daddr_t)0);
	}

	/*
	 * fetch the address from the inode
	 */
	nb = ip->i_un.i_file.i_addr[NADDR-j];
	if(nb == 0) {
		printf("bn void %D\n",bn);
		return((daddr_t)0);
	}

	/*
	 * fetch through the indirect blocks
	 */
	for(; j<=3; j++) {
		if (blknos[j] != nb) {
			io->i_bn = nb + io->i_boff;
			io->i_ma = b[j];
			io->i_cc = 512;
			devread(io);
			blknos[j] = nb;
		}
		ibp = (daddr_t *)b[j];
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nb = ibp[i];
		if(nb == 0) {
			printf("bn void %D\n",bn);
			return((daddr_t)0);
		}
	}

	return(nb);
}

static daddr_t
wbmap(io, bn)
register struct iob *io;
daddr_t bn;
{
	register i32 i;
	register struct inode *ip;
	i32 j, sh;
	daddr_t nb;
	register daddr_t *ibp;

	ip = &io->i_ino;
	if (bn < 0) {
		printf("bn negative\n");
		return((daddr_t)0);
	}

	if (bn < NADDR-3) {
		i = bn;
		nb = ip->i_un.i_file.i_addr[i];
		if (nb == (daddr_t)0) {
			nb = alloc(io);
			if (nb == (daddr_t)0)
				return((daddr_t)0);
			ip->i_un.i_file.i_addr[i] = nb;
		}
		return(nb);
	}

	sh = 0;
	nb = 1;
	bn -= NADDR-3;
	for (j = 3; j > 0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if (bn < nb)
			break;
		bn -= nb;
	}
	if (j == 0) {
		printf("bn ovf %D\n", bn);
		return((daddr_t)0);
	}

	nb = ip->i_un.i_file.i_addr[NADDR-j];
	if (nb == (daddr_t)0) {
		nb = alloc(io);
		if (nb == (daddr_t)0)
			return((daddr_t)0);
		ip->i_un.i_file.i_addr[NADDR-j] = nb;
	}

	for (; j <= 3; j++) {
		if (blknos[j] != nb) {
			io->i_bn = nb + io->i_boff;
			io->i_ma = b[j];
			io->i_cc = 512;
			devread(io);
			blknos[j] = nb;
		}
		ibp = (daddr_t *)b[j];
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nb = ibp[i];
		if (nb == (daddr_t)0) {
			nb = alloc(io);
			if (nb == (daddr_t)0)
				return((daddr_t)0);
			ibp[i] = nb;
			io->i_bn = blknos[j] + io->i_boff;
			io->i_ma = b[j];
			io->i_cc = 512;
			devwrite(io);
		}
	}

	return(nb);
}

static ino_t
dlook(s, io)
char *s;
register struct iob *io;
{
	register struct direct *dp;
	register struct inode *ip;
	daddr_t bn;
	i32 n,dc;

	if (s==NULL || *s=='\0')
		return(0);
	ip = &io->i_ino;
	if ((ip->i_mode&IFMT)!=IFDIR) {
		printf("not a directory\n");
		return(0);
	}

	n = ip->i_size/DISK_DIRENT_SIZE;

	if (n==0) {
		printf("zero length directory\n");
		return(0);
	}

	dc = 512;
	bn = (daddr_t)0;
	while(n--) {
		if (++dc >= 512/DISK_DIRENT_SIZE) {
			io->i_bn = sbmap(io, bn++) + io->i_boff;
			io->i_ma = io->i_buf;
			io->i_cc = 512;
			devread(io);
			dp = (struct direct *)io->i_buf;
			dc = 0;
		}
		if (strncmp(s, dp->d_name, DIRSIZ) == 0)
			return(dp->d_ino);
		dp++;
	}
	return(0);
}

i32 lseek(fdesc, addr, ptr)
i32	fdesc;
off_t	addr;
i32	ptr;
{
	register struct iob *io;

	if (ptr != 0) {
		printf("Seek not from beginning of file\n");
		return(-1);
	}
	if ((addr & 0777) != 0)
		return(-1);
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((io = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	io->i_offset = addr;
	io->i_bn = addr/512 + io->i_boff;
	io->i_cc = 0;
	return(0);
}

i32 read(fdesc, buf, count)
i32	fdesc;
char	*buf;
i32	count;
{
	register i32 i;
	register struct iob *file;
	i32 n, off;
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	if ((file->i_flgs&F_READ) == 0)
		return(-1);
	if ((file->i_flgs&F_FILE) == 0) {
		file->i_cc = count;
		file->i_ma = buf;
		i = devread(file);
		file->i_bn++;
		return(i);
	}
	if (file->i_offset >= file->i_ino.i_size)
		return(0);
	file->i_bn = file->i_offset/(off_t)512;
	off = file->i_offset%(off_t)512;
	file->i_bn = sbmap(file, file->i_bn) + file->i_boff;
	file->i_ma = file->i_buf;
	file->i_cc = 512;
	devread(file);
	n = file->i_ino.i_size - file->i_offset;
	if (n > 512 - off)
		n = 512 - off;
	if (n > count)
		n = count;
	memcpy(buf, file->i_buf + off, n);
	file->i_offset += n;
	file->i_cc = 0;
	return(n);
}

static i32
writei(file, buf, count)
register struct iob *file;
char *buf;
i32 count;
{
	register i32 i;
	register char *cp;
	i32 n, off, resid;
	daddr_t bn, nb;

	resid = count;
	while (resid > 0) {
		bn = file->i_offset/(off_t)512;
		off = file->i_offset%(off_t)512;
		n = 512 - off;
		if (n > resid)
			n = resid;
		nb = wbmap(file, bn);
		if (nb == (daddr_t)0)
			break;
		if (off != 0 || n != 512) {
			file->i_bn = nb + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devread(file);
		}
		cp = &file->i_buf[off];
		i = n;
		while (i--)
			*cp++ = *buf++;
		file->i_bn = nb + file->i_boff;
		file->i_ma = file->i_buf;
		file->i_cc = 512;
		devwrite(file);
		file->i_offset += n;
		if (file->i_offset > file->i_ino.i_size)
			file->i_ino.i_size = file->i_offset;
		resid -= n;
	}
	writeino(file);
	file->i_cc = 0;
	return(count - resid);
}

i32 write(fdesc, buf, count)
i32	fdesc;
char	*buf;
i32	count;
{
	register i32 i;
	register struct iob *file;
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	if ((file->i_flgs&F_WRITE) == 0)
		return(-1);
	if (file->i_flgs&F_FILE)
		return(writei(file, buf, count));
	file->i_cc = count;
	file->i_ma = buf;
	i = devwrite(file);
	file->i_bn++;
	return(i);
}

i32 mkdir(str)
char *str;
{
	register struct iob *file;
	char *path;
	i32 fdesc;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	if (setupiob(str, file, &path) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if (maknode(path, file, IFDIR|0777, (dev_t)0) == (ino_t)0) {
		file->i_flgs = 0;
		return(-1);
	}
	file->i_flgs = 0;
	return(0);
}

i32 exists(str)
char *str;
{
	register struct iob *file;
	char *path;
	char *name;
	char pathbuf[100];
	i32 fdesc;
	ino_t ino;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	memcpy(pathbuf, str, strlen(str) + 1);
	if (setupiob(pathbuf, file, &path) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if (path_parent(path, file, &name) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	ino = dlook(name, file);
	file->i_flgs = 0;
	if (ino == (ino_t)0)
		return(0);
	return(1);
}

i32 chmod(str, mode)
char *str;
i32 mode;
{
	register struct iob *file;
	char *path;
	i32 fdesc;
	ino_t ino;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	if (setupiob(str, file, &path) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if ((ino = find(path, file)) == 0) {
		file->i_flgs = 0;
		return(-1);
	}
	openi(ino, file);
	file->i_ino.i_mode &= IFMT;
	file->i_ino.i_mode |= (mode & ~IFMT);
	writeino(file);
	file->i_flgs = 0;
	return(0);
}

i32 chgrp(str, gid)
char *str;
i32 gid;
{
	register struct iob *file;
	char *path;
	i32 fdesc;
	ino_t ino;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	if (setupiob(str, file, &path) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if ((ino = find(path, file)) == 0) {
		file->i_flgs = 0;
		return(-1);
	}
	openi(ino, file);
	file->i_ino.i_gid = gid;
	writeino(file);
	file->i_flgs = 0;
	return(0);
}

i32 chown(str, uid)
char *str;
i32 uid;
{
	register struct iob *file;
	char *path;
	i32 fdesc;
	ino_t ino;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	if (setupiob(str, file, &path) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if ((ino = find(path, file)) == 0) {
		file->i_flgs = 0;
		return(-1);
	}
	openi(ino, file);
	file->i_ino.i_uid = uid;
	writeino(file);
	file->i_flgs = 0;
	return(0);
}

i32
mknod(char *str, i32 mode, dev_t dev)
{
	register struct iob *file;
	char *path;
	i32 fdesc;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	if ((mode&IFMT) == 0)
		mode |= IFREG;
	if (setupiob(str, file, &path) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if (maknode(path, file, mode, dev) == (ino_t)0) {
		file->i_flgs = 0;
		return(-1);
	}
	file->i_flgs = 0;
	return(0);
}

/* Create a hard link to an existing non-directory inode on the same V7 FS. */
i32
link(char *src, char *dst)
{
	struct iob *srcfile;
	struct iob *dstfile;
	char *srcpath;
	char *dstpath;
	char *name;
	ino_t ino;
	i32 srcfd;
	i32 dstfd;

	srcfd = allocslot();
	srcfile = &iob[srcfd];
	srcfile->i_flgs = F_ALLOC;
	if (setupiob(src, srcfile, &srcpath) < 0)
		goto badsrc;
	if ((ino = find(srcpath, srcfile)) == (ino_t)0)
		goto badsrc;
	openi(ino, srcfile);
	if ((srcfile->i_ino.i_mode & IFMT) == IFDIR) {
		printf("Can't link directory\n");
		goto badsrc;
	}

	dstfd = allocslot();
	dstfile = &iob[dstfd];
	dstfile->i_flgs = F_ALLOC;
	if (setupiob(dst, dstfile, &dstpath) < 0)
		goto baddst;
	if (srcfile->i_ino.i_dev != dstfile->i_ino.i_dev ||
	    srcfile->i_unit != dstfile->i_unit ||
	    srcfile->i_boff != dstfile->i_boff) {
		printf("Can't link across filesystems\n");
		goto baddst;
	}
	if (path_parent(dstpath, dstfile, &name) < 0)
		goto baddst;
	if (strncmp(name, ".", DIRSIZ) == 0 ||
	    strncmp(name, "..", DIRSIZ) == 0) {
		printf("Can't link . or ..\n");
		goto baddst;
	}
	if (dlook(name, dstfile) != (ino_t)0) {
		printf("%s exists\n", name);
		goto baddst;
	}
	if (adddir(dstfile, name, ino) < 0)
		goto baddst;
	srcfile->i_ino.i_nlink++;
	writeino(srcfile);
	dstfile->i_flgs = 0;
	srcfile->i_flgs = 0;
	return(0);

baddst:
	dstfile->i_flgs = 0;
badsrc:
	srcfile->i_flgs = 0;
	return(-1);
}

i32 rmdir(str)
char *str;
{
	register char *cp;
	register struct iob *file;
	register struct devsw *dp;
	char *name;
	i32 i, dc, fdesc;
	i32 parentdc;
	ino_t ino;
	daddr_t bn, dbn;
	daddr_t parentdbn;
	struct direct *dirp;
	struct inode parentino;
	i32 empty;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	for (cp = str; *cp && *cp != '('; cp++)
			;
	if (*cp != '(') {
		printf("Bad device\n");
		file->i_flgs = 0;
		return(-1);
	}
	*cp++ = '\0';
	for (dp = devsw; dp->dv_name; dp++) {
		if (strncmp(str, dp->dv_name, DIRSIZ) == 0)
			goto gotdev;
	}
	printf("Unknown device\n");
	file->i_flgs = 0;
	return(-1);
gotdev:
	*(cp-1) = '(';
	file->i_ino.i_dev = dp-devsw;
	file->i_unit = *cp++ - '0';
	if (file->i_unit < 0 || file->i_unit > 7) {
		printf("Bad unit specifier\n");
		file->i_flgs = 0;
		return(-1);
	}
	if (*cp++ != ',') {
badoff:
		printf("Missing offset specification\n");
		file->i_flgs = 0;
		return(-1);
	}
	file->i_boff = atol(cp);
	for (;;) {
		if (*cp == ')')
			break;
		if (*cp++)
			continue;
		goto badoff;
	}
	devopen(file);
	cp++;
	if (path_parent(cp, file, &name) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if (strncmp(name, ".", DIRSIZ) == 0 || strncmp(name, "..", DIRSIZ) == 0) {
		printf("Can't rmdir . or ..\n");
		file->i_flgs = 0;
		return(-1);
	}
	if ((ino = dlook(name, file)) == 0) {
		printf("%s not found\n", name);
		file->i_flgs = 0;
		return(-1);
	}
	i = file->i_ino.i_size/DISK_DIRENT_SIZE;
	dc = 512;
	bn = (daddr_t)0;
	while (i--) {
		if (++dc >= 512/DISK_DIRENT_SIZE) {
			dbn = sbmap(file, bn++);
			file->i_bn = dbn + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devread(file);
			dirp = (struct direct *)file->i_buf;
			dc = 0;
		}
		if (dirp->d_ino == ino && strncmp(name, dirp->d_name, DIRSIZ) == 0) {
			parentino = file->i_ino;
			parentdc = dc;
			parentdbn = dbn;
			openi(ino, file);
			if ((file->i_ino.i_mode&IFMT) != IFDIR) {
				printf("Not a directory\n");
				file->i_flgs = 0;
				return(-1);
			}
			empty = 1;
			for (i = file->i_ino.i_size/DISK_DIRENT_SIZE, dc = 512, bn = (daddr_t)0;
			    i--;
			    dirp++) {
				if (++dc >= 512/DISK_DIRENT_SIZE) {
					file->i_bn = sbmap(file, bn++) + file->i_boff;
					file->i_ma = file->i_buf;
					file->i_cc = 512;
					devread(file);
					dirp = (struct direct *)file->i_buf;
					dc = 0;
				}
				if (dirp->d_ino == 0)
					continue;
				if (strncmp(dirp->d_name, ".", DIRSIZ) == 0 ||
				    strncmp(dirp->d_name, "..", DIRSIZ) == 0)
					continue;
				empty = 0;
				break;
			}
			if (!empty) {
				printf("Directory not empty\n");
				file->i_flgs = 0;
				return(-1);
			}
			file->i_ino = parentino;
			file->i_bn = parentdbn + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devread(file);
			dirp = (struct direct *)file->i_buf;
			dirp += parentdc;
			dirp->d_ino = 0;
			file->i_bn = parentdbn + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devwrite(file);
			if (file->i_ino.i_nlink > 0)
				file->i_ino.i_nlink--;
			writeino(file);
			openi(ino, file);
			itrunc(file);
			file->i_ino.i_mode = 0;
			file->i_ino.i_nlink = 0;
			file->i_ino.i_uid = 0;
			file->i_ino.i_gid = 0;
			file->i_ino.i_size = 0;
			writeino(file);
			readsb(file);
			sblock.s_tinode++;
			writesb(file);
			file->i_flgs = 0;
			return(0);
		}
		dirp++;
	}
	file->i_flgs = 0;
	return(-1);
}

i32 unlink(str)
char *str;
{
	register char *cp;
	register struct iob *file;
	register struct devsw *dp;
	char pathbuf[128];
	char *name;
	i32 i, dc, fdesc;
	ino_t ino;
	daddr_t bn, dbn;
	struct direct *dirp;
	i32 freeino;

	fdesc = allocslot();
	file = &iob[fdesc];
	file->i_flgs = F_ALLOC;
	if (strlen(str) + 1 > sizeof(pathbuf)) {
		printf("Path too long\n");
		file->i_flgs = 0;
		return(-1);
	}
	strcpy(pathbuf, str);
	for (cp = pathbuf; *cp && *cp != '('; cp++)
			;
	if (*cp != '(') {
		printf("Bad device\n");
		file->i_flgs = 0;
		return(-1);
	}
	*cp++ = '\0';
	for (dp = devsw; dp->dv_name; dp++) {
		if (strncmp(pathbuf, dp->dv_name, DIRSIZ) == 0)
			goto gotdev;
	}
	printf("Unknown device\n");
	file->i_flgs = 0;
	return(-1);
gotdev:
	*(cp-1) = '(';
	file->i_ino.i_dev = dp-devsw;
	file->i_unit = *cp++ - '0';
	if (file->i_unit < 0 || file->i_unit > 7) {
		printf("Bad unit specifier\n");
		file->i_flgs = 0;
		return(-1);
	}
	if (*cp++ != ',') {
badoff:
		printf("Missing offset specification\n");
		file->i_flgs = 0;
		return(-1);
	}
	file->i_boff = atol(cp);
	for (;;) {
		if (*cp == ')')
			break;
		if (*cp++)
			continue;
		goto badoff;
	}
	devopen(file);
	cp++;
	if (path_parent(cp, file, &name) < 0) {
		file->i_flgs = 0;
		return(-1);
	}
	if (strncmp(name, ".", DIRSIZ) == 0 || strncmp(name, "..", DIRSIZ) == 0) {
		printf("Can't unlink . or ..\n");
		file->i_flgs = 0;
		return(-1);
	}
	if ((ino = dlook(name, file)) == 0) {
		printf("%s not found\n", name);
		file->i_flgs = 0;
		return(-1);
	}
	i = file->i_ino.i_size/DISK_DIRENT_SIZE;
	dc = 512;
	bn = (daddr_t)0;
	while (i--) {
		if (++dc >= 512/DISK_DIRENT_SIZE) {
			dbn = sbmap(file, bn++);
			file->i_bn = dbn + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devread(file);
			dirp = (struct direct *)file->i_buf;
			dc = 0;
		}
		if (dirp->d_ino == ino && strncmp(name, dirp->d_name, DIRSIZ) == 0) {
			openi(ino, file);
			if ((file->i_ino.i_mode&IFMT) == IFDIR) {
				printf("Can't unlink directory\n");
				file->i_flgs = 0;
				return(-1);
			}
			file->i_bn = dbn + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devread(file);
			dirp = (struct direct *)file->i_buf;
			dirp += dc;
			dirp->d_ino = 0;
			file->i_bn = dbn + file->i_boff;
			file->i_ma = file->i_buf;
			file->i_cc = 512;
			devwrite(file);
			freeino = 0;
			if (file->i_ino.i_nlink > 0)
				file->i_ino.i_nlink--;
			if (file->i_ino.i_nlink == 0) {
				itrunc(file);
				file->i_ino.i_mode = 0;
				file->i_ino.i_uid = 0;
				file->i_ino.i_gid = 0;
				file->i_ino.i_size = 0;
				freeino = 1;
			}
			writeino(file);
			if (freeino) {
				readsb(file);
				sblock.s_tinode++;
				writesb(file);
			}
			file->i_flgs = 0;
			return(0);
		}
		dirp++;
	}
	file->i_flgs = 0;
	return(-1);
}

i32 open(str, how)
char *str;
i32	how;
{
	register char *cp;
	i32 i;
	register struct iob *file;
	register struct devsw *dp;
	i32	fdesc;
	static i32 first = 1;

	if (first) {
		for (i = 0; i < NFILES; i++)
			iob[i].i_flgs = 0;
		first = 0;
	}

	fdesc = allocslot();
	(file = &iob[fdesc])->i_flgs |= F_ALLOC;

	for (cp = str; *cp && *cp != '('; cp++)
			;
	if (*cp != '(') {
		printf("Bad device\n");
		file->i_flgs = 0;
		return(-1);
	}
	*cp++ = '\0';
	for (dp = devsw; dp->dv_name; dp++) {
		if (strncmp(str, dp->dv_name, DIRSIZ) == 0)
			goto gotdev;
	}
	printf("Unknown device\n");
	file->i_flgs = 0;
	return(-1);
gotdev:
	*(cp-1) = '(';
	file->i_ino.i_dev = dp-devsw;
	file->i_unit = *cp++ - '0';
	if (file->i_unit < 0 || file->i_unit > 7) {
		printf("Bad unit specifier\n");
		file->i_flgs = 0;
		return(-1);
	}
	if (*cp++ != ',') {
badoff:
		printf("Missing offset specification\n");
		file->i_flgs = 0;
		return(-1);
	}
	file->i_boff = atol(cp);
	for (;;) {
		if (*cp == ')')
			break;
		if (*cp++)
			continue;
		goto badoff;
	}
	devopen(file);
	if (*++cp == '\0') {
		file->i_flgs |= how+1;
		file->i_cc = 0;
		file->i_offset = 0;
		return(fdesc+3);
	}
	if (how != 0) {
		i = create(cp, file);
	} else {
		i = find(cp, file);
	}
	if (i == 0) {
		file->i_flgs = 0;
		return(-1);
	}
	openi(i, file);
	if (how != 0) {
		if ((file->i_ino.i_mode&IFMT) == IFDIR) {
			printf("Can't write directory\n");
			file->i_flgs = 0;
			return(-1);
		}
		itrunc(file);
	}
	file->i_offset = 0;
	file->i_cc = 0;
	file->i_flgs |= F_FILE | (how+1);
	return(fdesc+3);
}

i32 close(fdesc)
i32	fdesc;
{
	struct iob *file;

	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES || ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0)
		return(-1);
	if ((file->i_flgs&F_FILE) == 0)
		devclose(file);
	file->i_flgs = 0;
	return(0);
}

void
v7io_close_all()
{
	register i32 i;

	for (i = 0; i < NFILES; i++)
		if (iob[i].i_flgs&F_ALLOC)
			close(i + 3);
}

void _stop(s)
char	*s;
{
	printf("%s\n", s);
	for(;;);
}
