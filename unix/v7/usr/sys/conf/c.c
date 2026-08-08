#include "../h/param.h"
#include "../h/systm.h"
#include "../h/buf.h"
#include "../h/tty.h"
#include "../h/conf.h"
#include "../h/proc.h"
#include "../h/text.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/file.h"
#include "../h/inode.h"
#include "../h/acct.h"

i32	nulldev_open(i32, i32), nulldev_close(i32, i32), nulldev_strategy(struct buf *);
i32	nulldev_rw(i32), nulldev_ioctl(i32, i32, caddr_t, i32);
i32	nulldev_stop(struct tty *), nulldev_lclose(struct tty *);
i32	nodev_open(i32, i32), nodev_close(i32, i32), nodev_strategy(struct buf *);
i32	nodev_rw(i32), nodev_ioctl(i32, i32, caddr_t, i32), nodev_stop(struct tty *);
i32	nodev_lopen(i32, struct tty *, caddr_t), nodev_lclose(struct tty *);
i32	nodev_lread(struct tty *), nodev_lioctl(i32, struct tty *, caddr_t);
i32	nodev_lrint(i32, struct tty *), nodev_lstart(struct tty *);
i32	nodev_lmodem(struct tty *, i32);
caddr_t	nodev_lwrite(struct tty *);
i32	rdopen(i32, i32), rdstrategy(struct buf *);
i32	rdread(i32), rdwrite(i32);
extern struct	buf	rdtab;
i32	sdopen(i32, i32), sdstrategy(struct buf *);
extern struct	buf	sdtab;
i32	htopen(i32, i32), htclose(i32, i32), htstrategy(struct buf *);
struct	buf	httab;
	struct	bdevsw	bdevsw[] =
{
	rdopen, nulldev_close, rdstrategy, &rdtab,	/* rd = 0 */
	sdopen, nulldev_close, sdstrategy, &sdtab,	/* sd = 1 */
	nodev_open, nodev_close, nodev_strategy, 0, /* rf = 2 */
	nodev_open, nodev_close, nodev_strategy, 0, /* tm = 3 */
	nodev_open, nodev_close, nodev_strategy, 0, /* tc = 4 */
	nodev_open, nodev_close, nodev_strategy, 0, /* hs = 5 */
	nodev_open, nodev_close, nodev_strategy, 0, /* hp = 6 */
	htopen, htclose, htstrategy, &httab,	/* ht = 7 */
	nodev_open, nodev_close, nodev_strategy, 0, /* rl = 8 */
	0
};

i32	klopen(i32, i32), klclose(i32, i32), klread(i32), klwrite(i32);
i32	klioctl(i32, i32, caddr_t, i32);
extern	struct tty epoch68_kl_tty[];
i32	mmread(i32), mmwrite(i32);
i32	rtcopen(i32, i32), rtcread(i32), rtcwrite(i32);
i32	htread(i32), htwrite(i32);
i32	syopen(i32, i32), syread(i32), sywrite(i32);
i32	sysioctl(i32, i32, caddr_t, i32);

	struct	cdevsw	cdevsw[] =
{
	klopen, klclose, klread, klwrite, klioctl, nulldev_stop, epoch68_kl_tty,	/* console = 0 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* pc = 1 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* lp = 2 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* dc = 3 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* dh = 4 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* dp = 5 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* dj = 6 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* dn = 7 */
	nulldev_open, nulldev_close, mmread, mmwrite, nodev_ioctl, nulldev_stop, 0, 	/* mem = 8 */
	rdopen, nulldev_close, rdread, rdwrite, nodev_ioctl, nulldev_stop, 0,	/* rd = 9 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* rf = 10 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* rp = 11 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* tm = 12 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* hs = 13 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* hp = 14 */
	htopen, htclose, htread, htwrite, nodev_ioctl, nulldev_stop, 0,	/* ht = 15 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* du = 16 */
	syopen, nulldev_close, syread, sywrite, sysioctl, nulldev_stop, 0,	/* tty = 17 */
	nodev_open, nodev_close, nodev_rw, nodev_rw, nodev_ioctl, nulldev_stop, 0, /* rl = 18 */
	rtcopen, nulldev_close, rtcread, rtcwrite, nodev_ioctl, nulldev_stop, 0, /* rtc = 19 */
	0
};

i32	ttyopen(i32, struct tty *, caddr_t), ttyclose(struct tty *);
i32	ttread(struct tty *);
caddr_t	ttwrite(struct tty *);
i32	ttyinput(i32, struct tty *);
i32	ttstart(struct tty *);
	struct	linesw	linesw[] =
{
	ttyopen, nulldev_lclose, ttread, ttwrite, nodev_lioctl,
	ttyinput, nodev_lstart, nodev_lstart, ttstart, nodev_lmodem, /* 0 */
	0
};
dev_t	rootdev	= makedev(0, 0);
dev_t	swapdev	= makedev(0, 0);
dev_t	pipedev = makedev(0, 0);
i32	nldisp = 1;
daddr_t	swplo	= 4000;
i32	nswap	= 872;
	
struct	buf	buf[NBUF];
struct	file	file[NFILE];
struct	inode	inode[NINODE];
i32	mpxchan(void);
i32	(*ldmpx)(void) = mpxchan;
extern struct	proc	proc[NPROC];
char epoch68_proc_size_check[(((long)sizeof(struct proc) * (long)NPROC) == 2268L) ? 1 : -1];
struct	text	text[NTEXT];
struct	buf	bfreelist;
struct	acct	acctbuf;
struct	inode	*acctp;
