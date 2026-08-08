#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void usage(void);
static void dorep(char *argv[]);
static i32 endtape(void);
static void getdir(void);
static void passtape(void);
static void putfile(char *longname, char *shortname);
static void doxtract(char *argv[]);
static void dotable(void);
static void putempty(void);
static void longt(struct stat *st);
static void pmode(struct stat *st);
static void select(i32 *pairp, struct stat *st);
static void checkdir(char *name);
static void onintr(void);
static void onquit(void);
static void onhup(void);
static void onterm(void);
static void tomodes(struct stat *sp);
static i32 checksum(void);
static i32 checkw(i32 c, char *name);
static i32 response(void);
static i32 checkupdate(char *arg);
static long fromoct(char *cp, i32 n);
static void done(i32 n);
static i32 prefix(char *s1, char *s2);
static void getwdir(char *s);
static daddr_t lookup(char *s);
static daddr_t bsrch(char *s, i32 n, daddr_t l, daddr_t h);
static i32 cmp(char *b, char *s, i32 n);
static i32 readtape(char *buffer);
static i32 writetape(char *buffer);
static void backtape(void);
static void flushtape(void);
static void copy(char *to, char *from);
char *mktemp(char *template);

#define TBLOCK	512
#define NBLOCK	20
#define NAMSIZ	100
union hblock {
	char dummy[TBLOCK];
	struct header {
		char name[NAMSIZ];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char chksum[8];
		char linkflag;
		char linkname[NAMSIZ];
	} dbuf;
} dblock, tbuf[NBLOCK];

struct linkbuf {
	ino_t	inum;
	dev_t	devnum;
	int	count;
	char	pathname[NAMSIZ];
	struct	linkbuf *nextp;
} *ihead;

struct stat stbuf;

int	rflag, xflag, vflag, tflag, mt, cflag, mflag;
int	term, chksum, wflag, recno, first, linkerrok;
int	freemem = 1;
int	nblock = 1;

daddr_t	low;
daddr_t	high;

FILE	*tfile;
char	tname[] = "/tmp/tarXXXXXX";


char	*usefile;
char	magtape[]	= "/dev/mt1";

char	*ctime(long *clock);

int
main(int argc, char *argv[])
{
	char *cp;

	if (argc < 2)
		usage();

	tfile = NULL;
	usefile =  magtape;
	argv[argc] = 0;
	argv++;
	for (cp = *argv++; *cp; cp++) 
		switch(*cp) {
		case 'f':
			usefile = *argv++;
			if (nblock == 1)
				nblock = 0;
			break;
		case 'c':
			cflag++;
			rflag++;
			break;
		case 'u':
			mktemp(tname);
			if ((tfile = fopen(tname, "w")) == NULL) {
				fprintf(stderr, "Tar: cannot create temporary file (%s)\n", tname);
				done(1);
			}
			fprintf(tfile, "!!!!!/!/!/!/!/!/!/! 000\n");
			/* FALL THROUGH */
		case 'r':
			rflag++;
			if (nblock != 1 && cflag == 0) {
noupdate:
				fprintf(stderr, "Tar: Blocked tapes cannot be updated (yet)\n");
				done(1);
			}
			break;
		case 'v':
			vflag++;
			break;
		case 'w':
			wflag++;
			break;
		case 'x':
			xflag++;
			break;
		case 't':
			tflag++;
			break;
		case 'm':
			mflag++;
			break;
		case '-':
			break;
		case '0':
		case '1':
			magtape[7] = *cp;
			usefile = magtape;
			break;
		case 'b':
			nblock = atoi(*argv++);
			if (nblock > NBLOCK || nblock <= 0) {
				fprintf(stderr, "Invalid blocksize. (Max %d)\n", NBLOCK);
				done(1);
			}
			if (rflag && !cflag)
				goto noupdate;
			break;
		case 'l':
			linkerrok++;
			break;
		default:
			fprintf(stderr, "tar: %c: unknown option\n", *cp);
			usage();
		}

	if (rflag) {
		if (cflag && tfile != NULL) {
			usage();
			done(1);
		}
			if (signal(SIGINT, SIG_IGN) != SIG_IGN)
				signal(SIGINT, (sighandler_t)onintr);
			if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
				signal(SIGHUP, (sighandler_t)onhup);
			if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
				signal(SIGQUIT, (sighandler_t)onquit);
/*
		if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
			signal(SIGTERM, onterm);
*/
		if (strcmp(usefile, "-") == 0) {
			if (cflag == 0) {
				fprintf(stderr, "Can only create standard output archives\n");
				done(1);
			}
			mt = dup(1);
			nblock = 1;
		}
		else if ((mt = open(usefile, 2)) < 0) {
			if (cflag == 0 || (mt =  creat(usefile, 0666)) < 0) {
				fprintf(stderr, "tar: cannot open %s\n", usefile);
				done(1);
			}
		}
		if (cflag == 0 && nblock == 0)
			nblock = 1;
		dorep(argv);
	}
	else if (xflag)  {
		if (strcmp(usefile, "-") == 0) {
			mt = dup(0);
			nblock = 1;
		}
		else if ((mt = open(usefile, 0)) < 0) {
			fprintf(stderr, "tar: cannot open %s\n", usefile);
			done(1);
		}
		doxtract(argv);
	}
	else if (tflag) {
		if (strcmp(usefile, "-") == 0) {
			mt = dup(0);
			nblock = 1;
		}
		else if ((mt = open(usefile, 0)) < 0) {
			fprintf(stderr, "tar: cannot open %s\n", usefile);
			done(1);
		}
		dotable();
	}
	else
		usage();
	done(0);
}

static void
usage(void)
{
	fprintf(stderr, "tar: usage  tar -{txru}[cvfblm] [tapefile] [blocksize] file1 file2...\n");
	done(1);
}

static void
dorep(char *argv[])
{
	register char *cp, *cp2;
	char wdir[60];

	if (!cflag) {
		getdir();
		do {
			passtape();
			if (term)
				done(0);
			getdir();
		} while (!endtape());
		if (tfile != NULL) {
			char buf[200];

			sprintf(buf, "sort +0 -1 +1nr %s -o %s; awk '$1 != prev {print; prev=$1}' %s >%sX;mv %sX %s",
				tname, tname, tname, tname, tname, tname);
			fflush(tfile);
			system(buf);
			freopen(tname, "r", tfile);
			fstat(fileno(tfile), &stbuf);
			high = stbuf.st_size;
		}
	}

	getwdir(wdir);
	while (*argv && ! term) {
		cp2 = *argv;
		for (cp = *argv; *cp; cp++)
			if (*cp == '/')
				cp2 = cp;
		if (cp2 != *argv) {
			*cp2 = '\0';
			chdir(*argv);
			*cp2 = '/';
			cp2++;
		}
		putfile(*argv++, cp2);
		chdir(wdir);
	}
	putempty();
	putempty();
	flushtape();
	if (linkerrok == 1)
		for (; ihead != NULL; ihead = ihead->nextp)
			if (ihead->count != 0)
				fprintf(stderr, "Missing links to %s\n", ihead->pathname);
}

static i32
endtape(void)
{
	if (dblock.dbuf.name[0] == '\0') {
		backtape();
		return(1);
	}
	else
		return(0);
}

static void
getdir(void)
{
	register struct stat *sp;
	int i;

	readtape( (char *) &dblock);
	if (dblock.dbuf.name[0] == '\0')
		return;
	sp = &stbuf;
	sp->st_mode = (int)fromoct(dblock.dbuf.mode, sizeof dblock.dbuf.mode);
	sp->st_uid = (int)fromoct(dblock.dbuf.uid, sizeof dblock.dbuf.uid);
	sp->st_gid = (int)fromoct(dblock.dbuf.gid, sizeof dblock.dbuf.gid);
	sp->st_size = fromoct(dblock.dbuf.size, sizeof dblock.dbuf.size);
	sp->st_mtime = fromoct(dblock.dbuf.mtime, sizeof dblock.dbuf.mtime);
	chksum = (int)fromoct(dblock.dbuf.chksum, sizeof dblock.dbuf.chksum);
	if (chksum != checksum()) {
		fprintf(stderr, "directory checksum error\n");
		done(2);
	}
	if (tfile != NULL)
		fprintf(tfile, "%s %s\n", dblock.dbuf.name, dblock.dbuf.mtime);
}

static void
passtape(void)
{
	long blocks;
	char buf[TBLOCK];

	if (dblock.dbuf.linkflag == '1')
		return;
	blocks = stbuf.st_size;
	blocks += TBLOCK-1;
	blocks /= TBLOCK;

	while (blocks-- > 0)
		readtape(buf);
}

static void
putfile(char *longname, char *shortname)
{
	int infile;
	long blocks;
	char buf[TBLOCK];
	register char *cp, *cp2;
	struct direct dbuf;
	int i, j;

	infile = open(shortname, 0);
	if (infile < 0) {
		fprintf(stderr, "tar: %s: cannot open file\n", longname);
		return;
	}

	fstat(infile, &stbuf);
	if (tfile != NULL && checkupdate(longname) == 0) {
		close(infile);
		return;
	}
	if (checkw('r', longname) == 0) {
		close(infile);
		return;
	}

	if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
		for (i = 0, cp = buf; *cp++ = longname[i++];);
		*--cp = '/';
		cp++;
		i = 0;
		chdir(shortname);
		while (read(infile, (char *)&dbuf, sizeof(dbuf)) > 0 && !term) {
			if (dbuf.d_ino == 0) {
				i++;
				continue;
			}
			if (strcmp(".", dbuf.d_name) == 0 || strcmp("..", dbuf.d_name) == 0) {
				i++;
				continue;
			}
			cp2 = cp;
			for (j=0; j < DIRSIZ; j++)
				*cp2++ = dbuf.d_name[j];
			*cp2 = '\0';
			close(infile);
			putfile(buf, cp);
			infile = open(".", 0);
			i++;
			lseek(infile, (long) (sizeof(dbuf) * i), 0);
		}
		close(infile);
		chdir("..");
		return;
	}
	if ((stbuf.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "tar: %s is not a file. Not dumped\n", longname);
		return;
	}

	tomodes(&stbuf);

	cp2 = longname;
	for (cp = dblock.dbuf.name, i=0; (*cp++ = *cp2++) && i < NAMSIZ; i++);
	if (i >= NAMSIZ) {
		fprintf(stderr, "%s: file name too long\n", longname);
		close(infile);
		return;
	}

	if (stbuf.st_nlink > 1) {
		struct linkbuf *lp;
		int found = 0;

		for (lp = ihead; lp != NULL; lp = lp->nextp) {
			if (lp->inum == stbuf.st_ino && lp->devnum == stbuf.st_dev) {
				found++;
				break;
			}
		}
		if (found) {
			strcpy(dblock.dbuf.linkname, lp->pathname);
			dblock.dbuf.linkflag = '1';
			sprintf(dblock.dbuf.chksum, "%6lo", checksum());
			writetape( (char *) &dblock);
			if (vflag) {
				fprintf(stderr, "a %s ", longname);
				fprintf(stderr, "link to %s\n", lp->pathname);
			}
			lp->count--;
			close(infile);
			return;
		}
		else {
			lp = (struct linkbuf *) malloc(sizeof(*lp));
			if (lp == NULL) {
				if (freemem) {
					fprintf(stderr, "Out of memory. Link information lost\n");
					freemem = 0;
				}
			}
			else {
				lp->nextp = ihead;
				ihead = lp;
				lp->inum = stbuf.st_ino;
				lp->devnum = stbuf.st_dev;
				lp->count = stbuf.st_nlink - 1;
				strcpy(lp->pathname, longname);
			}
		}
	}

	blocks = (stbuf.st_size + (TBLOCK-1)) / TBLOCK;
	if (vflag) {
		fprintf(stderr, "a %s ", longname);
		fprintf(stderr, "%ld blocks\n", blocks);
	}
	sprintf(dblock.dbuf.chksum, "%6lo", checksum());
	writetape( (char *) &dblock);

	while ((i = read(infile, buf, TBLOCK)) > 0 && blocks > 0) {
		writetape(buf);
		blocks--;
	}
	close(infile);
	if (blocks != 0 || i != 0)
		fprintf(stderr, "%s: file changed size\n", longname);
	while (blocks-- >  0)
		putempty();
}



static void
doxtract(char *argv[])
{
	long blocks, bytes;
	char buf[TBLOCK];
	char **cp;
	int ofile;

	for (;;) {
		getdir();
		if (endtape())
			break;

		if (*argv == 0)
			goto gotit;

		for (cp = argv; *cp; cp++)
			if (prefix(*cp, dblock.dbuf.name))
				goto gotit;
		passtape();
		continue;

gotit:
		if (checkw('x', dblock.dbuf.name) == 0) {
			passtape();
			continue;
		}

		checkdir(dblock.dbuf.name);

		if (dblock.dbuf.linkflag == '1') {
			unlink(dblock.dbuf.name);
			if (link(dblock.dbuf.linkname, dblock.dbuf.name) < 0) {
				fprintf(stderr, "%s: cannot link\n", dblock.dbuf.name);
				continue;
			}
			if (vflag)
				fprintf(stderr, "%s linked to %s\n", dblock.dbuf.name, dblock.dbuf.linkname);
			continue;
		}
		if ((ofile = creat(dblock.dbuf.name, stbuf.st_mode & 07777)) < 0) {
			fprintf(stderr, "tar: %s - cannot create\n", dblock.dbuf.name);
			passtape();
			continue;
		}

		chown(dblock.dbuf.name, stbuf.st_uid, stbuf.st_gid);

		blocks = ((bytes = stbuf.st_size) + TBLOCK-1)/TBLOCK;
		if (vflag)
			fprintf(stderr, "x %s, %ld bytes, %ld tape blocks\n", dblock.dbuf.name, bytes, blocks);
		while (blocks-- > 0) {
			readtape(buf);
			if (bytes > TBLOCK) {
				if (write(ofile, buf, TBLOCK) < 0) {
					fprintf(stderr, "tar: %s: HELP - extract write error\n", dblock.dbuf.name);
					done(2);
				}
			} else
				if (write(ofile, buf, (int) bytes) < 0) {
					fprintf(stderr, "tar: %s: HELP - extract write error\n", dblock.dbuf.name);
					done(2);
				}
			bytes -= TBLOCK;
		}
		close(ofile);
		if (mflag == 0) {
			time_t timep[2];

			timep[0] = time(NULL);
			timep[1] = stbuf.st_mtime;
			utime(dblock.dbuf.name, timep);
		}
	}
}

static void
dotable(void)
{
	for (;;) {
		getdir();
		if (endtape())
			break;
		if (vflag)
			longt(&stbuf);
		printf("%s", dblock.dbuf.name);
		if (dblock.dbuf.linkflag == '1')
			printf(" linked to %s", dblock.dbuf.linkname);
		printf("\n");
		passtape();
	}
}

static void
putempty(void)
{
	char buf[TBLOCK];
	char *cp;

	for (cp = buf; cp < &buf[TBLOCK]; )
		*cp++ = '\0';
	writetape(buf);
}

static void
longt(register struct stat *st)
{
	register char *cp;

	pmode(st);
	printf("%3d/%1d", st->st_uid, st->st_gid);
	printf("%7D", st->st_size);
	cp = ctime(&st->st_mtime);
	printf(" %-12.12s %-4.4s ", cp+4, cp+20);
}

#define	SUID	04000
#define	SGID	02000
#define	ROWN	0400
#define	WOWN	0200
#define	XOWN	0100
#define	RGRP	040
#define	WGRP	020
#define	XGRP	010
#define	ROTH	04
#define	WOTH	02
#define	XOTH	01
#define	STXT	01000
i32	m1[] = { 1, ROWN, 'r', '-' };
i32	m2[] = { 1, WOWN, 'w', '-' };
i32	m3[] = { 2, SUID, 's', XOWN, 'x', '-' };
i32	m4[] = { 1, RGRP, 'r', '-' };
i32	m5[] = { 1, WGRP, 'w', '-' };
i32	m6[] = { 2, SGID, 's', XGRP, 'x', '-' };
i32	m7[] = { 1, ROTH, 'r', '-' };
i32	m8[] = { 1, WOTH, 'w', '-' };
i32	m9[] = { 2, STXT, 't', XOTH, 'x', '-' };

i32	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

static void
pmode(register struct stat *st)
{
	register i32 **mp;

	for (mp = &m[0]; mp < &m[9];)
		select(*mp++, st);
}

static void
select(i32 *pairp, struct stat *st)
{
	register i32 n, *ap;

	ap = pairp;
	n = *ap++;
	while (--n>=0 && (st->st_mode&*ap++)==0)
		ap++;
	printf("%c", *ap);
}

static void
checkdir(register char *name)
{
	register char *cp;
	int i;
	for (cp = name; *cp; cp++) {
		if (*cp == '/') {
			*cp = '\0';
			if (access(name, 01) < 0) {
				if (fork() == 0) {
					execl("/bin/mkdir", "mkdir", name, (char *)0);
					execl("/usr/bin/mkdir", "mkdir", name, (char *)0);
					fprintf(stderr, "tar: cannot find mkdir!\n");
					done(0);
				}
					while (wait((i16 *)&i) >= 0);
				chown(name, stbuf.st_uid, stbuf.st_gid);
			}
			*cp = '/';
		}
	}
}

static void
onintr(void)
{
	signal(SIGINT, SIG_IGN);
	term++;
}

static void
onquit(void)
{
	signal(SIGQUIT, SIG_IGN);
	term++;
}

static void
onhup(void)
{
	signal(SIGHUP, SIG_IGN);
	term++;
}

static void
onterm(void)
{
	signal(SIGTERM, SIG_IGN);
	term++;
}

static void
tomodes(register struct stat *sp)
{
	register char *cp;

	for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
		*cp = '\0';
	sprintf(dblock.dbuf.mode, "%6o ", sp->st_mode & 07777);
	sprintf(dblock.dbuf.uid, "%6o ", sp->st_uid);
	sprintf(dblock.dbuf.gid, "%6o ", sp->st_gid);
	sprintf(dblock.dbuf.size, "%11lo ", sp->st_size);
	sprintf(dblock.dbuf.mtime, "%11lo ", sp->st_mtime);
}

static i32
checksum(void)
{
	register i;
	register char *cp;

	for (cp = dblock.dbuf.chksum; cp < &dblock.dbuf.chksum[sizeof(dblock.dbuf.chksum)]; cp++)
		*cp = ' ';
	i = 0;
	for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
		i += *cp;
	return(i);
}

static i32
checkw(i32 c, char *name)
{
	if (wflag) {
		printf("%c ", c);
		if (vflag)
			longt(&stbuf);
		printf("%s: ", name);
		if (response() == 'y'){
			return(1);
		}
		return(0);
	}
	return(1);
}

static i32
response(void)
{
	char c;

	c = getchar();
	if (c != '\n')
		while (getchar() != '\n');
	else c = 'n';
	return(c);
}

static i32
checkupdate(char *arg)
{
	char name[100];
	long	mtime;
	daddr_t seekp;

	rewind(tfile);
	for (;;) {
		if ((seekp = lookup(arg)) < 0)
			return(1);
		fseek(tfile, seekp, 0);
		fscanf(tfile, "%s %lo", name, &mtime);
		if (stbuf.st_mtime > mtime)
			return(1);
		else
			return(0);
	}
}

static long
fromoct(cp, n)
register char *cp;
register i32 n;
{
	register long v;

	v = 0;
	while (n > 0 && (*cp == ' ' || *cp == '\0')) {
		cp++;
		n--;
	}
	while (n > 0 && *cp >= '0' && *cp <= '7') {
		v = (v << 3) + *cp++ - '0';
		n--;
	}
	return(v);
}

static void
done(i32 n)
{
	unlink(tname);
	exit(n);
}

static i32
prefix(register char *s1, register char *s2)
{
	while (*s1)
		if (*s1++ != *s2++)
			return(0);
	if (*s2)
		return(*s2 == '/');
	return(1);
}

static void
getwdir(char *s)
{
	char *cp;
	char *pwd_argv[2];
	int i;
	i16	pipdes[2];
	i16	status;
	int	pid;
	int	nread;

	pipe(pipdes);
	if ((pid = fork()) == 0) {
		pwd_argv[0] = "pwd";
		pwd_argv[1] = (char *)0;
		close(1);
		dup(pipdes[1]);
		close(pipdes[0]);
		close(pipdes[1]);
		execv("/bin/pwd", pwd_argv);
		execv("/usr/bin/pwd", pwd_argv);
		fprintf(stderr, "pwd failed!\n");
		printf("/\n");
		exit(1);
	}
	while ((i = wait(&status)) != -1)
		;
	nread = read(pipdes[0], s, 50);
	if (nread > 0) {
		s[nread] = '\0';
		for (cp = s; *cp != '\n' && *cp != '\0'; cp++)
			;
		if (*cp != '\0')
			*cp = '\0';
	}
	if (nread <= 0) {
		s[0] = '/';
		s[1] = '\n';
		s[2] = '\0';
	}
	close(pipdes[0]);
	close(pipdes[1]);
}

#define	N	200
int	njab;
static daddr_t
lookup(char *s)
{
	register i;
	daddr_t a;

	for(i=0; s[i]; i++)
		if(s[i] == ' ')
			break;
	a = bsrch(s, i, low, high);
	return(a);
}

static daddr_t
bsrch(char *s, i32 n, daddr_t l, daddr_t h)
{
	register i, j;
	char b[N];
	daddr_t m, m1;

	njab = 0;

loop:
	if(l >= h)
		return(-1L);
	m = l + (h-l)/2 - N/2;
	if(m < l)
		m = l;
	fseek(tfile, m, 0);
	fread(b, 1, N, tfile);
	njab++;
	for(i=0; i<N; i++) {
		if(b[i] == '\n')
			break;
		m++;
	}
	if(m >= h)
		return(-1L);
	m1 = m;
	j = i;
	for(i++; i<N; i++) {
		m1++;
		if(b[i] == '\n')
			break;
	}
	i = cmp(b+j, s, n);
	if(i < 0) {
		h = m;
		goto loop;
	}
	if(i > 0) {
		l = m1;
		goto loop;
	}
	return(m);
}

static i32
cmp(char *b, char *s, i32 n)
{
	register i;

	if(b[0] != '\n')
		exit(2);
	for(i=0; i<n; i++) {
		if(b[i+1] > s[i])
			return(-1);
		if(b[i+1] < s[i])
			return(1);
	}
	return(b[i+1] == ' '? 0 : -1);
}

static i32
readtape(char *buffer)
{
	int i, j;

	if (recno >= nblock || first == 0) {
		if (first == 0 && nblock == 0)
			j = NBLOCK;
		else
			j = nblock;
		if ((i = read(mt, tbuf, TBLOCK*j)) < 0) {
			fprintf(stderr, "Tar: tape read error\n");
			done(3);
		}
		if (first == 0) {
			if ((i % TBLOCK) != 0) {
				fprintf(stderr, "Tar: tape blocksize error\n");
				done(3);
			}
			i /= TBLOCK;
			if (rflag && i != 1) {
				fprintf(stderr, "Tar: Cannot update blocked tapes (yet)\n");
				done(4);
			}
			if (i != nblock && i != 1) {
				fprintf(stderr, "Tar: blocksize = %d\n", i);
				nblock = i;
			}
		}
		recno = 0;
	}
	first = 1;
	copy(buffer, (char *)&tbuf[recno++]);
	return(TBLOCK);
}

static i32
writetape(char *buffer)
{
	first = 1;
	if (nblock == 0)
		nblock = 1;
	if (recno >= nblock) {
		if (write(mt, tbuf, TBLOCK*nblock) < 0) {
			fprintf(stderr, "Tar: tape write error\n");
			done(2);
		}
		recno = 0;
	}
	copy((char *)&tbuf[recno++], buffer);
	if (recno >= nblock) {
		if (write(mt, tbuf, TBLOCK*nblock) < 0) {
			fprintf(stderr, "Tar: tape write error\n");
			done(2);
		}
		recno = 0;
	}
	return(TBLOCK);
}

static void
backtape(void)
{
	lseek(mt, (long) -TBLOCK, 1);
	if (recno >= nblock) {
		recno = nblock - 1;
		if (read(mt, tbuf, TBLOCK*nblock) < 0) {
			fprintf(stderr, "Tar: tape read error after seek\n");
			done(4);
		}
		lseek(mt, (long) -TBLOCK, 1);
	}
}

static void
flushtape(void)
{
	if (recno > 0)
		write(mt, tbuf, TBLOCK*nblock);
}

static void
copy(register char *to, register char *from)
{
	register i;

	i = TBLOCK;
	do {
		*to++ = *from++;
	} while (--i);
}
