#include "types.h"

struct tm {
	i32 tm_sec;
	i32 tm_min;
	i32 tm_hour;
	i32 tm_mday;
	i32 tm_mon;
	i32 tm_year;
	i32 tm_wday;
	i32 tm_yday;
	i32 tm_isdst;
};

struct timeb {
	time32 time;
	u16 millitm;
	i16 timezone;
	i16 dstflag;
};

struct zone {
	i16 offset;
	char *stdzone;
	char *dlzone;
};

extern i32 ftime(struct timeb *tp);

static char cbuf[26];
static i16 dmsize[12] = {
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31
};

static struct {
	i16 daylb;
	i16 dayle;
} daytab[] = {
	5, 333,
	58, 303,
};

static struct zone zonetab[] = {
	4 * 60, "AST", "ADT",
	5 * 60, "EST", "EDT",
	6 * 60, "CST", "CDT",
	7 * 60, "MST", "MDT",
	8 * 60, "PST", "PDT",
	0, "GMT", 0,
	-1, 0, 0
};

static char *ct_numb(char *cp, i32 n);
static i32 sunday(struct tm *t, i32 d);
char *asctime(struct tm *t);
struct tm *localtime(time32 *tim);
struct tm *gmtime(time32 *tim);

i32
dysize(i32 y)
{
	if ((y % 4) == 0)
		return 366;
	return 365;
}

char *
ctime(time32 *t)
{
	return asctime(localtime(t));
}

struct tm *
localtime(time32 *tim)
{
	register i32 dayno;
	register struct tm *ct;
	register i32 daylbegin;
	register i32 daylend;
	time32 copyt;
	struct timeb systime;

	ftime(&systime);
	copyt = *tim - (time32)systime.timezone * 60;
	ct = gmtime(&copyt);
	dayno = ct->tm_yday;
	daylbegin = 119;
	daylend = 303;
	if (ct->tm_year == 74 || ct->tm_year == 75) {
		daylbegin = daytab[ct->tm_year - 74].daylb;
		daylend = daytab[ct->tm_year - 74].dayle;
	}
	daylbegin = sunday(ct, daylbegin);
	daylend = sunday(ct, daylend);
	if (systime.dstflag &&
	    (dayno > daylbegin || (dayno == daylbegin && ct->tm_hour >= 2)) &&
	    (dayno < daylend || (dayno == daylend && ct->tm_hour < 1))) {
		copyt += 60 * 60;
		ct = gmtime(&copyt);
		ct->tm_isdst++;
	}
	return ct;
}

static i32
sunday(struct tm *t, i32 d)
{
	if (d >= 58)
		d += dysize(t->tm_year) - 365;
	return d - (d - t->tm_yday + t->tm_wday + 700) % 7;
}

struct tm *
gmtime(time32 *tim)
{
	register i32 d0;
	register i32 d1;
	time32 hms;
	time32 day;
	register i32 *tp;
	static struct tm xtime;

	hms = *tim % 86400;
	day = *tim / 86400;
	if (hms < 0) {
		hms += 86400;
		day -= 1;
	}
	tp = (i32 *)&xtime;
	*tp++ = hms % 60;
	d1 = hms / 60;
	*tp++ = d1 % 60;
	d1 /= 60;
	*tp++ = d1;
	xtime.tm_wday = (day + 7340036) % 7;
	if (day >= 0) {
		for (d1 = 70; day >= dysize(d1); d1++)
			day -= dysize(d1);
	} else {
		for (d1 = 70; day < 0; d1--)
			day += dysize(d1 - 1);
	}
	xtime.tm_year = d1;
	xtime.tm_yday = d0 = day;
	if (dysize(d1) == 366)
		dmsize[1] = 29;
	for (d1 = 0; d0 >= dmsize[d1]; d1++)
		d0 -= dmsize[d1];
	dmsize[1] = 28;
	*tp++ = d0 + 1;
	*tp++ = d1;
	xtime.tm_isdst = 0;
	return &xtime;
}

char *
asctime(struct tm *t)
{
	register char *cp;
	register char *ncp;
	register i32 *tp;

	cp = cbuf;
	for (ncp = "Day Mon 00 00:00:00 1900\n"; (*cp++ = *ncp++) != 0;)
		;
	ncp = &"SunMonTueWedThuFriSat"[3 * t->tm_wday];
	cp = cbuf;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp++;
	tp = &t->tm_mon;
	ncp = &"JanFebMarAprMayJunJulAugSepOctNovDec"[(*tp) * 3];
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp = ct_numb(cp, *--tp);
	cp = ct_numb(cp, *--tp + 100);
	cp = ct_numb(cp, *--tp + 100);
	cp = ct_numb(cp, *--tp + 100);
	if (t->tm_year >= 100) {
		cp[1] = '2';
		cp[2] = '0';
	}
	cp += 2;
	cp = ct_numb(cp, t->tm_year + 100);
	return cbuf;
}

static char *
ct_numb(char *cp, i32 n)
{
	cp++;
	if (n >= 10)
		*cp++ = (n / 10) % 10 + '0';
	else
		*cp++ = ' ';
	*cp++ = n % 10 + '0';
	return cp;
}

char *
timezone(i32 zone, i32 dst)
{
	register struct zone *zp;
	static char czone[10];
	register char *p;
	i32 hours;
	i32 mins;

	for (zp = zonetab; zp->offset != -1; zp++) {
		if (zp->offset == zone) {
			if (dst && zp->dlzone)
				return zp->dlzone;
			if (!dst && zp->stdzone)
				return zp->stdzone;
		}
	}
	if (zone < 0) {
		zone = -zone;
		czone[3] = '+';
	} else {
		czone[3] = '-';
	}
	czone[0] = 'G';
	czone[1] = 'M';
	czone[2] = 'T';
	p = &czone[4];
	hours = zone / 60;
	mins = zone % 60;
	if (hours >= 10)
		*p++ = (hours / 10) + '0';
	*p++ = (hours % 10) + '0';
	*p++ = ':';
	*p++ = (mins / 10) + '0';
	*p++ = (mins % 10) + '0';
	*p = '\0';
	return czone;
}
