/*
 * date : print date
 * date YYYYMMDDHHMM : set date, if allowed
 * date YYMMDDHHMM[.SS] : legacy set-date form
 * date -u ... : date in GMT
 */
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <utmp.h>
#include <unistd.h>
long	timbuf;
char	*ap, *ep, *sp;
int	uflag;

char	*timezone(i32 zone, i32 dst);
static	int	dmsize[12] =
{
	31,
	28,
	31,
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31
};

struct utmp wtmp[2] = { {"|", "", 0}, {"{", "", 0}};

char	*ctime(long *clock);
char	*asctime(struct tm *tm);
struct	tm *localtime(long *clock);
struct	tm *gmtime(long *clock);
static i32 gtime(void);
static i32 gp(i32 dfault);
i32 dysize(i32 year);

int
main(int argc, char *argv[])
{
	register char *tzn;
	struct timeb info;
	int wf, rc;

	rc = 0;
	ftime(&info);
	if (argc>1 && argv[1][0]=='-' && argv[1][1]=='u') {
		argc--;
		argv++;
		uflag++;
	}
	if(argc > 1) {
		ap = argv[1];
		if (gtime()) {
			printf("date: bad conversion\n");
			exit(1);
		}
		/* convert to GMT assuming local time */
		if (uflag==0) {
			timbuf += (long)info.timezone*60;
			/* now fix up local daylight time */
			if(localtime(&timbuf)->tm_isdst)
				timbuf -= 60*60;
		}
		time(&wtmp[0].ut_time);
		if(stime(&timbuf) < 0) {
			rc++;
			printf("date: no permission\n");
		} else if ((wf = open("/usr/adm/wtmp", 1)) >= 0) {
			time(&wtmp[1].ut_time);
			lseek(wf, 0L, 2);
			write(wf, (char *)wtmp, sizeof(wtmp));
			close(wf);
		}
	}
	if (rc==0)
		time(&timbuf);
	if(uflag) {
		ap = asctime(gmtime(&timbuf));
		tzn = "GMT";
	} else {
		struct tm *tp;
		tp = localtime(&timbuf);
		ap = asctime(tp);
		tzn = timezone(info.timezone, tp->tm_isdst);
	}
	printf("%.20s", ap);
	if (tzn)
		printf("%s", tzn);
	printf("%s", ap+19);
	exit(rc);
}

static i32
gtime(void)
{
	register int i, year, month;
	int day, hour, mins, secs;
	struct tm *L;
	char x;
	char *orig;
	int len;
	int all_digits;

	orig = ap;
	len = 0;
	while (orig[len])
		len++;
	all_digits = 1;
	for (i = 0; i < len; i++) {
		if (orig[i] < '0' || orig[i] > '9') {
			all_digits = 0;
			break;
		}
	}
	if (len == 12 && all_digits) {
		year = ((orig[0] - '0') * 1000) + ((orig[1] - '0') * 100) +
		    ((orig[2] - '0') * 10) + (orig[3] - '0');
		month = ((orig[4] - '0') * 10) + (orig[5] - '0');
		day = ((orig[6] - '0') * 10) + (orig[7] - '0');
		hour = ((orig[8] - '0') * 10) + (orig[9] - '0');
		mins = ((orig[10] - '0') * 10) + (orig[11] - '0');
		secs = 0;
		if (month < 1 || month > 12 ||
		    day < 1 || day > 31 ||
		    mins < 0 || mins > 59)
			return(1);
		if (hour == 24) {
			hour = 0;
			day++;
		}
		if (hour < 0 || hour > 23)
			return(1);
		timbuf = 0;
		for(i = 1970; i < year; i++)
			timbuf += dysize(i);
		if (dysize(year) == 366 && month >= 3)
			timbuf++;
		while(--month)
			timbuf += dmsize[month-1];
		timbuf += day-1;
		timbuf = 24*timbuf + hour;
		timbuf = 60*timbuf + mins;
		timbuf = 60*timbuf + secs;
		return(0);
	}

	ep=ap;
	while(*ep) ep++;
	sp=ap;
	while(sp<ep) {
		x = *sp;
		*sp++ = *--ep;
		*ep = x;
	}
	sp=ap;
	time(&timbuf);
	L=localtime(&timbuf);
	secs = gp(-1);
	if(*sp!='.') {
		mins=secs;
		secs=0;
	} else {sp++;
		mins = gp(-1);
	}
	hour = gp(-1);
	day = gp(L->tm_mday);
	month = gp(L->tm_mon+1);
	year = gp(L->tm_year);
	if(*sp)
		return(1);
	if( month<1 || month>12 ||
	    day<1 || day>31 ||
	    mins<0 || mins>59 ||
	    secs<0 || secs>59)
		return(1);
	if (hour==24) {
		hour=0; day++;
	}
	if (hour<0 || hour>23)
		return(1);
	timbuf = 0;
	if(year < 70)
		year += 2000;
	else
		year += 1900;
	for(i=1970; i<year; i++)
		timbuf += dysize(i);
	/* Leap year */
	if (dysize(year)==366 && month >= 3)
		timbuf++;
	while(--month)
		timbuf += dmsize[month-1];
	timbuf += day-1;
	timbuf = 24*timbuf + hour;
	timbuf = 60*timbuf + mins;
	timbuf = 60*timbuf + secs;
	return(0);

}

static i32
gp(i32 dfault)
{
	register int c, d;

	if(*sp==0)
		return(dfault);
	c = (*sp++)-'0';
	d = (*sp ? (*sp++)-'0' : 0);
	if(c<0 || c>9 || d<0 || d>9)
		return(-1);
	return(c+10*d);
}
