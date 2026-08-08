#include <sgtty.h>
#include <unistd.h>

i32 isatty(i32 fd)
{
	struct sgttyb ttyb;

	return ioctl(fd, TIOCGETP, &ttyb) >= 0;
}
