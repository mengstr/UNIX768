#ifndef V7_SYS_CONF_H
#define V7_SYS_CONF_H

struct buf;
struct tty;

/*
 * Declaration of block device
 * switch. Each entry (row) is
 * the only link between the
 * main unix code and the driver.
 * The initialization of the
 * device switches is in the
 * file conf.c.
 */
extern struct bdevsw
{
		i32	(*d_open)(i32, i32);
		i32	(*d_close)(i32, i32);
		i32	(*d_strategy)(struct buf *);
	struct buf *d_tab;
} bdevsw[];

/*
 * Character device switch.
 */
extern struct cdevsw
{
		i32	(*d_open)(i32, i32);
		i32	(*d_close)(i32, i32);
		i32	(*d_read)(i32);
		i32	(*d_write)(i32);
		i32	(*d_ioctl)(i32, i32, caddr_t, i32);
		i32	(*d_stop)(struct tty *);
	struct tty *d_ttys;
} cdevsw[];

/*
 * tty line control switch.
 */
extern struct linesw
{
		i32	(*l_open)(i32, struct tty *, caddr_t);
		i32	(*l_close)(struct tty *);
		i32	(*l_read)(struct tty *);
		caddr_t	(*l_write)(struct tty *);
		i32	(*l_ioctl)(i32, struct tty *, caddr_t);
		i32	(*l_rint)(i32, struct tty *);
		i32	(*l_rend)(struct tty *);
		i32	(*l_meta)(struct tty *);
		i32	(*l_start)(struct tty *);
		i32	(*l_modem)(struct tty *, i32);
} linesw[];

#endif /* V7_SYS_CONF_H */
