/*
 * Epoch68 MM58167-compatible real-time clock driver.
 *
 * The emulator exposes the calendar registers as BCD digit registers in UTC.
 * The level-6 periodic timer remains the UNIX tick source.
 */

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"

#define MM58167_BASE ((volatile unsigned char *)0x00a00040)
#define MM58167_NREG 16

#define RTC_SEC_UNITS 0
#define RTC_SEC_TENS 1
#define RTC_MIN_UNITS 2
#define RTC_MIN_TENS 3
#define RTC_HOUR_UNITS 4
#define RTC_HOUR_TENS 5
#define RTC_DAY_UNITS 7
#define RTC_DAY_TENS 8
#define RTC_MONTH_UNITS 9
#define RTC_MONTH_TENS 10
#define RTC_YEAR_UNITS 11
#define RTC_YEAR_TENS 12

static i16 mdays[] = {
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31
};

i32 passc(i32 c);

static i32
rtcbcd(i16 units, i16 tens)
{
	register i16 u, t;

	u = MM58167_BASE[units] & 0xf;
	t = MM58167_BASE[tens] & 0xf;
	if (u > 9 || t > 9)
		return(-1);
	return((t * 10) + u);
}

static i32
rtcleap(i32 year)
{
	return((year % 4) == 0);
}

static void
rtcputbcd(i16 units, i16 tens, i16 value)
{
	MM58167_BASE[units] = value % 10;
	MM58167_BASE[tens] = (value / 10) % 10;
}

static i32
rtcdays_before_year(i32 year)
{
	register i32 days;
	register i32 y;

	days = 0;
	for (y = 1970; y < year; y++)
		days += rtcleap(y) ? 366 : 365;
	return(days);
}

i32
mm58167_time(time_t *tp)
{
	register i16 sec, min, hour, day, month;
	register i32 year;
	register i16 i;
	i32 days;

	sec = rtcbcd(RTC_SEC_UNITS, RTC_SEC_TENS);
	min = rtcbcd(RTC_MIN_UNITS, RTC_MIN_TENS);
	hour = rtcbcd(RTC_HOUR_UNITS, RTC_HOUR_TENS);
	day = rtcbcd(RTC_DAY_UNITS, RTC_DAY_TENS);
	month = rtcbcd(RTC_MONTH_UNITS, RTC_MONTH_TENS);
	year = rtcbcd(RTC_YEAR_UNITS, RTC_YEAR_TENS);
	if (sec < 0 || sec > 59 || min < 0 || min > 59 ||
	    hour < 0 || hour > 23 || month < 1 || month > 12)
		return(-1);

	if (year < 70)
		year += 2000;
	else
		year += 1900;
	if (year < 1970)
		return(-1);

	if (day < 1)
		return(-1);
	i = mdays[month - 1];
	if (month == 2 && rtcleap(year))
		i++;
	if (day > i)
		return(-1);

	days = rtcdays_before_year(year);
	for (i = 1; i < month; i++) {
		days += mdays[i - 1];
		if (i == 2 && rtcleap(year))
			days++;
	}
	days += day - 1;
	*tp = (((days * 24L) + hour) * 60L + min) * 60L + sec;
	return(0);
}

i32
mm58167_set_time(time_t t)
{
	register i32 year;
	register i16 month;
	i32 days;
	register i32 hms;

	if (t < 0)
		return(-1);
	hms = t % 86400L;
	days = t / 86400L;
	rtcputbcd(RTC_SEC_UNITS, RTC_SEC_TENS, (i16)(hms % 60));
	hms /= 60;
	rtcputbcd(RTC_MIN_UNITS, RTC_MIN_TENS, (i16)(hms % 60));
	hms /= 60;
	rtcputbcd(RTC_HOUR_UNITS, RTC_HOUR_TENS, (i16)hms);

	for (year = 1970; days >= (rtcleap(year) ? 366 : 365); year++)
		days -= rtcleap(year) ? 366 : 365;
	for (month = 1; month <= 12; month++) {
			register i16 dim;

		dim = mdays[month - 1];
		if (month == 2 && rtcleap(year))
			dim++;
		if (days < dim)
			break;
		days -= dim;
	}
	rtcputbcd(RTC_DAY_UNITS, RTC_DAY_TENS, (i16)days + 1);
	rtcputbcd(RTC_MONTH_UNITS, RTC_MONTH_TENS, month);
	rtcputbcd(RTC_YEAR_UNITS, RTC_YEAR_TENS, year % 100);
	return(0);
}

i32
rtcopen(i32 dev, i32 flag)
{
	if (minor(dev) != 0) {
		u.u_error = ENXIO;
		return(-1);
	}
	return(0);
}

i32
rtcread(i32 dev)
{
	register i16 i;

	if (minor(dev) != 0) {
		u.u_error = ENXIO;
		return(-1);
	}
	for (i = 0; i < MM58167_NREG && u.u_count != 0; i++)
		if (passc(MM58167_BASE[i] & 0xff) < 0)
			break;
	return(0);
}

i32
rtcwrite(i32 dev)
{
	u.u_error = EINVAL;
	return(-1);
}
