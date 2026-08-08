#ifndef V7_SYS_SYSTM_H
#define V7_SYS_SYSTM_H

#include "param.h"

struct buf;
struct clist;
struct chan;
struct file;
struct filsys;
struct inode;
struct map;
struct proc;
struct text;

/*
 * Random set of variables
 * used by more than one
 * routine.
 */
extern char	canonb[CANBSIZ];	/* buffer for erase and kill (#@) */
extern struct inode *rootdir;		/* pointer to inode of root directory */
extern struct proc *runq;		/* head of linked list of running processes */
extern i32	cputype;		/* type of cpu =40, 45, or 70 */
extern i32	lbolt;			/* time of day in 60th not in time */
extern time_t	time;			/* time in sec from 1970 */

/*
 * Nblkdev is the number of entries
 * (rows) in the block switch. It is
 * set in binit/bio.c by making
 * a pass over the switch.
 * Used in bounds checking on major
 * device numbers.
 */
extern i32	nblkdev;

/*
 * Number of character switch entries.
 * Set by cinit/tty.c
 */
extern i32	nchrdev;

extern i32	mpid;			/* generic for unique process id's */
extern char	runin;			/* scheduling flag */
extern char	runout;			/* scheduling flag */
extern char	runrun;			/* scheduling flag */
extern char	curpri;			/* more scheduling */
extern i32	maxmem;			/* actual max memory per process */
extern physadr	lks;			/* pointer to clock device */
extern volatile u32 kticks;		/* raw periodic tick counter */
extern daddr_t	swplo;			/* block number of swap space */
extern i32	nswap;			/* size of swap space */
extern i32	updlock;		/* lock for sync */
extern daddr_t	rablock;		/* block to be read ahead */
extern	char	regloc[];	/* locs. of saved user registers (trap.c) */
extern char	msgbuf[MSGBUFS];	/* saved "printf" characters */
extern dev_t	rootdev;		/* device of the root */
extern dev_t	swapdev;		/* swapping device */
extern dev_t	pipedev;		/* pipe device */
dev_t getmdev(void);
daddr_t	bmap(struct inode *ip, daddr_t bn, i32 rwflg);
struct inode *ialloc(i32 dev);
struct inode *iget(i32 dev, i32 ino);
struct inode *owner(void);
struct inode *maknode(i32 mode);
struct inode *namei(i32 (*func)(void), i32 flag);
struct buf *alloc(i32 dev);
struct buf *getblk(i32 dev, daddr_t blkno);
struct buf *geteblk(void);
struct buf *bread(i32 dev, daddr_t blkno);
struct buf *breada(i32 dev, daddr_t blkno, daddr_t rablkno);
struct filsys *getfs(i32 dev);
struct file *getf(i32 f);
struct file *falloc(void);
i32	uchar(void);
i32	access(struct inode *ip, i32 mode);
i32	copyin(caddr_t from, caddr_t to, i32 count);
i32	copyout(caddr_t from, caddr_t to, i32 count);
i32	copyiin(caddr_t from, caddr_t to, i32 count);
i32	copyiout(caddr_t from, caddr_t to, i32 count);
i32	epoch68_heap_stack_would_collide(u32 heap_end, u32 usp, char *where);
void	epoch68_check_heap_stack(u32 usp, char *where);
void	epoch68_set_proc_entry(struct proc *p, void (*entry)(void));
i32	core(void);
i32	fubyte(caddr_t addr);
i32	fuibyte(caddr_t addr);
i32	fuiword(caddr_t addr);
i32	fuword(caddr_t addr);
i32	suword(caddr_t addr, i32 value);
i32	suiword(caddr_t addr, i32 value);
i32	malloc(struct map *mp, i32 size);
i32	userpage_alloc(void);
i32	newproc(struct proc *parentp);
i32	procxmt(void);
i32	save(u32 *lp);
i32	idle(void);
i32	estabur(u32 nt, u32 nd, u32 ns, i32 sep, i32 xrw);
i32	fsig(struct proc *p);
i32	issig(void);
i32	getxfile(struct inode *ip, i32 nargc);
i32	getc(struct clist *p);
i32	putc(i32 c, struct clist *p);
i32	b_to_q(char *cp, i32 cc, struct clist *q);
i32	q_to_b(struct clist *q, char *cp, i32 cc);
i32	passc(i32 c);
i32	cpass(void);
u32	max(u32, u32);
u32	min(u32, u32);
i32	sdata(struct chan *cp);
i32	scontrol(struct chan *chan, i32 s, i32 c);
i32	spl5(void);
i32	spl1(void);
i32	spl0(void);
i32	spl6(void);
i32	spl7(void);
i32	splx(i32 s);
i32	subyte(caddr_t addr, i32 value);
i32	suibyte(caddr_t addr, i32 value);
void	bcopy(char *from, char *to, i32 count);
void	bdwrite(struct buf *bp);
void	bflush(i32 dev);
void	brelse(struct buf *bp);
void	bwrite(struct buf *bp);
void	acct(void);
void	clearseg(i32 click);
void	cinit(void);
void	closef(struct file *fp);
void	clrbuf(struct buf *bp);
void	copyseg(i32 src_click, i32 dst_click);
void	clkstart(void);
void	expand(i32 newsize);
void	exit(i32 rv);
void	free(i32 dev, daddr_t bno);
void	ifree(i32 dev, i32 ino);
void	iodone(struct buf *bp);
void	ioctl(void);
void	iput(struct inode *ip);
void	itrunc(struct inode *ip);
void	iupdat(struct inode *ip, time_t *ta, time_t *tm);
void	mfree(struct map *mp, i32 size, i32 a);
void	userpage_free(i32 page);
void	userpage_init(void);
void	panic(char *s);
void	printf(char *fmt, ...);
void	plock(struct inode *ip);
void	prele(struct inode *ip);
void	prdev(char *str, i32 dev);
void	putchar(i32 c);
void	stty(void);
void	gtty(void);
void	psignal(struct proc *p, i32 sig);
void	qswtch(void);
void	readi(struct inode *ip);
void	rexit(void);
void	resume(i32 page, label_t lp);
void	savfp(void *fp);
void	sendsig(caddr_t p, i32 signo);
void	setrun(struct proc *p);
i32	setpri(struct proc *p);
void	signal(i32 pgrp, i32 sig);
void	sleep(char *chan, i32 pri);
void	startup(void);
void	sureg(void);
void	swtch(void);
void	swap(daddr_t blkno, i32 coreaddr, i32 count, i32 rdflg);
void	update(void);
void	wakeup(char *chan);
void	writei(struct inode *ip);
void	xfree(void);
void	xlock(struct text *xp);
void	xunlock(struct text *xp);
i32	badblock(struct filsys *fp, daddr_t bn, i32 dev);
i32	incore(i32 dev, daddr_t blkno);
i32	suser(void);
void	physio(i32 (*strat)(struct buf *), struct buf *bp, i32 dev, i32 rw);
/*
 * Instrumentation
 */
extern i32	dk_busy;
extern long	dk_time[32];
extern long	dk_numb[3];
extern long	dk_wds[3];
extern long	tk_nin;
extern long	tk_nout;

/*
 * Structure of the system-entry table
 */
extern struct sysent {
	char	sy_narg;		/* total number of arguments */
	char	sy_nrarg;		/* number of args in registers */
	i32	(*sy_call)(void);	/* handler */
} sysent[];

#endif /* V7_SYS_SYSTM_H */
