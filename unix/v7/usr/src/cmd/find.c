/*	find	COMPILE:	cc -o find -s -O -i find.c -lS	*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdlib.h>
#define A_DAY	86400L /* a day full of seconds */
#define EQ(x, y)	(strcmp(x, y)==0)

int	Randlast;
char	Pathname[200];

struct anode {
	int (*F)(struct anode *);
	struct anode *L, *R;
} Node[100];
int Nn;  /* number of nodes */
char	*Fname;
long	Now;
int	Argc,
	Ai,
	Pi;
char	**Argv;
/* cpio stuff */
int	Cpio;
short	*Buf, *Dbuf, *Wp;
int	Bufsize = 5120;
int	Wct = 2560;

long	Newer;

struct stat Statb;

struct anode *exp(void);
struct anode *e1(void);
struct anode *e2(void);
struct anode *e3(void);
struct anode *mk(int (*f)(struct anode *), struct anode *l, struct anode *r);
char *nxtarg(void);
char	Home[128];
long	Blocks;
int and(register struct anode *p);
int or(register struct anode *p);
int not(register struct anode *p);
int glob(register struct anode *p);
int print(void);
int mtime(register struct anode *p);
int atime(register struct anode *p);
int find_ctime(register struct anode *p);
int user(register struct anode *p);
int ino(register struct anode *p);
int group(register struct anode *p);
int links(register struct anode *p);
int size(register struct anode *p);
int perm(register struct anode *p);
int type(register struct anode *p);
int exeq(register struct anode *p);
int ok(struct anode *p);
long mklong(short v[]);
int cpio(void);
int newer(void);
int scomp(int a, int b, int s);
int doex(i32 com);
int getunum(char *f, char *s);
int descend(char *name, char *fname, struct anode *exlist);
int gmatch(register char *s, register char *p);
int amatch(register char *s, register char *p);
int umatch(register char *s, register char *p);
int bwrite(register short *rp, register int c);
int chgreel(int x, int fl);
int pr(char *s);
int
main (int argc, char *argv[])
{
	struct anode *exlist;
	int paths;
	register char *cp, *sp = 0;
	FILE *pwd;

	time(&Now);
	pwd = popen("pwd", "r");
	fgets(Home, 128, pwd);
	pclose(pwd);
	Home[strlen(Home) - 1] = '\0';
	Argc = argc; Argv = argv;
	if(argc<3) {
usage:		pr("Usage: find path-list predicate-list\n");
		exit(1);
	}
	for(Ai = paths = 1; Ai < (argc-1); ++Ai, ++paths)
		if(*Argv[Ai] == '-' || EQ(Argv[Ai], "(") || EQ(Argv[Ai], "!"))
			break;
	if(paths == 1) /* no path-list */
		goto usage;
	if(!(exlist = exp())) { /* parse and compile the arguments */
		pr("find: parsing error\n");
		exit(1);
	}
	if(Ai<argc) {
		pr("find: missing conjunction\n");
		exit(1);
	}
	for(Pi = 1; Pi < paths; ++Pi) {
		sp = 0;
		chdir(Home);
		strcpy(Pathname, Argv[Pi]);
		if(cp = rindex(Pathname, '/')) {
			sp = cp + 1;
			*cp = '\0';
			if(chdir(*Pathname? Pathname: "/") == -1) {
				pr("find: bad starting directory\n");
				exit(2);
			}
			*cp = '/';
		}
		Fname = sp? sp: Pathname;
		descend(Pathname, Fname, exlist); /* to find files that match  */
	}
	if(Cpio) {
		strcpy(Pathname, "TRAILER!!!");
		Statb.st_size = 0;
		cpio();
	}
	exit(0);
}

/* compile time functions:  priority is  exp()<e1()<e2()<e3()  */

struct anode *
exp (void) { /* parse ALTERNATION (-o)  */
	register struct anode * p1;

	p1 = e1() /* get left operand */ ;
	if(Ai >= Argc)
		return(p1);
	if(EQ(nxtarg(), "-o")) {
		Randlast--;
		return(mk((int (*)(struct anode *))or, p1, exp()));
	}
	else if(Ai <= Argc) --Ai;
	return(p1);
}
struct anode *
e1 (void) { /* parse CONCATENATION (formerly -a) */
	register struct anode * p1;
	register char *a;

	p1 = e2();
	if(Ai >= Argc)
		return(p1);
	a = nxtarg();
	if(EQ(a, "-a")) {
And:
		Randlast--;
		return(mk((int (*)(struct anode *))and, p1, e1()));
	} else if(EQ(a, "(") || EQ(a, "!") || (*a=='-' && !EQ(a, "-o"))) {
		--Ai;
		goto And;
	} else if(Ai <= Argc) --Ai;
	return(p1);
}
struct anode *
e2 (void) { /* parse NOT (!) */
	if(Randlast) {
		pr("find: operand follows operand\n");
		exit(1);
	}
	Randlast++;
	if(EQ(nxtarg(), "!"))
		return(mk((int (*)(struct anode *))not, e3(), (struct anode *)0));
	else if(Ai <= Argc) --Ai;
	return(e3());
}
struct anode *
e3 (void) { /* parse parens and predicates */
	struct anode *p1;
	int i;
	register char *a, *b, s;

	a = nxtarg();
	if(EQ(a, "(")) {
		Randlast--;
		p1 = exp();
		a = nxtarg();
		if(!EQ(a, ")")) goto err;
		return(p1);
	}
	else if(EQ(a, "-print")) {
		return(mk((int (*)(struct anode *))print, (struct anode *)0, (struct anode *)0));
	}
	b = nxtarg();
	s = *b;
	if(s=='+') b++;
	if(EQ(a, "-name"))
		return(mk((int (*)(struct anode *))glob, (struct anode *)b, (struct anode *)0));
	else if(EQ(a, "-mtime"))
		return(mk((int (*)(struct anode *))mtime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-atime"))
		return(mk((int (*)(struct anode *))atime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-ctime"))
		return(mk((int (*)(struct anode *))find_ctime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-user")) {
		if((i=getunum("/etc/passwd", b)) == -1) {
			if(gmatch(b, "[0-9][0-9][0-9]*")
			|| gmatch(b, "[0-9][0-9]")
			|| gmatch(b, "[0-9]"))
				return mk((int (*)(struct anode *))user, (struct anode *)atoi(b), (struct anode *)s);
			pr("find: cannot find -user name\n");
			exit(1);
		}
		return(mk((int (*)(struct anode *))user, (struct anode *)i, (struct anode *)s));
	}
	else if(EQ(a, "-inum"))
		return(mk((int (*)(struct anode *))ino, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-group")) {
		if((i=getunum("/etc/group", b)) == -1) {
			if(gmatch(b, "[0-9][0-9][0-9]*")
			|| gmatch(b, "[0-9][0-9]")
			|| gmatch(b, "[0-9]"))
				return mk((int (*)(struct anode *))group, (struct anode *)atoi(b), (struct anode *)s);
			pr("find: cannot find -group name\n");
			exit(1);
		}
		return(mk((int (*)(struct anode *))group, (struct anode *)i, (struct anode *)s));
	} else if(EQ(a, "-size"))
		return(mk((int (*)(struct anode *))size, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-links"))
		return(mk((int (*)(struct anode *))links, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-perm")) {
		for(i=0; *b ; ++b) {
			if(*b=='-') continue;
			i <<= 3;
			i = i + (*b - '0');
		}
		return(mk((int (*)(struct anode *))perm, (struct anode *)i, (struct anode *)s));
	}
	else if(EQ(a, "-type")) {
		i = s=='d' ? S_IFDIR :
		    s=='b' ? S_IFBLK :
		    s=='c' ? S_IFCHR :
		    s=='f' ? 0100000 :
		    0;
		return(mk((int (*)(struct anode *))type, (struct anode *)i, (struct anode *)0));
	}
	else if (EQ(a, "-exec")) {
		i = Ai - 1;
		while(!EQ(nxtarg(), ";"));
		return(mk((int (*)(struct anode *))exeq, (struct anode *)i, (struct anode *)0));
	}
	else if (EQ(a, "-ok")) {
		i = Ai - 1;
		while(!EQ(nxtarg(), ";"));
		return(mk((int (*)(struct anode *))ok, (struct anode *)i, (struct anode *)0));
	}
	else if(EQ(a, "-cpio")) {
		if((Cpio = creat(b, 0666)) < 0) {
			pr("find: cannot create "), pr(b), pr("\n");
			exit(1);
		}
		Buf = (short *)sbrk(512);
		Wp = Dbuf = (short *)sbrk(5120);
		return(mk((int (*)(struct anode *))cpio, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-newer")) {
		if(stat(b, &Statb) < 0) {
			pr("find: cannot access "), pr(b), pr("\n");
			exit(1);
		}
		Newer = Statb.st_mtime;
		return mk((int (*)(struct anode *))newer, (struct anode *)0, (struct anode *)0);
	}
err:	pr("find: bad option "), pr(a), pr("\n");
	exit(1);
}
struct anode *
mk (int (*f)(struct anode *), struct anode *l, struct anode *r)
{
	Node[Nn].F = f;
	Node[Nn].L = l;
	Node[Nn].R = r;
	return(&(Node[Nn++]));
}

char *
nxtarg (void) { /* get next arg from command line */
	if(Ai>=Argc) {
		pr("find: incomplete statement\n");
		exit(1);
	}
	return(Argv[Ai++]);
}

/* execution time functions */
int
and (register struct anode *p)
{
	return(((*p->L->F)(p->L)) && ((*p->R->F)(p->R))?1:0);
}
int
or (register struct anode *p)
{
	 return(((*p->L->F)(p->L)) || ((*p->R->F)(p->R))?1:0);
}
int
not (register struct anode *p)
{
	return( !((*p->L->F)(p->L)));
}
int
glob (register struct anode *p)
{
	return(gmatch(Fname, (char *)p->L));
}
int
print (void)
{
	puts(Pathname);
	return(1);
}
int
mtime (register struct anode *p)
{
	return(scomp((int)((Now - Statb.st_mtime) / A_DAY),
	    (int)(i32)p->L, (int)(i32)p->R));
}
int
atime (register struct anode *p)
{
	return(scomp((int)((Now - Statb.st_atime) / A_DAY),
	    (int)(i32)p->L, (int)(i32)p->R));
}
int
find_ctime (register struct anode *p)
{
	return(scomp((int)((Now - Statb.st_ctime) / A_DAY),
	    (int)(i32)p->L, (int)(i32)p->R));
}
int
user (register struct anode *p)
{
	return(scomp(Statb.st_uid, (int)(i32)p->L, (int)(i32)p->R));
}
int
ino (register struct anode *p)
{
	return(scomp((int)Statb.st_ino, (int)(i32)p->L, (int)(i32)p->R));
}
int
group (register struct anode *p)
{
	return((i32)p->L == Statb.st_gid);
}
int
links (register struct anode *p)
{
	return(scomp(Statb.st_nlink, (int)(i32)p->L, (int)(i32)p->R));
}
int
size (register struct anode *p)
{
	return(scomp((int)((Statb.st_size+511)>>9),
	    (int)(i32)p->L, (int)(i32)p->R));
}
int
perm (register struct anode *p)
{
	register i;
	i = ((i32)p->R=='-') ? (i32)p->L : 07777;
	return((Statb.st_mode & i & 07777) == (i32)p->L);
}
int
type (register struct anode *p)
{
	return((Statb.st_mode&S_IFMT)==(i32)p->L);
}
int
exeq (register struct anode *p)
{
	fflush(stdout); /* to flush possible `-print' */
	return(doex((i32)p->L));
}
int
ok (struct anode *p)
{
	int c;  int yes;
	yes = 0;
	fflush(stdout); /* to flush possible `-print' */
	pr("< "), pr(Argv[(i32)p->L]), pr(" ... "), pr(Pathname), pr(" >?   ");
	fflush(stderr);
	if((c=getchar())=='y') yes = 1;
	while(c!='\n')
		if(c==EOF)
			exit(2);
		else
			c = getchar();
	if(yes) return(doex((i32)p->L));
	return(0);
}

#define MKSHORT(v, lv) {U.l=1L;if(U.c[0]) U.l=lv, v[0]=U.s[1], v[1]=U.s[0]; else U.l=lv, v[0]=U.s[0], v[1]=U.s[1];}
union { long l; short s[2]; char c[4]; } U;
long
mklong (short v[])
{
	U.l = 1;
	if(U.c[0] /* VAX */)
		U.s[0] = v[1], U.s[1] = v[0];
	else
		U.s[0] = v[0], U.s[1] = v[1];
	return U.l;
}
int
cpio (void)
{
#define MAGIC 070707
	struct header {
		short	h_magic,
			h_dev,
			h_ino,
			h_mode,
			h_uid,
			h_gid,
			h_nlink,
			h_rdev;
		short	h_mtime[2];
		short	h_namesize;
		short	h_filesize[2];
		char	h_name[256];
	} hdr;
	register ifile, ct;
	static long fsz;
	register i;

	hdr.h_magic = MAGIC;
	strcpy(hdr.h_name, !strncmp(Pathname, "./", 2)? Pathname+2: Pathname);
	hdr.h_namesize = strlen(hdr.h_name) + 1;
	hdr.h_uid = Statb.st_uid;
	hdr.h_gid = Statb.st_gid;
	hdr.h_dev = Statb.st_dev;
	hdr.h_ino = Statb.st_ino;
	hdr.h_mode = Statb.st_mode;
	MKSHORT(hdr.h_mtime, Statb.st_mtime);
	hdr.h_nlink = Statb.st_nlink;
	fsz = hdr.h_mode & S_IFREG? Statb.st_size: 0L;
	MKSHORT(hdr.h_filesize, fsz);
	hdr.h_rdev = Statb.st_rdev;
	if(EQ(hdr.h_name, "TRAILER!!!")) {
		bwrite((short *)&hdr, (sizeof hdr-256)+hdr.h_namesize);
		for(i = 0; i < 10; ++i)
			bwrite(Buf, 512);
		return;
	}
	if(!mklong(hdr.h_filesize)) {
		bwrite((short *)&hdr, (sizeof hdr-256)+hdr.h_namesize);
		return;
	}
	if((ifile = open(Fname, 0)) < 0) {
cerror:
		pr("find: cannot copy "), pr(hdr.h_name), pr("\n");
		return;
	}
	bwrite((short *)&hdr, (sizeof hdr-256)+hdr.h_namesize);
	for(fsz = mklong(hdr.h_filesize); fsz > 0; fsz -= 512) {
		ct = fsz>512? 512: fsz;
		if(read(ifile, (char *)Buf, ct) < 0)
			goto cerror;
		bwrite(Buf, ct);
	}
	close(ifile);
	return;
}
int
newer (void)
{
	return Statb.st_mtime > Newer;
}

/* support functions */
int
scomp ( /* funny signed compare */
    register a,
    register b,
    int s
)
{
	if(s == '+')
		return(a > b);
	if(s == '-')
		return(a < (b * -1));
	return(a == b);
}

int
doex (i32 com)
{
	register np;
	register char *na;
	static char *nargv[50];
	static i16 ccode;

	ccode = np = 0;
	while (na=Argv[com++]) {
		if(strcmp(na, ";")==0) break;
		if(strcmp(na, "{}")==0) nargv[np++] = Pathname;
		else nargv[np++] = na;
	}
	nargv[np] = 0;
	if (np==0) return(9);
	if(fork()) /*parent*/ wait(&ccode);
	else { /*child*/
		chdir(Home);
		execvp(nargv[0], nargv);
		exit(1);
	}
	return(ccode ? 0:1);
}

int
getunum (char *f, char *s)
{
	register i;
	register char *sp;
	register c;
	char str[20];
	FILE *pin;

	i = -1;
	pin = fopen(f, "r");
	c = '\n'; /* prime with a CR */
	do {
		if(c=='\n') {
			sp = str;
			while((c = *sp++ = getc(pin)) != ':')
				if(c == EOF) goto RET;
			*--sp = '\0';
			if(EQ(str, s)) {
				while((c=getc(pin)) != ':')
					if(c == EOF) goto RET;
				sp = str;
				while((*sp = getc(pin)) != ':') sp++;
				*sp = '\0';
				i = atoi(str);
				goto RET;
			}
		}
	} while((c = getc(pin)) != EOF);
 RET:
	fclose(pin);
	return(i);
}

int
descend (char *name, char *fname, struct anode *exlist)
{
	int	dir = 0, /* open directory */
		offset,
		dsize,
		entries,
		dirsize;
	struct direct dentry[32];
	register struct direct	*dp;
	register char *c1, *c2;
	int i;
	int rv = 0;
	char *endofname;

	if(stat(fname, &Statb)<0) {
		pr("find: bad status-- "), pr(name), pr("\n");
		return(0);
	}
	(*exlist->F)(exlist);
	if((Statb.st_mode&S_IFMT)!=S_IFDIR)
		return(1);

	for(c1 = name; *c1; ++c1);
	if(*(c1-1) == '/')
		--c1;
	endofname = c1;
	dirsize = Statb.st_size;

	if(chdir(fname) == -1)
		return(0);
	for(offset=0 ; offset < dirsize ; offset += 512) { /* each block */
		dsize = 512<(dirsize-offset)? 512: (dirsize-offset);
		if(!dir) {
			if((dir=open(".", 0))<0) {
				pr("find: cannot open "), pr(name), pr("\n");
				rv = 0;
				goto ret;
			}
			if(offset) lseek(dir, (long)offset, 0);
			if(read(dir, (char *)dentry, dsize)<0) {
				pr("find: cannot read "), pr(name), pr("\n");
				rv = 0;
				goto ret;
			}
			if(dir > 10) {
				close(dir);
				dir = 0;
			}
		} else
			if(read(dir, (char *)dentry, dsize)<0) {
				pr("find: cannot read "), pr(name), pr("\n");
				rv = 0;
				goto ret;
			}
		for(dp=dentry, entries=dsize>>4; entries; --entries, ++dp) { /* each directory entry */
			if(dp->d_ino==0
			|| (dp->d_name[0]=='.' && dp->d_name[1]=='\0')
			|| (dp->d_name[0]=='.' && dp->d_name[1]=='.' && dp->d_name[2]=='\0'))
				continue;
			c1 = endofname;
			*c1++ = '/';
			c2 = dp->d_name;
			for(i=0; i<14; ++i)
				if(*c2)
					*c1++ = *c2++;
				else
					break;
			*c1 = '\0';
			if(c1 == endofname) { /* ?? */
				rv = 0;
				goto ret;
			}
			Fname = endofname+1;
			if(!descend(name, Fname, exlist)) {
				*endofname = '\0';
				chdir(Home);
				if(chdir(Pathname) == -1) {
					pr("find: bad directory tree\n");
					exit(1);
				}
			}
		}
	}
	rv = 1;
ret:
	if(dir)
		close(dir);
	if(chdir("..") == -1) {
		*endofname = '\0';
		pr("find: bad directory "), pr(name), pr("\n");
		rv = 1;
	}
	return(rv);
}

int
gmatch ( /* string match as in glob */
    register char *s,
    register char *p
)
{
	if (*s=='.' && *p!='.') return(0);
	return amatch(s, p);
}

int
amatch (register char *s, register char *p)
{
	register cc;
	int scc, k;
	int c, lc;

	scc = *s;
	lc = 077777;
	switch (c = *p) {

	case '[':
		k = 0;
		while (cc = *++p) {
			switch (cc) {

			case ']':
				if (k)
					return(amatch(++s, ++p));
				else
					return(0);

			case '-':
				k |= lc <= scc & scc <= (cc=p[1]);
			}
			if (scc==(lc=cc)) k++;
		}
		return(0);

	case '?':
	caseq:
		if(scc) return(amatch(++s, ++p));
		return(0);
	case '*':
		return(umatch(s, ++p));
	case 0:
		return(!scc);
	}
	if (c==scc) goto caseq;
	return(0);
}

int
umatch (register char *s, register char *p)
{
	if(*p==0) return(1);
	while(*s)
		if (amatch(s++, p)) return(1);
	return(0);
}

int
bwrite (register short *rp, register c)
{
	register short *wp = Wp;

	c = (c+1) >> 1;
	while(c--) {
		if(!Wct) {
again:
			if(write(Cpio, (char *)Dbuf, Bufsize)<0) {
				Cpio = chgreel(1, Cpio);
				goto again;
			}
			Wct = Bufsize >> 1;
			wp = Dbuf;
			++Blocks;
		}
		*wp++ = *rp++;
		--Wct;
	}
	Wp = wp;
}
int
chgreel (int x, int fl)
{
	register f;
	char str[22];
	FILE *devtty;
	struct stat statb;

	pr("find: can't "), pr(x? "write output": "read input"), pr("\n");
	fstat(fl, &statb);
	if((statb.st_mode&S_IFMT) != S_IFCHR)
		exit(1);
again:
	pr("If you want to go on, type device/file name when ready\n");
	devtty = fopen("/dev/tty", "r");
	fgets(str, 20, devtty);
	str[strlen(str) - 1] = '\0';
	if(!*str)
		exit(1);
	close(fl);
	if((f = open(str, x? 1: 0)) < 0) {
		pr("That didn't work");
		fclose(devtty);
		goto again;
	}
	return f;
}
int
pr (char *s)
{
	fputs(s, stderr);
}
