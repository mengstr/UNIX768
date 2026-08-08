/* /usr/lib/calendar produces an egrep -f file
   that will select today's and tomorrow's
   calendar entries, with special weekend provisions

   used by calendar command
*/
#include <time.h>
#include <stdio.h>

#define DAY (3600*24L)

char *month[] = {
	"[Jj]an",
	"[Ff]eb",
	"[Mm]ar",
	"[Aa]pr",
	"[Mm]ay",
	"[Jj]un",
	"[Jj]ul",
	"[Aa]ug",
	"[Ss]ep",
	"[Oo]ct",
	"[Nn]ov",
	"[Dd]ec"
};
static void tprint(time_t t);

static void
tprint(time_t t)
{
	struct tm *tm;
	int year;

	tm = localtime(&t);
	year = (tm->tm_year + 1900) % 100;
	printf("(^|[ (,;])((%s[^ ]* *|%d/)0*%d(/(20%02d|%02d))?)([^0123456789]|$)\n",
		month[tm->tm_mon], tm->tm_mon + 1, tm->tm_mday, year, year);
}

int
main(void)
{
	time_t t;
	time(&t);
	tprint(t);
	switch(localtime(&t)->tm_wday) {
	case 5:
		t += DAY;
		tprint(t);
	case 6:
		t += DAY;
		tprint(t);
	default:
		t += DAY;
		tprint(t);
	}
}
