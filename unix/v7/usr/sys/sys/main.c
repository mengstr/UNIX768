#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/filsys.h"
#include "../h/mount.h"
#include "../h/map.h"
#include "../h/proc.h"
#include "../h/inode.h"
#include "../h/seg.h"
#include "../h/conf.h"
#include "../h/buf.h"

#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_BOOT_FLAGS_REG 3
#define EPOCH68_BOOT_FLAG_MULTIUSER 1
#define EPOCH68_TIMER_VECTOR_ADDR 0x00000078L
#define EPOCH68_UART_VECTOR_ADDR 0x00000074L
#define EPOCH68_KMETA_BASE 0x00c00000L
#define EPOCH68_KMETA_MAGIC 0x6b6d6574UL
#define INIT_STACK_TOP 0x0003fffeL

void iinit(void);
void binit(void);
i32 newproc(struct proc *parentp);
void sched(void);
void epoch68_enter_user(void *pc, u32 usp, u16 sr);
void epoch68_start_on_stack(void *new_sp, i32 (*entry)(void));
extern i32 syscall_trap0(void);
extern i32 epoch68_bus_error(void);
extern i32 epoch68_address_error(void);
extern i32 clock_intr(void);
extern i32 uart_intr(void);
extern unsigned short icode[];
extern i32 szicode;
extern unsigned short icode_multi[];
extern i32 szicode_multi;
extern u32 ukstack[NPROC][EPOCH68_KSTACK_LONGS];
void epoch68_uart_kernel_init(void);
void epoch68_uart_enable_rx_irqs(void);
void epoch68_init_kernel_stack_guards(void);
i32 mm58167_time(time_t *tp);

static unsigned short *epoch68_initcode;
static i32 epoch68_initcodesz;

static void
epoch68_kmeta_probe(void)
{
	volatile unsigned long *p;

	p = (volatile unsigned long *)EPOCH68_KMETA_BASE;
	*p = EPOCH68_KMETA_MAGIC;
	if (*p != EPOCH68_KMETA_MAGIC)
		panic("kmeta");
}

static void
epoch68_kfixed_clear(void)
{
	register char *p;
	register char *end;
	unsigned long bytes;

	bytes = (sizeof(struct user) * (unsigned long)NPROC) +
	    (sizeof(unsigned long) * (unsigned long)NPROC *
	    (unsigned long)EPOCH68_KSTACK_LONGS) +
	    (sizeof(struct proc) * (unsigned long)NPROC);
	p = (char *)EPOCH68_KMETA_BASE;
	end = p + bytes;
	while(p < end)
		*p++ = 0;
}

static void
epoch68_proc0_sched(void)
{
	for (;;)
		sched();
}

static i32 epoch68_main(void);
static void epoch68_init_process(void);

/*
 * Initialization code.
 * Called from cold start routine as
 * soon as a stack and segmentation
 * have been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *
 * loop at low address in user mode -- /etc/init
 *	cannot be executed.
 */
i32
main(void)
{
	*(long *)0x00000080 = (long)syscall_trap0;
	*(long *)0x00000008 = (long)epoch68_bus_error;
	*(long *)0x0000000c = (long)epoch68_address_error;
	*(long *)EPOCH68_TIMER_VECTOR_ADDR = (long)clock_intr;
	*(long *)EPOCH68_UART_VECTOR_ADDR = (long)uart_intr;
	epoch68_kmeta_probe();
	epoch68_kfixed_clear();
	epoch68_init_kernel_stack_guards();
	epoch68_start_on_stack((void *)(ukstack[0] + EPOCH68_KSTACK_LONGS),
	    epoch68_main);
	return 0;
}

static i32
epoch68_main()
{
	if (EPOCH68_MMIO_BASE[EPOCH68_BOOT_FLAGS_REG] & EPOCH68_BOOT_FLAG_MULTIUSER) {
		epoch68_initcode = icode_multi;
		epoch68_initcodesz = szicode_multi;
	} else {
		epoch68_initcode = icode;
		epoch68_initcodesz = szicode;
	}

	startup();
	userpage_init();
	/*
	* set up system process
	*/
	proc[0].p_addr = 0;
	proc[0].p_size = USIZE;
	proc[0].p_stat = SRUN;
	proc[0].p_flag |= SLOAD|SSYS;
	proc[0].p_nice = NZERO;
	uarrp = &uarea[0]; /* point u to the first u-area*/
	u.u_procp = &proc[0];
	u.u_cmask = CMASK;

	/*
	 * Initialize devices and
	 * set up 'known' i-nodes.
	 */
	epoch68_uart_kernel_init();
	clkstart();
	cinit();
	binit();
	iinit();
	rootdir = iget(rootdev, (ino_t)ROOTINO);
	rootdir->i_flag &= ~ILOCK;
	u.u_cdir = iget(rootdev, (ino_t)ROOTINO);
	u.u_cdir->i_flag &= ~ILOCK;
	u.u_rdir = NULL;

	/*
	 * Make init process and enter the scheduling loop
	 * with the system process.
	 */
	u.u_rsav[0] = (unsigned long)epoch68_proc0_sched;
	u.u_rsav[1] = (unsigned long)(ukstack[0] + EPOCH68_KSTACK_LONGS);
	if (newproc(&proc[0]) || u.u_error)
		panic("init fork");
	epoch68_set_proc_entry(&proc[1], epoch68_init_process);
	sched();
	return 0;
}

static void
epoch68_init_process()
{
	expand(USIZE + (i32)btoc(epoch68_initcodesz));
	if (u.u_error)
		panic("init expand");
	if (estabur((unsigned)0, btoc(epoch68_initcodesz),
	    (unsigned)0, 0, RO) < 0)
		panic("init estabur");
	if (copyout((caddr_t)epoch68_initcode, (caddr_t)0,
	    epoch68_initcodesz) < 0)
		panic("init icode");
	epoch68_uart_enable_rx_irqs();
	epoch68_enter_user((void *)0, INIT_STACK_TOP, 0);
	panic("init return");
}

/*
 * iinit is called once (from main)
 * very early in initialization.
 * It reads the root's super block
 * and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super
 * block. Usually because of an IO error.
 */
void iinit()
{
	register struct buf *cp, *bp;
	register struct filsys *fp;
	time_t rtctime;

	(*bdevsw[major(rootdev)].d_open)(rootdev, 1);
	bp = bread(rootdev, SUPERB);
	cp = geteblk();
	if(u.u_error)
		panic("iinit");
	bcopy(bp->b_un.b_addr, cp->b_un.b_addr, sizeof(struct filsys));
	brelse(bp);
	mount[0].m_bufp = cp;
	mount[0].m_dev = rootdev;
	fp = cp->b_un.b_filsys;
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_ronly = 0;
	time = fp->s_time;
	if (mm58167_time(&rtctime) == 0)
		time = rtctime;
}

/*
 * This is the set of buffers proper, whose heads
 * were declared in buf.h.  There can exist buffer
 * headers not pointing here that are used purely
 * as arguments to the I/O routines to describe
 * I/O to be done-- e.g. swbuf for
 * swapping.
 */
char	buffers[NBUF][BSIZE+BSLOP];

/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device buffer lists to empty.
 */
void binit()
{
	register struct buf *bp;
	register struct buf *dp;
	register i32 i;
	struct bdevsw *bdp;

	bfreelist.b_forw = bfreelist.b_back =
	    bfreelist.av_forw = bfreelist.av_back = &bfreelist;
	for (i=0; i<NBUF; i++) {
		bp = &buf[i];
		bp->b_dev = NODEV;
		bp->b_un.b_addr = buffers[i];
		bp->b_back = &bfreelist;
		bp->b_forw = bfreelist.b_forw;
		bfreelist.b_forw->b_back = bp;
		bfreelist.b_forw = bp;
		bp->b_flags = B_BUSY;
		brelse(bp);
	}
	for (bdp = bdevsw; bdp->d_open; bdp++) {
		dp = bdp->d_tab;
		if(dp) {
			dp->b_forw = dp;
			dp->b_back = dp;
		}
		nblkdev++;
	}
}
