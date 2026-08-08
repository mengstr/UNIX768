#include <stdio.h>
#include <unistd.h>
#include <sys/inttypes.h>

int
main(void)
{
	printf("uid=%d gid=%d\n", (int)getuid(), (int)getgid());
	exit(0);
}
