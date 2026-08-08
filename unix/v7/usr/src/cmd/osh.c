/*
 */

#include <setjmp.h>
#include <signal.h>
#include <sys/inttypes.h>
#include <unistd.h>
#define	INTR	2
#define	QUIT	3
#define LINSIZ 1000
#define ARGSIZ 50
#define TRESIZ 100

#define QUOTE 0200
#define FAND 1
#define FCAT 2
#define FPIN 4
#define FPOU 8
#define FPAR 16
#define FINT 32
#define FPRS 64
#define TCOM 1
#define TPAR 2
#define TFIL 3
#define TLST 4
#define DTYP 0
#define DLEF 1
#define DRIT 2
#define DFLG 3
#define DSPR 4
#define DCOM 5
#define	ENOMEM	12
#define	ENOEXEC 8

int errval;
char	*dolp;
char	pidp[6];
char	**dolv;
jmp_buf	jmpbuf;
int	dolc;
char	*promp;
char	*linep;
char	*elinep;
char	**argp;
char	**eargp;
long	*treep;
long	*treeend;
char	peekc;
char	gflg;
char	error;
char	uid;
char	setintr;
char	*arginp;
int	onelflg;
int	stoperr;

#define	OSH_NSIG	16
char	*mesg[OSH_NSIG] = {
	0,
	"Hangup",
	0,
	"Quit",
	"Illegal instruction",
	"Trace/BPT trap",
	"IOT trap",
	"EMT trap",
	"Floating exception",
	"Killed",
	"Bus error",
	"Memory fault",
	"Bad system call",
	0,
	"Alarm clock",
	"Terminated",
};

char	line[LINSIZ];
char	*args[ARGSIZ];
long	trebuf[TRESIZ];

static i32	tglob(i32 c);
static i32	trim(i32 c);
static long	*syntax(char **p1, char **p2);
static long	*syn1(char **p1, char **p2);
static long	*syn2(char **p1, char **p2);
static long	*syn3(char **p1, char **p2);
static long	*tree(i32 n);
static i32	getc(void);
static i32	readc(void);
static i32	any(i32 c, char *as);
static i32	equal(char *as1, char *as2);
static void	main1(void);
static void	word(void);
static void	scan(long *at, i32 (*f)(i32));
static void	execute(long *t, i16 *pf1, i16 *pf2);
static void	texec(char *f, long *at);
static void	err(char *s, i32 exitno);
static void	prs(char *as);
static void	putc(i32 c);
static void	prn(i32 n);
static void	pwait(i32 i, long *t);

int
main(int c, char **av)
{
	register f;
	register char *acname, **v;
	sighandler_t intr;

	for(f=3; f<15; f++)
		close(f);
	dolc = getpid();
	for(f=4; f>=0; f--) {
		dolc = dolc/10;
		pidp[f] = dolc%10 + '0';
	}
	v = av;
	acname = "<none>";
	promp = "% ";
	if((uid = getuid()) == 0)
		promp = "# ";
	if(c>1 && v[1][0]=='-' && v[1][1]=='e') {
		++stoperr;
		v[1] = v[0];
		++v;
		--c;
	}
	if(c > 1) {
		promp = 0;
		if (*v[1]=='-') {
			**v = '-';
			if (v[1][1]=='c' && c>2)
				arginp = v[2];
			else if (v[1][1]=='t')
				onelflg = 2;
		} else {
			close(0);
			f = open(v[1], 0);
			if(f < 0) {
				prs(v[1]);
				err(": cannot open",255);
			}
		}
	}
	if(**v == '-') {
		signal(QUIT, SIG_IGN);
		intr = signal(INTR, SIG_IGN);
		if ((arginp==0&&onelflg==0) || (((i32)intr&01)==0))
			setintr++;
	}
	dolv = v+1;
	dolc = c-1;

loop:
	if(promp != 0)
		prs(promp);
	peekc = getc();
	main1();
	goto loop;
}

static void
main1()
{
	register char  *cp;
	register long *t;

	argp = args;
	eargp = args+ARGSIZ-5;
	linep = line;
	elinep = line+LINSIZ-5;
	error = 0;
	gflg = 0;
	do {
		cp = linep;
		word();
	} while(*cp != '\n');
	treep = trebuf;
	treeend = &trebuf[TRESIZ];
	if(gflg == 0) {
		if(error == 0) {
			setjmp(jmpbuf);
			if (error)
				return;
			t = syntax(args, argp);
		}
		if(error != 0)
			err("syntax error",255); else
			execute(t, 0, 0);
	}
}

static void
word()
{
	register char c, c1;

	*argp++ = linep;

loop:
	switch(c = getc()) {

	case ' ':
	case '\t':
		goto loop;

	case '\'':
	case '"':
		c1 = c;
		while((c=readc()) != c1) {
			if(c == '\n') {
				error++;
				peekc = c;
				return;
			}
			*linep++ = c|QUOTE;
		}
		goto pack;

	case '&':
	case ';':
	case '<':
	case '>':
	case '(':
	case ')':
	case '|':
	case '^':
	case '\n':
		*linep++ = c;
		*linep++ = '\0';
		return;
	}

	peekc = c;

pack:
	for(;;) {
		c = getc();
		if(any(c, " '\"\t;&<>()|^\n")) {
			peekc = c;
			if(any(c, "\"'"))
				goto loop;
			*linep++ = '\0';
			return;
		}
		*linep++ = c;
	}
}

static long *
tree(n)
i32 n;
{
	register long *t;

	t = treep;
	treep += n;
	if (treep>treeend) {
		prs("Command line overflow\n");
		error++;
		longjmp(jmpbuf, 1);
	}
	return(t);
}

static i32
getc()
{
	register char c;

	if(peekc) {
		c = peekc;
		peekc = 0;
		return(c);
	}
	if(argp > eargp) {
		argp -= 10;
		while((c=getc()) != '\n');
		argp += 10;
		err("Too many args",255);
		gflg++;
		return(c);
	}
	if(linep > elinep) {
		linep -= 10;
		while((c=getc()) != '\n');
		linep += 10;
		err("Too many characters",255);
		gflg++;
		return(c);
	}
getd:
	if(dolp) {
		c = *dolp++;
		if(c != '\0')
			return(c);
		dolp = 0;
	}
	c = readc();
	if(c == '\\') {
		c = readc();
		if(c == '\n')
			return(' ');
		return(c|QUOTE);
	}
	if(c == '$') {
		c = readc();
		if(c>='0' && c<='9') {
			if(c-'0' < dolc)
				dolp = dolv[c-'0'];
			goto getd;
		}
		if(c == '$') {
			dolp = pidp;
			goto getd;
		}
	}
	return(c&0177);
}

static i32
readc()
{
	int rdstat;
	char cc;
	register c;

	if (arginp) {
		if (arginp == (char *)1)
			exit(errval);
		if ((c = *arginp++) == 0) {
			arginp = (char *)1;
			c = '\n';
		}
		return(c);
	}
	if (onelflg==1)
		exit(255);
	if((rdstat = read(0, &cc, 1)) != 1)
		if(rdstat==0) exit(errval); /* end of file*/
		else exit(255); /* error */
	if (cc=='\n' && onelflg)
		onelflg--;
	return(cc);
}

/*
 * syntax
 *	empty
 *	syn1
 */

static long *
syntax(p1, p2)
char **p1, **p2;
{

	while(p1 != p2) {
		if(any(**p1, ";&\n"))
			p1++; else
			return(syn1(p1, p2));
	}
	return(0);
}

/*
 * syn1
 *	syn2
 *	syn2 & syntax
 *	syn2 ; syntax
 */

static long *
syn1(p1, p2)
char **p1, **p2;
{
	register char **p;
	register long *t, *t1;
	int l;

	l = 0;
	for(p=p1; p!=p2; p++)
	switch(**p) {

	case '(':
		l++;
		continue;

	case ')':
		l--;
		if(l < 0)
			error++;
		continue;

	case '&':
	case ';':
	case '\n':
		if(l == 0) {
			l = **p;
			t = tree(4);
			t[DTYP] = TLST;
			t[DLEF] = (long)syn2(p1, p);
			t[DFLG] = 0;
			if(l == '&') {
				t1 = (long *)t[DLEF];
				t1[DFLG] |= FAND|FPRS|FINT;
			}
			t[DRIT] = (long)syntax(p+1, p2);
			return(t);
		}
	}
	if(l == 0)
		return(syn2(p1, p2));
	error++;
	return(0);
}

/*
 * syn2
 *	syn3
 *	syn3 | syn2
 */

static long *
syn2(p1, p2)
char **p1, **p2;
{
	register char **p;
	register int l;
	register long *t;

	l = 0;
	for(p=p1; p!=p2; p++)
	switch(**p) {

	case '(':
		l++;
		continue;

	case ')':
		l--;
		continue;

	case '|':
	case '^':
		if(l == 0) {
			t = tree(4);
			t[DTYP] = TFIL;
			t[DLEF] = (long)syn3(p1, p);
			t[DRIT] = (long)syn2(p+1, p2);
			t[DFLG] = 0;
			return(t);
		}
	}
	return(syn3(p1, p2));
}

/*
 * syn3
 *	( syn1 ) [ < in  ] [ > out ]
 *	word word* [ < in ] [ > out ]
 */

static long *
syn3(p1, p2)
char **p1, **p2;
{
	register char **p;
	char **lp, **rp;
	register long *t;
	int n, l, c, flg;
	char *i, *o;

	flg = 0;
	if(**p2 == ')')
		flg |= FPAR;
	lp = 0;
	rp = 0;
	i = 0;
	o = 0;
	n = 0;
	l = 0;
	for(p=p1; p!=p2; p++)
	switch(c = **p) {

	case '(':
		if(l == 0) {
			if(lp != 0)
				error++;
			lp = p+1;
		}
		l++;
		continue;

	case ')':
		l--;
		if(l == 0)
			rp = p;
		continue;

	case '>':
		p++;
		if(p!=p2 && **p=='>')
			flg |= FCAT; else
			p--;

	case '<':
		if(l == 0) {
			p++;
			if(p == p2) {
				error++;
				p--;
			}
			if(any(**p, "<>("))
				error++;
			if(c == '<') {
				if(i != 0)
					error++;
				i = *p;
				continue;
			}
			if(o != 0)
				error++;
			o = *p;
		}
		continue;

	default:
		if(l == 0)
			p1[n++] = *p;
	}
	if(lp != 0) {
		if(n != 0)
			error++;
		t = tree(5);
		t[DTYP] = TPAR;
		t[DSPR] = (long)syn1(lp, rp);
		goto out;
	}
	if(n == 0)
		error++;
	p1[n++] = 0;
	t = tree(n+5);
	t[DTYP] = TCOM;
	for(l=0; l<n; l++)
		t[l+DCOM] = (long)p1[l];
out:
	t[DFLG] = flg;
	t[DLEF] = (long)i;
	t[DRIT] = (long)o;
	return(t);
}

static void
scan(at, f)
long *at;
i32 (*f)(i32);
{
	register char *p, c;
	register long *t;

	t = at+DCOM;
	while(p = (char *)*t++)
		while(c = *p)
			*p++ = (*f)(c);
}

static i32
tglob(c)
i32 c;
{

	if(any(c, "[?*"))
		gflg = 1;
	return(c);
}

static i32
trim(c)
i32 c;
{

	return(c&0177);
}

static void
execute(t, pf1, pf2)
long *t;
i16 *pf1, *pf2;
{
	int i, f;
	i16 pv[2];
	register long *t1;
	register char *cp1, *cp2;
	extern errno;

	if(t != 0)
	switch(t[DTYP]) {

	case TCOM:
		cp1 = (char *)t[DCOM];
		if(equal(cp1, "chdir") || equal(cp1, "cd")) {
			if(t[DCOM+1] != 0) {
				if(chdir((char *)t[DCOM+1]) < 0)
					err("chdir: bad directory",255);
			} else
				err("chdir: arg count",255);
			return;
		}
		if(equal(cp1, "shift")) {
			if(dolc < 1) {
				prs("shift: no args\n");
				return;
			}
			dolv[1] = dolv[0];
			dolv++;
			dolc--;
			return;
		}
		if(equal(cp1, "login")) {
			if(promp != 0) {
				execv("/bin/login", (char **)(t+DCOM));
			}
			prs("login: cannot execute\n");
			return;
		}
		if(equal(cp1, "newgrp")) {
			if(promp != 0) {
				execv("/bin/newgrp", (char **)(t+DCOM));
			}
			prs("newgrp: cannot execute\n");
			return;
		}
		if(equal(cp1, "wait")) {
			pwait(-1, 0);
			return;
		}
		if(equal(cp1, ":"))
			return;

	case TPAR:
		f = t[DFLG];
		i = 0;
		if((f&FPAR) == 0)
			i = fork();
		if(i == -1) {
			err("try again",255);
			return;
		}
		if(i != 0) {
			if((f&FPIN) != 0) {
				close(pf1[0]);
				close(pf1[1]);
			}
			if((f&FPRS) != 0) {
				prn(i);
				prs("\n");
			}
			if((f&FAND) != 0)
				return;
			if((f&FPOU) == 0)
				pwait(i, t);
			return;
		}
		if(t[DLEF] != 0) {
			close(0);
			i = open((char *)t[DLEF], 0);
			if(i < 0) {
				prs((char *)t[DLEF]);
				err(": cannot open",255);
				exit(255);
			}
		}
		if(t[DRIT] != 0) {
			if((f&FCAT) != 0) {
				i = open((char *)t[DRIT], 1);
				if(i >= 0) {
					lseek(i, 0L, 2);
					goto f1;
				}
			}
			i = creat((char *)t[DRIT], 0666);
			if(i < 0) {
				prs((char *)t[DRIT]);
				err(": cannot create",255);
				exit(255);
			}
		f1:
			close(1);
			dup(i);
			close(i);
		}
		if((f&FPIN) != 0) {
			close(0);
			dup(pf1[0]);
			close(pf1[0]);
			close(pf1[1]);
		}
		if((f&FPOU) != 0) {
			close(1);
			dup(pf2[1]);
			close(pf2[0]);
			close(pf2[1]);
		}
		if((f&FINT)!=0 && t[DLEF]==0 && (f&FPIN)==0) {
			close(0);
			open("/dev/null", 0);
		}
		if((f&FINT) == 0 && setintr) {
			signal(INTR, SIG_DFL);
			signal(QUIT, SIG_DFL);
		}
		if(t[DTYP] == TPAR) {
			if(t1 = (long *)t[DSPR])
				t1[DFLG] |= f&FINT;
				execute(t1, 0, 0);
			exit(255);
		}
		gflg = 0;
		scan(t, tglob);
		if(gflg) {
			t[DSPR] = (long)"/etc/glob";
			execv((char *)t[DSPR], (char **)(t+DSPR));
			prs("glob: cannot execute\n");
			exit(255);
		}
		scan(t, trim);
		*linep = 0;
		texec((char *)t[DCOM], t);
		cp1 = linep;
		cp2 = "/usr/bin/";
		while(*cp1 = *cp2++)
			cp1++;
		cp2 = (char *)t[DCOM];
		while(*cp1++ = *cp2++);
		texec(linep+4, t);
		texec(linep, t);
		prs((char *)t[DCOM]);
		err(": not found",255);
		exit(255);

	case TFIL:
		f = t[DFLG];
		pipe(pv);
		t1 = (long *)t[DLEF];
		t1[DFLG] |= FPOU | (f&(FPIN|FINT|FPRS));
		execute(t1, pf1, pv);
		t1 = (long *)t[DRIT];
		t1[DFLG] |= FPIN | (f&(FPOU|FINT|FAND|FPRS));
		execute(t1, pv, pf2);
		return;

	case TLST:
		f = t[DFLG]&FINT;
		if(t1 = (long *)t[DLEF])
			t1[DFLG] |= f;
		execute(t1, 0, 0);
		if(t1 = (long *)t[DRIT])
			t1[DFLG] |= f;
		execute(t1, 0, 0);
		return;

	}
}

static void
texec(f, at)
char *f;
long *at;
{
	extern errno;
	register long *t;

	t = at;
	errno = 0;
	execv(f, (char **)(t+DCOM));
	if (errno==ENOEXEC) {
		if (*linep)
			t[DCOM] = (long)linep;
		t[DSPR] = (long)"/bin/sh";
		errno = 0;
		execv((char *)t[DSPR], (char **)(t+DSPR));
		prs("No shell!\n");
		exit(255);
	}
	if (errno==ENOMEM) {
		prs((char *)t[DCOM]);
		err(": too large",255);
		exit(255);
	}
}

static void
err(s, exitno)
char *s;
i32 exitno;
{

	prs(s);
	prs("\n");
	if(promp == 0) {
		lseek(0, 0L, 2);
		exit(exitno);
	}
}

static void
prs(as)
char *as;
{
	register char *s;

	s = as;
	while(*s)
		putc(*s++);
}

static void
putc(c)
i32 c;
{
	char cc;

	cc = c;
	write(2, &cc, 1);
}

static void
prn(n)
i32 n;
{
	register a;

	if (a = n/10)
		prn(a);
	putc(n%10 + '0');
}

static i32
any(c, as)
i32 c;
char *as;
{
	register char *s;

	s = as;
	while(*s)
		if(*s++ == c)
			return(1);
	return(0);
}

static i32
equal(as1, as2)
char *as1, *as2;
{
	register char *s1, *s2;

	s1 = as1;
	s2 = as2;
	while(*s1++ == *s2)
		if(*s2++ == '\0')
			return(1);
	return(0);
}

static void
pwait(i, t)
i32 i;
long *t;
{
	register p, e;
		i16 s;

	if(i != 0)
	for(;;) {
		p = wait(&s);
		if(p == -1)
			break;
		e = s&0177;
		if (e>=OSH_NSIG || mesg[e]) {
			if(p != i) {
				prn(p);
				prs(": ");
			}
			if (e < OSH_NSIG)
				prs(mesg[e]);
			else {
				prs("Signal ");
				prn(e);
			}
			if(s&0200)
				prs(" -- Core dumped");
		}
		if (e || s&&stoperr) {
			if (stoperr) {
				prs("osh: child wait status ");
				prn(s);
				prs("\n");
			}
			err("", (s>>8)|e );
		}
		errval |= (s>>8);
	}
}
