#include "../h/param.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/conf.h"
#include "../h/tty.h"
#include "../h/proc.h"

struct chan;

/*
 * Focused Epoch68 kernel-link stubs.
 *
 * These satisfy switch-table and helper references from conf/c.c while the
 * corresponding drivers and line disciplines are not yet part of the focused
 * porting build.
 */

static i32
notready(void)
{
	u.u_error = ENODEV;
	return(-1);
}

i32
nodev_open(i32 dev, i32 flag)
{
	return(notready());
}

i32
nodev_close(i32 dev, i32 flag)
{
	return(notready());
}

i32
nodev_strategy(struct buf *bp)
{
	return(notready());
}

i32
nodev_rw(i32 dev)
{
	return(notready());
}

i32
nodev_ioctl(i32 dev, i32 cmd, caddr_t addr, i32 flag)
{
	return(notready());
}

i32
nodev_stop(struct tty *tp)
{
	return(notready());
}

i32
nodev_lopen(i32 dev, struct tty *tp, caddr_t addr)
{
	return(notready());
}

i32
nodev_lclose(struct tty *tp)
{
	return(notready());
}

i32
nodev_lread(struct tty *tp)
{
	return(notready());
}

caddr_t
nodev_lwrite(struct tty *tp)
{
	notready();
	return(NULL);
}

i32
nodev_lioctl(i32 com, struct tty *tp, caddr_t addr)
{
	return(notready());
}

i32
nodev_lrint(i32 c, struct tty *tp)
{
	return(notready());
}

i32
nodev_lstart(struct tty *tp)
{
	return(notready());
}

i32
nodev_lmodem(struct tty *tp, i32 flag)
{
	return(notready());
}

i32
nulldev_open(i32 dev, i32 flag)
{
	return(0);
}

i32
nulldev_close(i32 dev, i32 flag)
{
	return(0);
}

i32
nulldev_strategy(struct buf *bp)
{
	return(0);
}

i32
nulldev_rw(i32 dev)
{
	return(0);
}

i32
nulldev_ioctl(i32 dev, i32 cmd, caddr_t addr, i32 flag)
{
	return(0);
}

i32
nulldev_stop(struct tty *tp)
{
	return(0);
}

i32
nulldev_lclose(struct tty *tp)
{
	return(0);
}

i32
rkstrategy(struct buf *bp)
{
	return(notready());
}

i32
rkread(i32 dev)
{
	return(notready());
}

i32
rkwrite(i32 dev)
{
	return(notready());
}

i32
htopen(i32 dev, i32 flag)
{
	return(notready());
}

i32
htclose(i32 dev, i32 flag)
{
	return(notready());
}

i32
htstrategy(struct buf *bp)
{
	return(notready());
}

i32
htread(i32 dev)
{
	return(notready());
}

i32
htwrite(i32 dev)
{
	return(notready());
}

i32
syopen(i32 dev, i32 flag)
{
	if (u.u_ttyp == NULL) {
		u.u_error = ENXIO;
		return(-1);
	}
	(*cdevsw[major(u.u_ttyd)].d_open)(u.u_ttyd, flag);
	return(u.u_error ? -1 : 0);
}

i32
syread(i32 dev)
{
	if (u.u_ttyp == NULL) {
		u.u_error = ENXIO;
		return(-1);
	}
	(*cdevsw[major(u.u_ttyd)].d_read)(u.u_ttyd);
	return(u.u_error ? -1 : 0);
}

i32
sywrite(i32 dev)
{
	if (u.u_ttyp == NULL) {
		u.u_error = ENXIO;
		return(-1);
	}
	(*cdevsw[major(u.u_ttyd)].d_write)(u.u_ttyd);
	return(u.u_error ? -1 : 0);
}

i32
sysioctl(i32 dev, i32 cmd, caddr_t addr, i32 flag)
{
	if (u.u_ttyp == NULL) {
		u.u_error = ENXIO;
		return(-1);
	}
	(*cdevsw[major(u.u_ttyd)].d_ioctl)(u.u_ttyd, cmd, addr, flag);
	return(u.u_error ? -1 : 0);
}

i32
mpxchan(void)
{
	return(0);
}

i32
sdata(struct chan *cp)
{
	return(0);
}

i32
scontrol(struct chan *chan, i32 s, i32 c)
{
	return(0);
}
