/*
 * acct [ -w wtmp ] [ -d ] [ -p ] [ people ]
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <utmp.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdlib.h>

#define	TSIZE	33
#define	USIZE	200
struct tbuf;
static void loop(void);
static void print(void);
static void upall(int);
static void update(struct tbuf *, int);
static int among(int);
static void newday(void);
static void puser(char *, long);
static void phours(long);
static void pdate(void);

struct  utmp ibuf;

struct ubuf {
	char	uname[8];
	long	utime;
} ubuf[USIZE];

struct tbuf {
	struct	ubuf	*userp;
	long	ttime;
} tbuf[TSIZE];

char	*wtmp;
int	pflag, byday;
long	dtime;
long	midnight;
long	lastime;
long	day	= 86400L;
int	pcount;
char	**pptr;

int
main(int argc, char **argv)
{
	int c, fl;
	register int i;
	FILE *wf;

	wtmp = "/usr/adm/wtmp";
	while (--argc > 0 && **++argv == '-')
	switch(*++*argv) {
	case 'd':
		byday++;
		continue;

	case 'w':
		if (--argc>0)
			wtmp = *++argv;
		continue;

	case 'p':
		pflag++;
		continue;
	}
	pcount = argc;
	pptr = argv;
	if ((wf = fopen(wtmp, "r")) == NULL) {
		printf("No %s\n", wtmp);
		exit(1);
	}
	for(;;) {
		if (fread((char *)&ibuf, sizeof(ibuf), 1, wf) != 1)
			break;
		fl = 0;
		for (i=0; i<8; i++) {
			c = ibuf.ut_name[i];
			if(isdigit(c) || isalpha(c)) {
				if (fl)
					goto skip;
				continue;
			}
			if (c==' ' || c=='\0') {
				fl++;
				ibuf.ut_name[i] = '\0';
			} else
				goto skip;
		}
		loop();
    skip:;
	}
	ibuf.ut_name[0] = '\0';
	ibuf.ut_line[0] = '~';
	time(&ibuf.ut_time);
	loop();
	print();
	exit(0);
}

static void
loop(void)
{
	register int i;
	register struct tbuf *tp;
	register struct ubuf *up;

	if(ibuf.ut_line[0] == '|') {
		dtime = ibuf.ut_time;
		return;
	}
	if(ibuf.ut_line[0] == '}') {
		if(dtime == 0)
			return;
		for(tp = tbuf; tp < &tbuf[TSIZE]; tp++)
			tp->ttime += ibuf.ut_time-dtime;
		dtime = 0;
		return;
	}
	if (lastime>ibuf.ut_time || lastime+day+day/2<ibuf.ut_time)
		midnight = 0;
	if (midnight==0)
		newday();
	lastime = ibuf.ut_time;
	if (byday && ibuf.ut_time > midnight) {
		upall(1);
		print();
		newday();
		for (up=ubuf; up < &ubuf[USIZE]; up++)
			up->utime = 0;
	}
	if (ibuf.ut_line[0] == '~') {
		ibuf.ut_name[0] = '\0';
		upall(0);
		return;
	}
	if (ibuf.ut_line[0]=='t')
		i = (ibuf.ut_line[3]-'0')*10 + (ibuf.ut_line[4]-'0');
	else
		i = TSIZE-1;
	if (i<0 || i>=TSIZE)
		i = TSIZE-1;
	tp = &tbuf[i];
	update(tp, 0);
}

static void
print(void)
{
	int i;
	long ttime, t;

	ttime = 0;
	for (i=0; i<USIZE; i++) {
		if(!among(i))
			continue;
		t = ubuf[i].utime;
		if (t>0)
			ttime += t;
		if (pflag && ubuf[i].utime > 0)
			puser(ubuf[i].uname, ubuf[i].utime);
	}
	if (ttime > 0) {
		pdate();
		printf("\ttotal");
		phours(ttime);
		putchar('\n');
	}
}

static void
upall(f)
int f;
{
	register struct tbuf *tp;

	for (tp=tbuf; tp < &tbuf[TSIZE]; tp++)
		update(tp, f);
}

static void
update(tp, f)
struct tbuf *tp;
int f;
{
	int j;
	struct ubuf *up;
	long t, t1;

	if (f)
		t = midnight;
	else
		t = ibuf.ut_time;
	if (tp->userp) {
		t1 = t - tp->ttime;
		if (t1>0 && t1 < day+day/2)
			tp->userp->utime += t1;
	}
	tp->ttime = t;
	if (f)
		return;
	if (ibuf.ut_name[0]=='\0') {
		tp->userp = 0;
		return;
	}
	for (up=ubuf; up < &ubuf[USIZE]; up++) {
		if (up->uname[0] == '\0')
			break;
		for (j=0; j<8 && up->uname[j]==ibuf.ut_name[j]; j++);
		if (j>=8)
			break;
	}
	for (j=0; j<8; j++)
		up->uname[j] = ibuf.ut_name[j];
	tp->userp = up;
}

static int
among(i)
int i;
{
	register int j, k;
	register char *p;

	if (pcount==0)
		return(1);
	for (j=0; j<pcount; j++) {
		p = pptr[j];
		for (k=0; k<8; k++) {
			if (*p == ubuf[i].uname[k]) {
				if (*p++ == '\0')
					return(1);
			} else
				break;
		}
	}
	return(0);
}

static void
newday(void)
{
	long ttime;
	struct timeb tb;

	time(&ttime);
	if (midnight == 0) {
		ftime(&tb);
		midnight = 60*(long)tb.timezone;
		if (localtime(&ttime)->tm_isdst)
			midnight -= 3600;
	}
	while (midnight <= ibuf.ut_time)
		midnight += day;
}

static void
puser(name, secs)
char *name;
long secs;
{
	register int i;

	putchar('\t');
	for (i=0; i<8 && name[i]; i++)
		putchar(name[i]);
	while (i++ < 8)
		putchar(' ');
	phours(secs);
	putchar('\n');
}

static void
phours(secs)
long secs;
{
	long h;

	h = (secs * 100 + 1800) / 3600;
	printf("%6ld.%02ld", h / 100, h % 100);
}

static void
pdate(void)
{
	long x;
	register char *cp;
	register int i;

	if (byday==0)
		return;
	x = midnight-1;
	cp = ctime(&x)+4;
	for (i=0; i<6 && cp[i]; i++)
		putchar(cp[i]);
}
