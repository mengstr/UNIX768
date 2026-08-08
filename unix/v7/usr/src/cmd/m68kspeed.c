/* Interactive MC68000 speed test. */

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>

static volatile i32 stopped;

void m68kspeed_chunk(void);
static void stop_test(i16 signo);

static void
stop_test(i16 signo)
{
	(void)signo;
	stopped = 1;
}

int
main(void)
{
	struct tms before;
	struct tms after;
	i32 ticks;
	i32 chunks;
	i32 mhz100;
	i32 mhz;
	i32 hundredths;
	i32 seconds;
	i32 sec_hundredths;
	printf("Press CTRL-C after about 10 seconds...\n");
	fflush(stdout);
	signal(SIGINT, stop_test);

	times(&before);
	chunks = 0;
	while (!stopped) {
		m68kspeed_chunk();
		chunks++;
	}
	times(&after);

	ticks = after.tms_utime - before.tms_utime;
	if (ticks <= 0) {
		fprintf(stderr, "m68kspeed: test was too short\n");
		return(1);
	}
	mhz100 = (chunks * 6000 + ticks / 2) / ticks;
	mhz = mhz100 / 100;
	hundredths = mhz100 % 100;
	seconds = ticks / 60;
	sec_hundredths = ((ticks % 60) * 100 + 30) / 60;
	if (sec_hundredths >= 100) {
		seconds++;
		sec_hundredths -= 100;
	}
	printf("%ld chunks in %ld.%02ld seconds: %ld.%02ld MHz\n",
	    chunks, seconds, sec_hundredths, mhz, hundredths);
	return(0);
}
