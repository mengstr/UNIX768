/*
 * Editor
 */

#include <signal.h>
#include <sgtty.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define	NULL	0
#define	FNSIZE	64
#define	LBSIZE	512
#define	ESIZE	128
#define	GBSIZE	256
#define	NBRA	5
#define	EOF	-1
#define	KSIZE	9

#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14

#define	STAR	01

char	Q[]	= "";
char	T[]	= "TMP";
#define	READ	0
#define	WRITE	1

int	peekc;
int	lastc;
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	rhsbuf[LBSIZE/2];
char	expbuf[ESIZE+4];
int	circfl;
i32	*zero;
i32	*dot;
i32	*dol;
i32	*addr1;
i32	*addr2;
char	genbuf[LBSIZE];
long	count;
char	*nextip;
char	*linebp;
int	ninbuf;
int	io;
int	pflag;
i32	lseek(i32 fd, i32 off, i32 sbase);
sighandler_t	oldhup;
sighandler_t	oldquit;
int	vflag	= 1;
int	xflag;
int	xtflag;
int	kflag;
char	key[KSIZE + 1];
char	crbuf[512];
char	perm[768];
char	tperm[768];
int	listf;
int	col;
char	*globp;
int	tfile	= -1;
int	tline;
char	*tfname;
char	*loc1;
char	*loc2;
char	*locs;
char	ibuff[512];
int	iblock	= -1;
char	obuff[512];
int	oblock	= -1;
int	ichanged;
int	nleft;
char	WRERR[]	= "WRITE ERROR";
i32	names[26];
int	anymarks;
char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra;
int	subnewa;
int	subolda;
int	fchange;
int	wrapp;
unsigned nlall = 128;

static void	commands(void);
static i32	*address(void);
static void	setdot(void);
static void	setall(void);
static void	setnoaddr(void);
static void	nonzero(void);
static void	newline(void);
static void	filename(i32 comm);
static void	exfile(void);
static i32	onintr(void);
static i32	onhup(void);
static i32	gettty(void);
static i32	getfile(void);
static void	putfile(void);
static i32	append(i32 (*f)(void), i32 *a);
static void	callunix(void);
static i32	quit(void);
static void	delete(void);
static void	rdelete(i32 *ad1, i32 *ad2);
static void	gdelete(void);
static char	*getline(i32 tl);
static i32	putline(void);
static char	*getblock(i32 atl, i32 iof);
static void	blkio(i32 b, void *buf, i32 (*iofcn)(i32 fd, void *buf, i32 nbyte));
static void	init(void);
static void	global(i32 k);
static void	join(void);
static void	substitute(i32 inglob);
static i32	compsub(void);
static i32	getsub(void);
static void	dosub(void);
static char	*place(char *sp, char *l1, char *l2);
static void	move(i32 cflag);
static void	reverse(i32 *a1, i32 *a2);
static i32	getcopy(void);
static void	compile(i32 aeof);
static i32	execute(i32 gf, i32 *addr);
static i32	advance(char *lp, char *ep);
static i32	backref(i32 i, char *lp);
static i32	cclass(char *set, i32 c, i32 af);
static void	putd(void);
static void	ed_puts(char *sp);
char	*mktemp(char *as);
static void	crblock(char *permp, char *buf, i32 nchar, long startn);
static i32	getkey(void);
static i32	crinit(char *keyp, char *permp);
static void	makekey(char *a, char *b);
static void error(char *s);
static i32 getchr(void);
static void putchr(i32 ac);
jmp_buf	savej;

int
main(int argc, char **argv)
{
	register char *p1, *p2;
	sighandler_t oldintr;

	oldquit = signal(SIGQUIT, SIG_IGN);
	oldhup = signal(SIGHUP, SIG_IGN);
	oldintr = signal(SIGINT, SIG_IGN);
	if (signal(SIGTERM, SIG_IGN) == SIG_DFL)
		signal(SIGTERM, (sighandler_t)quit);
	argv++;
	while (argc > 1 && **argv=='-') {
		switch((*argv)[1]) {

		case '\0':
			vflag = 0;
			break;

		case 'q':
			signal(SIGQUIT, SIG_DFL);
			vflag = 1;
			break;

		case 'x':
			xflag = 1;
			break;
		}
		argv++;
		argc--;
	}
	if(xflag){
		getkey();
		kflag = crinit(key, perm);
	}

	if (argc>1) {
		p1 = *argv;
		p2 = savedfile;
		while (*p2++ = *p1++)
			;
		globp = "r";
	}
	zero = (i32 *)malloc(nlall * sizeof(i32));
	tfname = mktemp("/tmp/eXXXXX");
	init();
	if (((u32)oldintr&01) == 0)
		signal(SIGINT, (sighandler_t)onintr);
	if (((u32)oldhup&01) == 0)
		signal(SIGHUP, (sighandler_t)onhup);
	setjmp(savej);
	commands();
	quit();
}

static void
commands(void)
{
	register i32 *a1;
	register i32 c;

	for (;;) {
	if (pflag) {
		pflag = 0;
		addr1 = addr2 = dot;
		goto print;
	}
	addr1 = 0;
	addr2 = 0;
	do {
		addr1 = addr2;
		if ((a1 = address())==0) {
			c = getchr();
			break;
		}
		addr2 = a1;
		if ((c=getchr()) == ';') {
			c = ',';
			dot = a1;
		}
	} while (c==',');
	if (addr1==0)
		addr1 = addr2;
	switch(c) {

	case 'a':
		setdot();
		newline();
		append(gettty, addr2);
		continue;

	case 'c':
		delete();
		append(gettty, addr1-1);
		continue;

	case 'd':
		delete();
		continue;

	case 'E':
		fchange = 0;
		c = 'e';
	case 'e':
		setnoaddr();
		if (vflag && fchange) {
			fchange = 0;
			error(Q);
		}
		filename(c);
		init();
		addr2 = zero;
		goto caseread;

	case 'f':
		setnoaddr();
		filename(c);
		ed_puts(savedfile);
		continue;

	case 'g':
		global(1);
		continue;

	case 'i':
		setdot();
		nonzero();
		newline();
		append(gettty, addr2-1);
		continue;


	case 'j':
		if (addr2==0) {
			addr1 = dot;
			addr2 = dot+1;
		}
		setdot();
		newline();
		nonzero();
		join();
		continue;

	case 'k':
		if ((c = getchr()) < 'a' || c > 'z')
			error(Q);
		newline();
		setdot();
		nonzero();
		names[c-'a'] = *addr2 & ~01;
		anymarks |= 01;
		continue;

	case 'm':
		move(0);
		continue;

	case '\n':
		if (addr2==0)
			addr2 = dot+1;
		addr1 = addr2;
		goto print;

	case 'l':
		listf++;
	case 'p':
	case 'P':
		newline();
	print:
		setdot();
		nonzero();
		a1 = addr1;
		do {
			ed_puts(getline(*a1++));
		} while (a1 <= addr2);
		dot = addr2;
		listf = 0;
		continue;

	case 'Q':
		fchange = 0;
	case 'q':
		setnoaddr();
		newline();
		quit();

	case 'r':
		filename(c);
	caseread:
		if ((io = open(file, 0)) < 0) {
			lastc = '\n';
			error(file);
		}
		setall();
		ninbuf = 0;
		c = zero != dol;
		append(getfile, addr2);
		exfile();
		fchange = c;
		continue;

	case 's':
		setdot();
		nonzero();
		substitute(globp!=0);
		continue;

	case 't':
		move(1);
		continue;

	case 'u':
		setdot();
		nonzero();
		newline();
		if ((*addr2&~01) != subnewa)
			error(Q);
		*addr2 = subolda;
		dot = addr2;
		continue;

	case 'v':
		global(0);
		continue;

	case 'W':
		wrapp++;
	case 'w':
		setall();
		nonzero();
		filename(c);
		if(!wrapp ||
		  ((io = open(file,1)) == -1) ||
		  ((lseek(io, 0L, 2)) == -1))
			if ((io = creat(file, 0666)) < 0)
				error(file);
		wrapp = 0;
		putfile();
		exfile();
		if (addr1==zero+1 && addr2==dol)
			fchange = 0;
		continue;

	case 'x':
		setnoaddr();
		newline();
		xflag = 1;
		ed_puts("Entering encrypting mode!");
		getkey();
		kflag = crinit(key, perm);
		continue;


	case '=':
		setall();
		newline();
		count = (addr2-zero)&077777;
		putd();
		putchr('\n');
		continue;

	case '!':
		callunix();
		continue;

	case EOF:
		return;

	}
	error(Q);
	}
}

static i32 *
address(void)
{
	register i32 *a1;
	register i32 minus, c;
	i32 n, relerr;

	minus = 0;
	a1 = 0;
	for (;;) {
		c = getchr();
		if ('0'<=c && c<='9') {
			n = 0;
			do {
				n *= 10;
				n += c - '0';
			} while ((c = getchr())>='0' && c<='9');
			peekc = c;
			if (a1==0)
				a1 = zero;
			if (minus<0)
				n = -n;
			a1 += n;
			minus = 0;
			continue;
		}
		relerr = 0;
		if (a1 || minus)
			relerr++;
		switch(c) {
		case ' ':
		case '\t':
			continue;
	
		case '+':
			minus++;
			if (a1==0)
				a1 = dot;
			continue;

		case '-':
		case '^':
			minus--;
			if (a1==0)
				a1 = dot;
			continue;
	
		case '?':
		case '/':
			compile(c);
			a1 = dot;
			for (;;) {
				if (c=='/') {
					a1++;
					if (a1 > dol)
						a1 = zero;
				} else {
					a1--;
					if (a1 < zero)
						a1 = dol;
				}
				if (execute(0, a1))
					break;
				if (a1==dot)
					error(Q);
			}
			break;
	
		case '$':
			a1 = dol;
			break;
	
		case '.':
			a1 = dot;
			break;

		case '\'':
			if ((c = getchr()) < 'a' || c > 'z')
				error(Q);
			for (a1=zero; a1<=dol; a1++)
				if (names[c-'a'] == (*a1 & ~01))
					break;
			break;
	
		default:
			peekc = c;
			if (a1==0)
				return(0);
			a1 += minus;
			if (a1<zero || a1>dol)
				error(Q);
			return(a1);
		}
		if (relerr)
			error(Q);
	}
}

static void
setdot(void)
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2)
		error(Q);
}

static void
setall(void)
{
	if (addr2==0) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}

static void
setnoaddr(void)
{
	if (addr2)
		error(Q);
}

static void
nonzero(void)
{
	if (addr1<=zero || addr2>dol)
		error(Q);
}

static void
newline(void)
{
	register c;

	if ((c = getchr()) == '\n')
		return;
	if (c=='p' || c=='l') {
		pflag++;
		if (c=='l')
			listf++;
		if (getchr() == '\n')
			return;
	}
	error(Q);
}

static void
filename(i32 comm)
{
	register char *p1, *p2;
	register i32 c;

	count = 0;
	c = getchr();
	if (c=='\n' || c==EOF) {
		p1 = savedfile;
		if (*p1==0 && comm!='f')
			error(Q);
		p2 = file;
		while (*p2++ = *p1++)
			;
		return;
	}
	if (c!=' ')
		error(Q);
	while ((c = getchr()) == ' ')
		;
	if (c=='\n')
		error(Q);
	p1 = file;
	do {
		*p1++ = c;
		if (c==' ' || c==EOF)
			error(Q);
	} while ((c = getchr()) != '\n');
	*p1++ = 0;
	if (savedfile[0]==0 || comm=='e' || comm=='f') {
		p1 = savedfile;
		p2 = file;
		while (*p1++ = *p2++)
			;
	}
}

static void
exfile(void)
{
	close(io);
	io = -1;
	if (vflag) {
		putd();
		putchr('\n');
	}
}

static i32
onintr(void)
{
	signal(SIGINT, (sighandler_t)onintr);
	putchr('\n');
	lastc = '\n';
	error(Q);
	return(0);
}

static i32
onhup(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	if (dol > zero) {
		addr1 = zero+1;
		addr2 = dol;
		io = creat("ed.hup", 0666);
		if (io > 0)
			putfile();
	}
	fchange = 0;
	quit();
	return(0);
}

static void
error(s)
char *s;
{
	register c;

	wrapp = 0;
	listf = 0;
	putchr('?');
	ed_puts(s);
	count = 0;
	lseek(0, (long)0, 2);
	pflag = 0;
	if (globp)
		lastc = '\n';
	globp = 0;
	peekc = lastc;
	if(lastc)
		while ((c = getchr()) != '\n' && c != EOF)
			;
	if (io > 0) {
		close(io);
		io = -1;
	}
	longjmp(savej, 1);
}

static i32
getchr()
{
	char c;
	if (lastc=peekc) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0)
			return(lastc);
		globp = 0;
		return(EOF);
	}
	if (read(0, &c, 1) <= 0)
		return(lastc = EOF);
	lastc = c&0177;
	return(lastc);
}

static i32
gettty(void)
{
	register c;
	register char *gf;
	register char *p;

	p = linebuf;
	gf = globp;
	while ((c = getchr()) != '\n') {
		if (c==EOF) {
			if (gf)
				peekc = c;
			return(c);
		}
		if ((c &= 0177) == 0)
			continue;
		*p++ = c;
		if (p >= &linebuf[LBSIZE-2])
			error(Q);
	}
	*p++ = 0;
	if (linebuf[0]=='.' && linebuf[1]==0)
		return(EOF);
	return(0);
}

static i32
getfile(void)
{
	register c;
	register char *lp, *fp;

	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0)
				return(EOF);
			fp = genbuf;
			while(fp < &genbuf[ninbuf]) {
				if (*fp++ & 0200) {
					if (kflag)
						crblock(perm, genbuf, ninbuf+1, count);
					break;
				}
			}
			fp = genbuf;
		}
		c = *fp++;
		if (c=='\0')
			continue;
		if (c&0200 || lp >= &linebuf[LBSIZE]) {
			lastc = '\n';
			error(Q);
		}
		*lp++ = c;
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}

static void
putfile(void)
{
	i32 *a1;
	i32 n;
	register char *fp, *lp;
	register nib;

	nib = 512;
	fp = genbuf;
	a1 = addr1;
	do {
		lp = getline(*a1++);
		for (;;) {
			if (--nib < 0) {
				n = fp-genbuf;
				if(kflag)
					crblock(perm, genbuf, n, count-n);
				if(write(io, genbuf, n) != n) {
					ed_puts(WRERR);
					error(Q);
				}
				nib = 511;
				fp = genbuf;
			}
			count++;
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	} while (a1 <= addr2);
	n = fp-genbuf;
	if(kflag)
		crblock(perm, genbuf, n, count-n);
	if(write(io, genbuf, n) != n) {
		ed_puts(WRERR);
		error(Q);
	}
}

static i32
append(i32 (*f)(void), i32 *a)
{
	register i32 *a1, *a2, *rdot;
	i32 nline, tl;

	nline = 0;
	dot = a;
	while ((*f)() == 0) {
		if ((dol-zero)+1 >= nlall) {
			i32 *ozero = zero;
			nlall += 512;
			if ((zero = (i32 *)realloc((char *)zero, nlall * sizeof(i32))) == NULL) {
				lastc = '\n';
				zero = ozero;
				error("MEM?");
			}
			dot += zero - ozero;
			dol += zero - ozero;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot)
			*--a2 = *--a1;
		*rdot = tl;
	}
	return(nline);
}

static void
callunix(void)
{
	sighandler_t savint;
	register i32 pid, rpid;
	i16 retcode;

	setnoaddr();
	if ((pid = fork()) == 0) {
		signal(SIGHUP, oldhup);
		signal(SIGQUIT, oldquit);
	execl("/bin/sh", "sh", "-t", (char *)0);
		exit(0100);
	}
	savint = signal(SIGINT, SIG_IGN);
	while ((rpid = wait(&retcode)) != pid && rpid != -1)
		;
	signal(SIGINT, savint);
	ed_puts("!");
}

static i32
quit(void)
{
	if (vflag && fchange && dol!=zero) {
		fchange = 0;
		error(Q);
	}
	unlink(tfname);
	exit(0);
	return(0);
}

static void
delete(void)
{
	setdot();
	newline();
	nonzero();
	rdelete(addr1, addr2);
}

static void
rdelete(i32 *ad1, i32 *ad2)
{
	register i32 *a1, *a2, *a3;

	a1 = ad1;
	a2 = ad2+1;
	a3 = dol;
	dol -= a2 - a1;
	do {
		*a1++ = *a2++;
	} while (a2 <= a3);
	a1 = ad1;
	if (a1 > dol)
		a1 = dol;
	dot = a1;
	fchange = 1;
}

static void
gdelete(void)
{
	register i32 *a1, *a2, *a3;

	a3 = dol;
	for (a1=zero+1; (*a1&01)==0; a1++)
		if (a1>=a3)
			return;
	for (a2=a1+1; a2<=a3;) {
		if (*a2&01) {
			a2++;
			dot = a1;
		} else
			*a1++ = *a2++;
	}
	dol = a1-1;
	if (dot>dol)
		dot = dol;
	fchange = 1;
}

static char *
getline(i32 tl)
{
	register char *bp, *lp;
	register nl;

	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~0377;
	while (*lp++ = *bp++)
		if (--nl == 0) {
			bp = getblock(tl+=0400, READ);
			nl = nleft;
		}
	return(linebuf);
}

static i32
putline(void)
{
	register char *bp, *lp;
	register nl;
	int tl;

	fchange = 1;
	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~0377;
	while (*bp = *lp++) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl+=0400, WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}

static char *
getblock(i32 atl, i32 iof)
{
	register bno, off;
	register char *p1, *p2;
	register int n;
	
	bno = (atl>>8)&0377;
	off = (atl<<1)&0774;
	if (bno >= 255) {
		lastc = '\n';
		error(T);
	}
	nleft = 512 - off;
	if (bno==iblock) {
		ichanged |= iof;
		return(ibuff+off);
	}
	if (bno==oblock)
		return(obuff+off);
	if (iof==READ) {
		if (ichanged) {
			if(xtflag)
				crblock(tperm, ibuff, 512, (long)0);
			blkio(iblock, ibuff, write);
		}
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		if(xtflag)
			crblock(tperm, ibuff, 512, (long)0);
		return(ibuff+off);
	}
	if (oblock>=0) {
		if(xtflag) {
			p1 = obuff;
			p2 = crbuf;
			n = 512;
			while(n--)
				*p2++ = *p1++;
			crblock(tperm, crbuf, 512, (long)0);
			blkio(oblock, crbuf, write);
		} else
			blkio(oblock, obuff, write);
	}
	oblock = bno;
	return(obuff+off);
}

static void
blkio(i32 b, void *buf, i32 (*iofcn)(i32 fd, void *buf, i32 nbyte))
{
	lseek(tfile, (long)b<<9, 0);
	if ((*iofcn)(tfile, buf, 512) != 512) {
		error(T);
	}
}

static void
init(void)
{
	register i32 *markp;

	close(tfile);
	tline = 2;
	for (markp = names; markp < &names[26]; )
		*markp++ = 0;
	subnewa = 0;
	anymarks = 0;
	iblock = -1;
	oblock = -1;
	ichanged = 0;
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	if(xflag) {
		xtflag = 1;
		makekey(key, tperm);
	}
	dot = dol = zero;
}

static void
global(i32 k)
{
	register char *gp;
	register c;
	register i32 *a1;
	char globuf[GBSIZE];

	if (globp)
		error(Q);
	setall();
	nonzero();
	if ((c=getchr())=='\n')
		error(Q);
	compile(c);
	gp = globuf;
	while ((c = getchr()) != '\n') {
		if (c==EOF)
			error(Q);
		if (c=='\\') {
			c = getchr();
			if (c!='\n')
				*gp++ = '\\';
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE-2])
			error(Q);
	}
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(0, a1)==k)
			*a1 |= 01;
	}
	/*
	 * Special case: g/.../d (avoid n^2 algorithm)
	 */
	if (globuf[0]=='d' && globuf[1]=='\n' && globuf[2]=='\0') {
		gdelete();
		return;
	}
	for (a1=zero; a1<=dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands();
			a1 = zero;
		}
	}
}

static void
join(void)
{
	register char *gp, *lp;
	register i32 *a1;

	gp = genbuf;
	for (a1=addr1; a1<=addr2; a1++) {
		lp = getline(*a1);
		while (*gp = *lp++)
			if (gp++ >= &genbuf[LBSIZE-2])
				error(Q);
	}
	lp = linebuf;
	gp = genbuf;
	while (*lp++ = *gp++)
		;
	*addr1 = putline();
	if (addr1<addr2)
		rdelete(addr1+1, addr2);
	dot = addr1;
}

static void
substitute(i32 inglob)
{
	register i32 *markp, *a1, nl;
	int gsubf;

	gsubf = compsub();
	for (a1 = addr1; a1 <= addr2; a1++) {
		i32 *ozero;
		if (execute(0, a1)==0)
			continue;
		inglob |= 01;
		dosub();
		if (gsubf) {
			while (*loc2) {
				if (execute(1, (i32 *)0)==0)
					break;
				dosub();
			}
		}
		subnewa = putline();
		*a1 &= ~01;
		if (anymarks) {
			for (markp = names; markp < &names[26]; markp++)
				if (*markp == *a1)
					*markp = subnewa;
		}
		subolda = *a1;
		*a1 = subnewa;
		ozero = zero;
		nl = append(getsub, a1);
		nl += zero-ozero;
		a1 += nl;
		addr2 += nl;
	}
	if (inglob==0)
		error(Q);
}

static i32
compsub(void)
{
	register seof, c;
	register char *p;

	if ((seof = getchr()) == '\n' || seof == ' ')
		error(Q);
	compile(seof);
	p = rhsbuf;
	for (;;) {
		c = getchr();
		if (c=='\\')
			c = getchr() | 0200;
		if (c=='\n') {
			if (globp)
				c |= 0200;
			else
				error(Q);
		}
		if (c==seof)
			break;
		*p++ = c;
		if (p >= &rhsbuf[LBSIZE/2])
			error(Q);
	}
	*p++ = 0;
	if ((peekc = getchr()) == 'g') {
		peekc = 0;
		newline();
		return(1);
	}
	newline();
	return(0);
}

static i32
getsub(void)
{
	register char *p1, *p2;

	p1 = linebuf;
	if ((p2 = linebp) == 0)
		return(EOF);
	while (*p1++ = *p2++)
		;
	linebp = 0;
	return(0);
}

static void
dosub(void)
{
	register char *lp, *sp, *rp;
	int c;

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
	while (lp < loc1)
		*sp++ = *lp++;
	while (c = *rp++&0377) {
		if (c=='&') {
			sp = place(sp, loc1, loc2);
			continue;
		} else if (c&0200 && (c &= 0177) >='1' && c < nbra+'1') {
			sp = place(sp, braslist[c-'1'], braelist[c-'1']);
			continue;
		}
		*sp++ = c&0177;
		if (sp >= &genbuf[LBSIZE])
			error(Q);
	}
	lp = loc2;
	loc2 = sp - genbuf + linebuf;
	while (*sp++ = *lp++)
		if (sp >= &genbuf[LBSIZE])
			error(Q);
	lp = linebuf;
	sp = genbuf;
	while (*lp++ = *sp++)
		;
}

static char *
place(register char *sp, register char *l1, register char *l2)
{

	while (l1 < l2) {
		*sp++ = *l1++;
		if (sp >= &genbuf[LBSIZE])
			error(Q);
	}
	return(sp);
}

static void
move(i32 cflag)
{
	register i32 *adt, *ad1, *ad2;

	setdot();
	nonzero();
	if ((adt = address())==0)
		error(Q);
	newline();
	if (cflag) {
		i32 *ozero, delta;
		ad1 = dol;
		ozero = zero;
		append(getcopy, ad1++);
		ad2 = dol;
		delta = zero - ozero;
		ad1 += delta;
		adt += delta;
	} else {
		ad2 = addr2;
		for (ad1 = addr1; ad1 <= ad2;)
			*ad1++ &= ~01;
		ad1 = addr1;
	}
	ad2++;
	if (adt<ad1) {
		dot = adt + (ad2-ad1);
		if ((++adt)==ad1)
			return;
		reverse(adt, ad1);
		reverse(ad1, ad2);
		reverse(adt, ad2);
	} else if (adt >= ad2) {
		dot = adt++;
		reverse(ad1, ad2);
		reverse(ad2, adt);
		reverse(ad1, adt);
	} else
		error(Q);
	fchange = 1;
}

static void
reverse(register i32 *a1, register i32 *a2)
{
	register i32 t;

	for (;;) {
		t = *--a2;
		if (a2 <= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}

static i32
getcopy(void)
{
	if (addr1 > addr2)
		return(EOF);
	getline(*addr1++);
	return(0);
}

static void
compile(i32 aeof)
{
	register eof, c;
	register char *ep;
	char *lastep;
	char bracket[NBRA], *bracketp;
	int cclcnt;

	ep = expbuf;
	eof = aeof;
	bracketp = bracket;
	if ((c = getchr()) == eof) {
		if (*ep==0)
			error(Q);
		return;
	}
	circfl = 0;
	nbra = 0;
	if (c=='^') {
		c = getchr();
		circfl++;
	}
	peekc = c;
	lastep = 0;
	for (;;) {
		if (ep >= &expbuf[ESIZE])
			goto cerror;
		c = getchr();
		if (c==eof) {
			if (bracketp != bracket)
				goto cerror;
			*ep++ = CEOF;
			return;
		}
		if (c!='*')
			lastep = ep;
		switch (c) {

		case '\\':
			if ((c = getchr())=='(') {
				if (nbra >= NBRA)
					goto cerror;
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			}
			if (c == ')') {
				if (bracketp <= bracket)
					goto cerror;
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			if (c>='1' && c<'1'+NBRA) {
				*ep++ = CBACK;
				*ep++ = c-'1';
				continue;
			}
			*ep++ = CCHR;
			if (c=='\n')
				goto cerror;
			*ep++ = c;
			continue;

		case '.':
			*ep++ = CDOT;
			continue;

		case '\n':
			goto cerror;

		case '*':
			if (lastep==0 || *lastep==CBRA || *lastep==CKET)
				goto defchar;
			*lastep |= STAR;
			continue;

		case '$':
			if ((peekc=getchr()) != eof)
				goto defchar;
			*ep++ = CDOL;
			continue;

		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c=getchr()) == '^') {
				c = getchr();
				ep[-2] = NCCL;
			}
			do {
				if (c=='\n')
					goto cerror;
				if (c=='-' && ep[-1]!=0) {
					if ((c=getchr())==']') {
						*ep++ = '-';
						cclcnt++;
						break;
					}
					while (ep[-1]<c) {
						*ep = ep[-1]+1;
						ep++;
						cclcnt++;
						if (ep>=&expbuf[ESIZE])
							goto cerror;
					}
				}
				*ep++ = c;
				cclcnt++;
				if (ep >= &expbuf[ESIZE])
					goto cerror;
			} while ((c = getchr()) != ']');
			lastep[1] = cclcnt;
			continue;

		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
   cerror:
	expbuf[0] = 0;
	nbra = 0;
	error(Q);
}

static i32
execute(i32 gf, i32 *addr)
{
	register char *p1, *p2, c;

	for (c=0; c<NBRA; c++) {
		braslist[c] = 0;
		braelist[c] = 0;
	}
	if (gf) {
		if (circfl)
			return(0);
		p1 = linebuf;
		p2 = genbuf;
		while (*p1++ = *p2++)
			;
		locs = p1 = loc2;
	} else {
		if (addr==zero)
			return(0);
		p1 = getline(*addr);
		locs = 0;
	}
	p2 = expbuf;
	if (circfl) {
		loc1 = p1;
		return(advance(p1, p2));
	}
	/* fast check for first character */
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1!=c)
				continue;
			if (advance(p1, p2)) {
				loc1 = p1;
				return(1);
			}
		} while (*p1++);
		return(0);
	}
	/* regular algorithm */
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return(1);
		}
	} while (*p1++);
	return(0);
}

static i32
advance(register char *lp, register char *ep)
{
	register char *curlp;
	i32 i;

	for (;;) switch (*ep++) {

	case CCHR:
		if (*ep++ == *lp++)
			continue;
		return(0);

	case CDOT:
		if (*lp++)
			continue;
		return(0);

	case CDOL:
		if (*lp==0)
			continue;
		return(0);

	case CEOF:
		loc2 = lp;
		return(1);

	case CCL:
		if (cclass(ep, *lp++, 1)) {
			ep += *ep;
			continue;
		}
		return(0);

	case NCCL:
		if (cclass(ep, *lp++, 0)) {
			ep += *ep;
			continue;
		}
		return(0);

	case CBRA:
		braslist[*ep++] = lp;
		continue;

	case CKET:
		braelist[*ep++] = lp;
		continue;

	case CBACK:
		if (braelist[i = *ep++]==0)
			error(Q);
		if (backref(i, lp)) {
			lp += braelist[i] - braslist[i];
			continue;
		}
		return(0);

	case CBACK|STAR:
		if (braelist[i = *ep++] == 0)
			error(Q);
		curlp = lp;
		while (backref(i, lp))
			lp += braelist[i] - braslist[i];
		while (lp >= curlp) {
			if (advance(lp, ep))
				return(1);
			lp -= braelist[i] - braslist[i];
		}
		continue;

	case CDOT|STAR:
		curlp = lp;
		while (*lp++)
			;
		goto star;

	case CCHR|STAR:
		curlp = lp;
		while (*lp++ == *ep)
			;
		ep++;
		goto star;

	case CCL|STAR:
	case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)))
			;
		ep += *ep;
		goto star;

	star:
		do {
			lp--;
			if (lp==locs)
				break;
			if (advance(lp, ep))
				return(1);
		} while (lp > curlp);
		return(0);

	default:
		error(Q);
	}
}

static i32
backref(register i32 i, register char *lp)
{
	register char *bp;

	bp = braslist[i];
	while (*bp++ == *lp++)
		if (bp >= braelist[i])
			return(1);
	return(0);
}

static i32
cclass(register char *set, register i32 c, i32 af)
{
	register n;

	if (c==0)
		return(0);
	n = *set++;
	while (--n)
		if (*set++ == c)
			return(af);
	return(!af);
}

static void
putd(void)
{
	register r;

	r = count%10;
	count /= 10;
	if (count)
		putd();
	putchr(r + '0');
}

static void
ed_puts(register char *sp)
{
	col = 0;
	while (*sp)
		putchr(*sp++);
	putchr('\n');
}

char	line[70];
char	*linp	= line;

static void
putchr(ac)
i32 ac;
{
	register char *lp;
	register c;

	lp = linp;
	c = ac;
	if (listf) {
		col++;
		if (col >= 72) {
			col = 0;
			*lp++ = '\\';
			*lp++ = '\n';
		}
		if (c=='\t') {
			c = '>';
			goto esc;
		}
		if (c=='\b') {
			c = '<';
		esc:
			*lp++ = '-';
			*lp++ = '\b';
			*lp++ = c;
			goto out;
		}
		if (c<' ' && c!= '\n') {
			*lp++ = '\\';
			*lp++ = (c>>3)+'0';
			*lp++ = (c&07)+'0';
			col += 2;
			goto out;
		}
	}
	*lp++ = c;
out:
	if(c == '\n' || lp >= &line[64]) {
		linp = line;
		write(1, line, lp-line);
		return;
	}
	linp = lp;
}
static void
crblock(char *permp, char *buf, i32 nchar, long startn)
{
	register char *p1;
	int n1;
	int n2;
	register char *t1, *t2, *t3;

	t1 = permp;
	t2 = &permp[256];
	t3 = &permp[512];

	n1 = startn&0377;
	n2 = (startn>>8)&0377;
	p1 = buf;
	while(nchar--) {
		*p1 = t2[(t3[(t1[(*p1+n1)&0377]+n2)&0377]-n2)&0377]-n1;
		n1++;
		if(n1==256){
			n1 = 0;
			n2++;
			if(n2==256) n2 = 0;
		}
		p1++;
	}
}

static i32
getkey(void)
{
	struct sgttyb b;
	int save;
	sighandler_t sig;
	register char *p;
	register c;

	sig = signal(SIGINT, SIG_IGN);
	if (gtty(0, &b) == -1)
		error("Input not tty");
	save = b.sg_flags;
	b.sg_flags &= ~ECHO;
	stty(0, &b);
	ed_puts("Key:");
	p = key;
	while(((c=getchr()) != EOF) && (c!='\n')) {
		if(p < &key[KSIZE])
			*p++ = c;
	}
	*p = 0;
	b.sg_flags = save;
	stty(0, &b);
	signal(SIGINT, sig);
	return(key[0] != 0);
}

/*
 * Besides initializing the encryption machine, this routine
 * returns 0 if the key is null, and 1 if it is non-null.
 */
static i32
crinit(char *keyp, char *permp)
{
	register char *t1, *t2, *t3;
	register i;
	int ic, k, temp;
	i16 pf[2];
	unsigned random;
	char buf[13];
	long seed;

	t1 = permp;
	t2 = &permp[256];
	t3 = &permp[512];
	if(*keyp == 0)
		return(0);
	strncpy(buf, keyp, 8);
	while (*keyp)
		*keyp++ = '\0';
	buf[8] = buf[0];
	buf[9] = buf[1];
	if (pipe(pf)<0)
		pf[0] = pf[1] = -1;
	if (fork()==0) {
		close(0);
		close(1);
		dup(pf[0]);
		dup(pf[1]);
	execl("/usr/lib/makekey", "-", (char *)0);
	execl("/lib/makekey", "-", (char *)0);
		exit(1);
	}
	write(pf[1], buf, 10);
	if (wait((i16 *)NULL)==-1 || read(pf[0], buf, 13)!=13)
		error("crypt: cannot generate key");
	close(pf[0]);
	close(pf[1]);
	seed = 123;
	for (i=0; i<13; i++)
		seed = seed*buf[i] + i;
	for(i=0;i<256;i++){
		t1[i] = i;
		t3[i] = 0;
	}
	for(i=0; i<256; i++) {
		seed = 5*seed + buf[i%13];
		random = seed % 65521;
		k = 256-1 - i;
		ic = (random&0377) % (k+1);
		random >>= 8;
		temp = t1[k];
		t1[k] = t1[ic];
		t1[ic] = temp;
		if(t3[k]!=0) continue;
		ic = (random&0377) % k;
		while(t3[ic]!=0) ic = (ic+1) % k;
		t3[k] = ic;
		t3[ic] = k;
	}
	for(i=0; i<256; i++)
		t2[t1[i]&0377] = i;
	return(1);
}

static void
makekey(char *a, char *b)
{
	register int i;
	long t;
	char temp[KSIZE + 1];

	for(i = 0; i < KSIZE; i++)
		temp[i] = *a++;
	time(&t);
	t += getpid();
	for(i = 0; i < 4; i++)
		temp[i] ^= (t>>(8*i))&0377;
	crinit(temp, b);
}
