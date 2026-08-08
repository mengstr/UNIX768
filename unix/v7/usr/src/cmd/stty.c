/*
 * set teletype modes
 */

#include <stdio.h>
#include <sgtty.h>
#include <unistd.h>

#define EPOCH68_MINIMAL_STTY 1

struct
{
	char	*string;
	int	speed;
} speeds[] = {
	"0",	B0,
	"50",	B50,
	"75",	B75,
	"110",	B110,
	"134",	B134,
	"134.5",B134,
	"150",	B150,
	"200",	B200,
	"300",	B300,
	"600",	B600,
	"1200",	B1200,
	"1800",	B1800,
	"2400",	B2400,
	"4800",	B4800,
	"9600",	B9600,
	"exta",	EXTA,
	"extb",	EXTB,
	0,
};
struct
{
	char	*string;
	int	set;
	int	reset;
} modes[] = {
	"even",
	EVENP, 0,

	"-even",
	0, EVENP,

	"odd",
	ODDP, 0,

	"-odd",
	0, ODDP,

	"raw",
	RAW, 0,

	"-raw",
	0, RAW,

	"cooked",
	0, RAW,

	"-nl",
	CRMOD, 0,

	"nl",
	0, CRMOD,

	"echo",
	ECHO, 0,

	"-echo",
	0, ECHO,

	"LCASE",
	LCASE, 0,

	"lcase",
	LCASE, 0,

	"-LCASE",
	0, LCASE,

	"-lcase",
	0, LCASE,

	"-tabs",
	XTABS, 0,

	"tabs",
	0, XTABS,


	"cbreak",
	CBREAK, 0,

	"-cbreak",
	0, CBREAK,

	"cr0",
	CR0, CR3,

	"cr1",
	CR1, CR3,

	"cr2",
	CR2, CR3,

	"cr3",
	CR3, CR3,

	"tab0",
	TAB0, XTABS,

	"tab1",
	TAB1, XTABS,

	"tab2",
	TAB2, XTABS,

	"nl0",
	NL0, NL3,

	"nl1",
	NL1, NL3,

	"nl2",
	NL2, NL3,

	"nl3",
	NL3, NL3,

	"ff0",
	FF0, FF1,

	"ff1",
	FF1, FF1,

	"bs0",
	BS0, BS1,

	"bs1",
	BS1, BS1,

	"33",
	CR1, ALLDELAY,

	"tty33",
	CR1, ALLDELAY,

	"37",
	FF1+CR2+TAB1+NL1, ALLDELAY,

	"tty37",
	FF1+CR2+TAB1+NL1, ALLDELAY,

	"05",
	NL2, ALLDELAY,

	"vt05",
	NL2, ALLDELAY,

	"tn",
	CR1, ALLDELAY,

	"tn300",
	CR1, ALLDELAY,

	"ti",
	CR2, ALLDELAY,

	"ti700",
	CR2, ALLDELAY,

	"tek",
	FF1, ALLDELAY,

	0,
	};

char	*arg;
struct sgttyb mode;
struct tchars tchar;

int eq(char *string);
int charval(char *s);
void prmodes(void);
void delay(int m, char *s);
void prspeed(char *c, int s);

#if EPOCH68_MINIMAL_STTY
static int
strlen_local (char *s)
{
	register n;

	n = 0;
	while(*s++) n++;
	return(n);
}

static void
ewrite (char *s)
{
	write(2, s, strlen_local(s));
}

static void
eputc (int c)
{
	char b;

	b = c;
	write(2, &b, 1);
}

static void
edec (int n)
{
	char buf[12];
	register i;

	if(n == 0) {
		eputc('0');
		return;
	}
	i = sizeof(buf);
	while(n > 0 && i > 0) {
		buf[--i] = '0' + (n % 10);
		n /= 10;
	}
	write(2, &buf[i], sizeof(buf) - i);
}
#endif

int
main (int argc, char *argv[])
{
	int i;

#if EPOCH68_MINIMAL_STTY
	if(ioctl(1, TIOCGETP, &mode) < 0) {
		ewrite("stty: not a tty\n");
		exit(1);
	}
	ewrite("@stty:getp ok\n");
#else
	gtty(1, &mode);
#endif
	if(ioctl(1, TIOCGETC, &tchar) < 0) {
#if EPOCH68_MINIMAL_STTY
		ewrite("stty: cannot get special characters\n");
#else
		fprintf(stderr, "stty: cannot get special characters\n");
#endif
		exit(1);
	}
	if(argc == 1) {
		prmodes();
		exit(0);
	}
	while(--argc > 0) {

		arg = *++argv;
		ewrite("@stty:arg ");
		ewrite(arg);
		ewrite("\n");
		if (eq("ek")){
			mode.sg_erase = '#';
			mode.sg_kill = '@';
		}
		if (eq("erase")) {
			mode.sg_erase = charval(*++argv);
			argc--;
		}
		if (eq("kill")) {
			mode.sg_kill = charval(*++argv);
			argc--;
		}
		if (eq("intr")) {
			tchar.t_intrc = charval(*++argv);
			argc--;
		}
		if (eq("gspeed")) {
			mode.sg_ispeed = B300;
			mode.sg_ospeed = B9600;
		}
		if (eq("hup")) {
			ioctl(1, TIOCHPCL, NULL);
		} else
		for(i=0; speeds[i].string; i++)
			if(eq(speeds[i].string))
				mode.sg_ispeed = mode.sg_ospeed = speeds[i].speed;
		for(i=0; modes[i].string; i++)
			if(eq(modes[i].string)) {
				mode.sg_flags &= ~modes[i].reset;
				mode.sg_flags |= modes[i].set;
			}
		if(arg)
#if EPOCH68_MINIMAL_STTY
		{
			ewrite("unknown mode: ");
			ewrite(arg);
			ewrite("\n");
		}
#else
			fprintf(stderr,"unknown mode: %s\n", arg);
#endif
	}
#if EPOCH68_MINIMAL_STTY
	ewrite("@stty:setn enter\n");
	ioctl(1, TIOCSETN, &mode);
	ewrite("@stty:setn done\n");
#else
	stty(1,&mode);
#endif
	ioctl(1, TIOCSETC, &tchar);
}

int
charval(char *s)
{
	if(s[0] == '^') {
		if(s[1] == '?')
			return(0177);
		return(s[1] & 037);
	}
	return(s[0] & 0377);
}

int
eq (char *string)
{
	int i;

	if(!arg)
		return(0);
	i = 0;
loop:
	if(arg[i] != string[i])
		return(0);
	if(arg[i++] != '\0')
		goto loop;
	arg = 0;
	return(1);
}

void
prmodes (void)
{
	register m;

	if(mode.sg_ispeed != mode.sg_ospeed) {
		prspeed("input speed  ", mode.sg_ispeed);
		prspeed("output speed ", mode.sg_ospeed);
	} else
		prspeed("speed ", mode.sg_ispeed);
	if (mode.sg_erase < ' ')
#if EPOCH68_MINIMAL_STTY
	{
		ewrite("erase = '^");
		eputc('@' + mode.sg_erase);
		ewrite("'; ");
	}
#else
		fprintf(stderr, "erase = '^%c'; ", '@' + mode.sg_erase);
#endif
	else
#if EPOCH68_MINIMAL_STTY
	{
		ewrite("erase = '");
		eputc(mode.sg_erase);
		ewrite("'; ");
	}
#else
		fprintf(stderr, "erase = '%c'; ", mode.sg_erase);
#endif
	if (mode.sg_kill < ' ')
#if EPOCH68_MINIMAL_STTY
	{
		ewrite("kill = '^");
		eputc('@' + mode.sg_kill);
		ewrite("'\n");
	}
#else
		fprintf(stderr, "kill = '^%c'\n", '@' + mode.sg_kill);
#endif
	else
#if EPOCH68_MINIMAL_STTY
	{
		ewrite("kill = '");
		eputc(mode.sg_kill);
		ewrite("'\n");
	}
#else
		fprintf(stderr, "kill = '%c'\n", mode.sg_kill);
#endif
	m = mode.sg_flags;
#if EPOCH68_MINIMAL_STTY
	if(m & EVENP)	ewrite("even ");
	if(m & ODDP)	ewrite("odd ");
	if(m & RAW)	ewrite("raw ");
	if(m & CRMOD)	ewrite("-nl ");
	if(m & ECHO)	ewrite("echo ");
	if(m & LCASE)	ewrite("lcase ");
	if((m & XTABS)==XTABS)	ewrite("-tabs ");
	if (m & CBREAK)	ewrite("cbreak ");
#else
	if(m & EVENP)	fprintf(stderr,"even ");
	if(m & ODDP)	fprintf(stderr,"odd ");
	if(m & RAW)	fprintf(stderr,"raw ");
	if(m & CRMOD)	fprintf(stderr,"-nl ");
	if(m & ECHO)	fprintf(stderr,"echo ");
	if(m & LCASE)	fprintf(stderr,"lcase ");
	if((m & XTABS)==XTABS)	fprintf(stderr,"-tabs ");
	if (m & CBREAK)	fprintf(stderr,"cbreak ");
#endif
	delay((m&NLDELAY)/NL1,	"nl");
	if ((m&TBDELAY)!=XTABS)
		delay((m&TBDELAY)/TAB1,	"tab");
	delay((m&CRDELAY)/CR1,	"cr");
	delay((m&VTDELAY)/FF1,	"ff");
	delay((m&BSDELAY)/BS1,	"bs");
#if EPOCH68_MINIMAL_STTY
	ewrite("\n");
#else
	fprintf(stderr,"\n");
#endif
}

void
delay (int m, char *s)
{

	if(m)
#if EPOCH68_MINIMAL_STTY
	{
		ewrite(s);
		edec(m);
		ewrite(" ");
	}
#else
		fprintf(stderr,"%s%d ", s, m);
#endif
}

int	speed[] = {
	0,50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,0,0
};

void
prspeed (char *c, int s)
{

#if EPOCH68_MINIMAL_STTY
	ewrite(c);
	edec(speed[s]);
	ewrite(" baud\n");
#else
	fprintf(stderr,"%s%d baud\n", c, speed[s]);
#endif
}
