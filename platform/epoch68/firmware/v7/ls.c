/*
 * list file or directory
 */

#include <sys/param.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include "../v7time.h"
#include "prf.h"
#include "../string.h"
#include "v7io.h"

#define	LS_NFILES	64
#define	LS_FIXED_TIME	EPOCH68_V7_FILE_LS_TIME
static int	dirf;

i32 ls(char *path);

struct lbuf {
	union {
		char	lname[15];
		char	*namep;
	} ln;
	char	ltype;
	short	lnum;
	short	lflags;
	short	lnl;
	short	luid;
	short	lgid;
	long	lsize;
};

static int	flags;
static long	tblocks;
static struct	lbuf	lbuf_pool[LS_NFILES];
static struct	lbuf	*lbuf_next = lbuf_pool;
static struct	lbuf	*flist[LS_NFILES];
static struct	lbuf	**lastp = flist;
static struct	lbuf	**firstp = flist;

static struct lbuf *gstat(char *file, i32 argfl);
static char *makename(char *dir, char *file);
static int namecmp(register struct lbuf *p1, register struct lbuf *p2);
static long nblock(long size);
static void normalize_path(char *src, char *dst);
static void pentry(struct lbuf *ap);
static void pmode(i32 aflag);
static void readdir(char *dir);
static void select(register int *pairp);
static void sort_entries(struct lbuf **first, struct lbuf **last);

#define	ISARG	0100000

i32
ls(char *path)
{
	register struct lbuf *ep, **ep1;
	register struct lbuf **slastp;
	char pathbuf[100];

	lbuf_next = lbuf_pool;
	lastp = flist;
	firstp = flist;
	normalize_path(path, pathbuf);
	if ((ep = gstat(pathbuf, 1))==NULL)
	    return(-1);
	ep->ln.namep = pathbuf;
	ep->lflags |= ISARG;
	slastp = lastp;
	if (ep->ltype=='d') {
		lastp = slastp;
		readdir(ep->ln.namep);
		sort_entries(slastp, lastp);
		printf("total %D\n", tblocks);
		for (ep1=slastp; ep1<lastp; ep1++)
			pentry(*ep1);
	} else
		pentry(ep);
	return(0);
}

static void
normalize_path(char *src, char *dst)
{
	register char *start;
	register char *openp;

	start = dst;
	if (src == 0 || *src == '\0') {
		*dst++ = '.';
		*dst = '\0';
		return;
	}
	if (src[0] == '/' && src[1] == '\0') {
		*dst++ = '.';
		*dst = '\0';
		return;
	}
	while ((*dst = *src) != '\0') {
		dst++;
		src++;
	}
	if (dst > start && dst[-1] == ')') {
		*dst++ = '.';
		*dst = '\0';
		return;
	}
	for (openp = start; *openp && *openp != ')'; openp++)
		;
	if (*openp == ')' && openp[1] == '/' && openp[2] == '\0') {
		openp[1] = '.';
		openp[2] = '\0';
	}
}

static void
pentry(struct lbuf *ap)
{
	register i32 t;
	register struct lbuf *p;

	p = ap;
	if (p->lnum == -1)
		return;
	printf("%5u ", p->lnum);
	putchar(p->ltype);
	pmode(p->lflags);
	printf("%2d ", p->lnl);
	t = p->luid;
	printf("%-6d", t);
	if (p->ltype=='b' || p->ltype=='c')
		printf("%3d,%3d", major((int)p->lsize), minor((int)p->lsize));
	else
		printf("%7ld", p->lsize);
	printf("%s", LS_FIXED_TIME);
	if (p->lflags&ISARG)
		printf("%s\n", p->ln.namep);
	else
		printf("%.14s\n", p->ln.lname);
}

static long
nblock(long size)
{
	return((size+511)>>9);
}

static int	m1[] = { 1, IREAD>>0, 'r', '-' };
static int	m2[] = { 1, IWRITE>>0, 'w', '-' };
static int	m3[] = { 2, ISUID, 's', IEXEC>>0, 'x', '-' };
static int	m4[] = { 1, IREAD>>3, 'r', '-' };
static int	m5[] = { 1, IWRITE>>3, 'w', '-' };
static int	m6[] = { 2, ISGID, 's', IEXEC>>3, 'x', '-' };
static int	m7[] = { 1, IREAD>>6, 'r', '-' };
static int	m8[] = { 1, IWRITE>>6, 'w', '-' };
static int	m9[] = { 2, ISVTX, 't', IEXEC>>6, 'x', '-' };

static int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

static void
pmode(i32 aflag)
{
	register int **mp;

	flags = aflag;
	for (mp = &m[0]; mp < &m[sizeof(m)/sizeof(m[0])];)
		select(*mp++);
}

static void
select(register int *pairp)
{
	register int n;

	n = *pairp++;
	while (--n>=0 && (flags&*pairp++)==0)
		pairp++;
	putchar(*pairp);
}

static char *
makename(char *dir, char *file)
{
	static char dfile[100];
	register char *dp, *fp;
	register int i;

	dp = dfile;
	fp = dir;
	while (*fp)
		*dp++ = *fp++;
	*dp++ = '/';
	fp = file;
	for (i=0; i<DIRSIZ; i++)
		*dp++ = *fp++;
	*dp = 0;
	return(dfile);
}

static void
readdir(char *dir)
{
	char blk[512];
	struct direct *dp;
	register int j;
	register struct lbuf *ep;
	int n, nent;

	if ((dirf = open(dir, 0)) < 0) {
		printf("%s unreadable\n", dir);
		return;
	}
	tblocks = 0;
	for(;;) {
		n = read(dirf, blk, sizeof(blk));
		if (n <= 0)
			break;
		nent = n / sizeof(struct direct);
		dp = (struct direct *)blk;
		for (; nent > 0; nent--, dp++) {
			if (dp->d_ino==0)
				continue;
			ep = gstat(makename(dir, dp->d_name), 0);
			if (ep==NULL)
				continue;
			if (ep->lnum != -1)
				ep->lnum = dp->d_ino;
			for (j=0; j<DIRSIZ; j++)
				ep->ln.lname[j] = dp->d_name[j];
			ep->ln.lname[DIRSIZ] = '\0';
		}
		if (n < sizeof(blk))
			break;
	}
	close(dirf);
}

static void
sort_entries(struct lbuf **first, struct lbuf **last)
{
	register struct lbuf **i, **j;
	register struct lbuf *tmp;

	for (i = first + 1; i < last; i++) {
		tmp = *i;
		j = i;
		while (j > first && namecmp(*(j - 1), tmp) > 0) {
			*j = *(j - 1);
			j--;
		}
		*j = tmp;
	}
}

static struct lbuf *
gstat(char *file, i32 argfl)
{
	register struct lbuf *rep;
	register int fd;
	register struct iob *io;
	if (lbuf_next >= &lbuf_pool[LS_NFILES]) {
		printf("ls: out of lbuf slots\n");
		return(NULL);
	}
	rep = lbuf_next++;
	*lastp++ = rep;
	rep->lflags = 0;
	rep->lnum = 0;
	rep->ltype = '-';
	fd = open(file, 0);
	if (fd < 0) {
		printf("%s not found\n", file);
		rep->lnum = -1;
		if (argfl) {
			lbuf_next--;
			lastp--;
			return(0);
		}
		return(rep);
	}
	io = &iob[fd - 3];
	rep->lnum = io->i_ino.i_number;
	rep->lsize = io->i_ino.i_size;
	switch(io->i_ino.i_mode&IFMT) {

	case IFDIR:
		rep->ltype = 'd';
		break;

	case IFBLK:
		rep->ltype = 'b';
		rep->lsize = io->i_ino.i_un.i_file.i_addr[0];
		break;

	case IFCHR:
		rep->ltype = 'c';
		rep->lsize = io->i_ino.i_un.i_file.i_addr[0];
		break;
	}
	rep->lflags = io->i_ino.i_mode & ~IFMT;
	rep->luid = io->i_ino.i_uid;
	rep->lgid = io->i_ino.i_gid;
	rep->lnl = io->i_ino.i_nlink;
	tblocks += nblock(io->i_ino.i_size);
	close(fd);
	return(rep);
}

static int
namecmp(register struct lbuf *p1, register struct lbuf *p2)
{
	register char *s1, *s2;

	if (p1->lflags&ISARG && p1->ltype=='d') {
		if (!(p2->lflags&ISARG && p2->ltype=='d'))
			return(1);
	} else {
		if (p2->lflags&ISARG && p2->ltype=='d')
			return(-1);
	}
	s1 = p1->lflags&ISARG ? p1->ln.namep : p1->ln.lname;
	s2 = p2->lflags&ISARG ? p2->ln.namep : p2->ln.lname;
	return(strcmp(s1, s2));
}
