#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
	errno = 0;
	if (argc > 1)
		acct(argv[1]);
	else
		acct((char *)0);
	if (errno) {
		perror("accton");
		exit(1);
	}
	exit(0);
}
