/*
 * Update the file system every 30 seconds.
 * For cache benefit, open certain system directories.
 */

#include <signal.h>
#include <unistd.h>

char *fillst[] = {
	"/bin",
	"/usr",
	"/usr/bin",
	0,
};

static void dosync(void);

int
main(int argc, char **argv)
{
	char **f;

	(void)argc;
	(void)argv;

	if(fork())
		exit(0);
	close(0);
	close(1);
	close(2);
	for(f = fillst; *f; f++)
		open(*f, 0);
	dosync();
	for(;;)
		pause();
}

static void
dosync()
{
	sync();
	signal(SIGALRM, (sighandler_t)dosync);
	alarm(30);
}
