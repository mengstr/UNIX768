#include <signal.h>
#include <unistd.h>

int
system(char *s)
{
	i16 status;
	i32 pid, w;
	sighandler_t istat, qstat;

	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	if ((pid = fork()) == 0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execl("/bin/sh", "sh", "-c", s, (char *)0);
		_exit(127);
	}
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return status;
}
