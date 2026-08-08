/*
 * Writearound to old stty and gtty system calls
 */

#include <sgtty.h>

i32 ioctl(i32, i32, char *);

i32
stty(fd, ap)
i32 fd;
struct sgttyb *ap;
{
	return(ioctl(fd, TIOCSETP, (char *)ap));
}

i32
gtty(fd, ap)
i32 fd;
struct sgttyb *ap;
{
	return(ioctl(fd, TIOCGETP, (char *)ap));
}
