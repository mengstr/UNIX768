#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/text.h"
#include "../h/file.h"
#include "../h/inode.h"
#include "../h/reg.h"
#include "../h/acct.h"

#pragma dontwarn 213

#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_SYSTEM_PAGE_REG 0
#define EPOCH68_MIRROR1_PAGE_REG 1
#define EPOCH68_MIRROR2_PAGE_REG 2
#define EPOCH68_USER_COPY_SRC 0x00200000L
#define EPOCH68_USER_COPY_DST 0x00400000L
#define EPOCH68_USER_PAGE_SIZE (256UL * 1024UL)
#define EPOCH68_KERNEL_METADATA_PAGE 63
#define EPOCH68_KSTACK_GUARD_WORDS 8
#define EPOCH68_KSTACK_GUARD_PATTERN 0x6b737461UL

void sleep(char *chan, i32 pri);
void wakeup(char *chan);
void setrq(struct proc *p);
void sched(void);
void setrun(struct proc *p);
void swtch(void);
void expand(i32 newsize);
extern u32 ukstack[NPROC][EPOCH68_KSTACK_LONGS];
extern void epoch68_resume_irq_frame(void);
extern void epoch68_set_resume_usp(struct proc *p);
extern u32 epoch68_fork_regs[15];
extern i32 epoch68_fork_regs_valid;
extern void epoch68_uart_poll_rx(void);

static struct user *uarea_for_proc(struct proc *p);
static void epoch68_init_kernel_stack_guard_slot(i32 slot);
static void epoch68_check_kernel_stack_guard_slot(i32 slot, char *where);
static void epoch68_copy_user_page(i32 src_page, i32 dst_page);
static void epoch68_copy_kernel_stack(struct proc *parentp, struct proc *childp);
static void epoch68_setup_child_irq_frame(struct proc *childp);

#define SQSIZE 0100	/* Must be power of 2 */
#define HASH(x)	(((long)(x) >> 5) & (SQSIZE-1))
struct proc *slpque[SQSIZE];

static struct user *
uarea_for_proc(p)
register struct proc *p;
{
	return(&uarea[p - proc]);
}

static void
epoch68_init_kernel_stack_guard_slot(slot)
i32 slot;
{
	register i32 i;

	for (i = 0; i < EPOCH68_KSTACK_GUARD_WORDS; i++)
		ukstack[slot][i] = EPOCH68_KSTACK_GUARD_PATTERN;
}

void
epoch68_init_kernel_stack_guards(void)
{
	register i32 slot;

	for (slot = 0; slot < NPROC; slot++)
		epoch68_init_kernel_stack_guard_slot(slot);
}

static void
epoch68_check_kernel_stack_guard_slot(i32 slot, char *where)
{
	register i32 i;

	if (slot < 0 || slot >= NPROC)
		return;
	for (i = 0; i < EPOCH68_KSTACK_GUARD_WORDS; i++) {
		if (ukstack[slot][i] != EPOCH68_KSTACK_GUARD_PATTERN)
			panic("kstack");
	}
}

void
epoch68_check_current_kernel_stack(char *where)
{
	if (u.u_procp == 0)
		return;
	epoch68_check_kernel_stack_guard_slot((i32)(u.u_procp - proc), where);
}

/*
 * Start a newly created system process at a fresh kernel entry point.
 * User fork children resume through their copied trap frame instead.
 */
void
epoch68_set_proc_entry(p, entry)
register struct proc *p;
void (*entry)(void);
{
	register struct user *up;
	register i32 slot;
	register i32 i;

	slot = p - proc;
	if (slot <= 0 || slot >= NPROC)
		panic("proc entry");
	up = uarea_for_proc(p);
	up->u_rsav[0] = (u32)entry;
	up->u_rsav[1] = (u32)(ukstack[slot] + EPOCH68_KSTACK_LONGS);
	for (i = 2; i < 13; i++)
		up->u_rsav[i] = 0;
}

static void
epoch68_copy_user_page(src_page, dst_page)
i32 src_page;
i32 dst_page;
{
	register volatile unsigned char *mmio;
	register volatile u32 *src;
	register volatile u32 *dst;
	register u32 count;
	unsigned char old_m1, old_m2;

	mmio = EPOCH68_MMIO_BASE;
	old_m1 = mmio[EPOCH68_MIRROR1_PAGE_REG];
	old_m2 = mmio[EPOCH68_MIRROR2_PAGE_REG];
	mmio[EPOCH68_MIRROR1_PAGE_REG] = (unsigned char)src_page;
	mmio[EPOCH68_MIRROR2_PAGE_REG] = (unsigned char)dst_page;
	src = (volatile u32 *)EPOCH68_USER_COPY_SRC;
	dst = (volatile u32 *)EPOCH68_USER_COPY_DST;
	for (count = 0; count < EPOCH68_USER_PAGE_SIZE / sizeof(u32); count++)
		dst[count] = src[count];
	mmio[EPOCH68_MIRROR1_PAGE_REG] = old_m1;
	mmio[EPOCH68_MIRROR2_PAGE_REG] = old_m2;
}

static void
epoch68_copy_kernel_stack(parentp, childp)
register struct proc *parentp;
register struct proc *childp;
{
	register u32 *src;
	register u32 *dst;
	register i32 words;
	register struct user *childu;
	char *old_base;
	char *old_limit;
	long delta;

	epoch68_check_kernel_stack_guard_slot((i32)(parentp - proc), 0);
	src = ukstack[parentp - proc];
	dst = ukstack[childp - proc];
	words = EPOCH68_KSTACK_LONGS;
	while (words-- > 0)
		*dst++ = *src++;
	epoch68_init_kernel_stack_guard_slot((i32)(childp - proc));

	childu = uarea_for_proc(childp);
	delta = (char *)ukstack[childp - proc] - (char *)ukstack[parentp - proc];
	old_base = (char *)ukstack[parentp - proc];
	old_limit = old_base + sizeof(ukstack[0]);
	childu->u_ssav[1] += delta;
	childu->u_rsav[1] += delta;
	childu->u_qsav[1] += delta;
	if ((char *)childu->u_ar0 >= old_base && (char *)childu->u_ar0 < old_limit)
		childu->u_ar0 = (i32 *)((char *)childu->u_ar0 + delta);
	if ((char *)childu->u_ap >= old_base && (char *)childu->u_ap < old_limit)
		childu->u_ap = (i32 *)((char *)childu->u_ap + delta);
}

static void
epoch68_setup_child_irq_frame(childp)
register struct proc *childp;
{
	register struct user *childu;
	register char *top;
	register char *frame;
	register u32 *usp_slot;
	register u32 *reg_slot;
	u16 *sr_slot;
	u32 *pc_slot;
	register i32 n;

	childu = uarea_for_proc(childp);
	if (childu->u_ar0 == 0)
		return;
	top = (char *)ukstack[childp - proc] + sizeof(ukstack[0]);

	/*
	 * Build the exact frame that _clock_intr expects to restore:
	 *   +0   saved USP
	 *   +4   saved d0-d7/a0-a6 (15 longwords)
	 *   +64  saved SR
	 *   +66  saved PC
	 *
	 * The user return state comes from u_ar0, which trap0.c already seeded
	 * from the trapped fork() syscall frame.
	 */
	frame = top - (4 + (15 * 4) + 2 + 4);
	usp_slot = (u32 *)frame;
	reg_slot = (u32 *)(frame + 4);
	sr_slot = (u16 *)(frame + 64);
	pc_slot = (u32 *)(frame + 66);

	*usp_slot = (u32)childu->u_ar0[R6];
	if (epoch68_fork_regs_valid)
		for (n = 0; n < 15; n++)
			reg_slot[n] = epoch68_fork_regs[n];
	else
		for (n = 0; n < 15; n++)
			reg_slot[n] = 0;
	reg_slot[0] = 0; /* child fork() return value in d0 */
	*sr_slot = (u16)(childu->u_ar0[RPS] & ~0x2000);
	*pc_slot = (u32)childu->u_ar0[PC];

	childu->u_rsav[0] = (u32)epoch68_resume_irq_frame;
	childu->u_rsav[1] = (u32)frame;
}

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * The most important effect of pri is that when
 * pri<=PZERO a signal cannot disturb the sleep;
 * if pri>PZERO signals will be processed.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
void sleep(chan, pri)
caddr_t chan;
i32 pri;
{
	register struct proc *rp;
	register i32 s, h;

	rp = u.u_procp;
	s = spl6();
	if (chan==0)
		panic("zero wchan");
	rp->p_stat = SSLEEP;
	rp->p_wchan = chan;
	if (chan==0)
		panic("Sleeping on wchan 0");
	rp->p_pri = pri;
	h = HASH(chan);
	rp->p_link = slpque[h];
	slpque[h] = rp;
	if(pri > PZERO) {
		if(issig()) {
			rp->p_wchan = 0;
			rp->p_stat = SRUN;
			slpque[h] = rp->p_link;
			spl0();
			goto psig;
		}
		spl0();
		if(runin != 0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
		swtch();
		if(issig())
			goto psig;
	} else {
		spl0();
		swtch();
	}
	splx(s);
	return;

	/*
	 * If priority was low (>PZERO) and
	 * there has been a signal,
	 * execute non-local goto to
	 * the qsav location.
	 * (see trap1/trap.c)
	 */
psig:
	resume(u.u_procp->p_addr, uarea_for_proc(u.u_procp)->u_qsav);
}

/*
 * Wake up all processes sleeping on chan.
 */
void wakeup(chan)
register caddr_t chan;
{
	register struct proc *p, *q;
	register i32 i;
	i32 s;

	s = spl6();
	i = HASH(chan);
	p = slpque[i];
	q = NULL;
	while(p != NULL) {
		if(p->p_wchan==chan && p->p_stat!=SZOMB) {
			struct proc *sp;

			if (q == NULL)
				sp = slpque[i] = p->p_link;
			else
				sp = q->p_link = p->p_link;
			p->p_wchan = 0;
			setrun(p);
			p = sp;
			continue;
		}
		q = p;
		p = p->p_link;
	}
	splx(s);
}

/*
 * when you are sure that it
 * is impossible to get the
 * 'proc on q' diagnostic, the
 * diagnostic loop can be removed.
 */
void setrq(p)
struct proc *p;
{
	register struct proc *q;
	register i32 s;

	s = spl6();
	for(q=runq; q!=NULL; q=q->p_link)
		if(q == p) {
			printf("proc on q\n");
			goto out;
		}
	p->p_link = runq;
	runq = p;
out:
	splx(s);
}

/*
 * Set the process running;
 * arrange for it to be swapped in if necessary.
 */
void setrun(p)
register struct proc *p;
{
	register caddr_t w;

	if (p->p_stat==0 || p->p_stat==SZOMB)
		panic("Running a dead proc");
	/*
	 * The assignment to w is necessary because of
	 * race conditions. (Interrupt between test and use)
	 */
	if ((w = p->p_wchan) != 0) {
		wakeup(w);
		return;
	}
	p->p_stat = SRUN;
	setrq(p);
	if(p->p_pri < curpri)
		runrun++;
	if(runout != 0 && (p->p_flag&SLOAD) == 0) {
		runout = 0;
		wakeup((caddr_t)&runout);
	}
}

/*
 * Set user priority.
 * The rescheduling flag (runrun)
 * is set if the priority is better
 * than the currently running process.
 */
i32
setpri(pp)
register struct proc *pp;
{
	register i32 p;

	p = (pp->p_cpu & 0377)/16;
	p += PUSER + pp->p_nice - NZERO;
	if(p > 127)
		p = 127;
	if(p < curpri)
		runrun++;
	pp->p_pri = p;
	return(p);
}

/*
 * The main loop of process 0.  Epoch68 keeps every process resident in its
 * own user page, so there is no swapper work to perform here.  Polling keeps
 * the console usable until all supported UARTs provide receive interrupts.
 */
void sched()
{
	for (;;) {
		epoch68_uart_poll_rx();
		swtch();
	}
}

/*
 * put the current process on
 * the Q of running processes and
 * call the scheduler.
 */
void qswtch()
{
	setrq(u.u_procp);
	swtch();
}

/*
 * This routine is called to reschedule the CPU.
 * if the calling process is not in RUN state,
 * arrangements for it to restart must have
 * been made elsewhere, usually by calling via sleep.
 * Epoch68 switches explicitly between fixed u-areas and user pages.  When no
 * user process is runnable, control returns to process 0, whose sched() loop
 * polls the console and tries again.
 */
void swtch()
{
	register i32 n;
	register struct proc *p, *q, *pp, *pq;

	if (u.u_procp != &proc[0]) {
		if (save(u.u_rsav)) {
			sureg();
			return;
		}
	}
	spl6();
	runrun = 0;
	pp = NULL;
	q = NULL;
	n = 128;
	for(p=runq; p!=NULL; p=p->p_link) {
		if((p->p_stat==SRUN) && (p->p_flag&SLOAD)) {
			if(p->p_pri < n) {
				pp = p;
				pq = q;
				n = p->p_pri;
			}
		}
		q = p;
	}
	p = pp;
	if(p == NULL) {
		if (u.u_procp == &proc[0]) {
			spl0();
			return;
		}
		spl0();
		p = &proc[0];
		n = PUSER;
		goto found;
	}
	q = pq;
	if(q == NULL)
		runq = p->p_link;
	else
		q->p_link = p->p_link;
found:
	if (p != &proc[0] && (p->p_stat != SRUN ||
	    (p->p_flag&SLOAD) == 0 || p->p_addr <= 0 ||
	    p->p_addr >= EPOCH68_KERNEL_METADATA_PAGE))
		panic("bad runnable");
	curpri = n;
	n = p->p_flag&SSWAP;
	p->p_flag &= ~SSWAP;
	uarrp = uarea_for_proc(p);
	u.u_procp = p;
	EPOCH68_MMIO_BASE[EPOCH68_SYSTEM_PAGE_REG] = (unsigned char)p->p_addr;
	EPOCH68_MMIO_BASE[EPOCH68_MIRROR1_PAGE_REG] = (unsigned char)p->p_addr;
	epoch68_set_resume_usp(p);
	resume(p->p_addr, n? uarrp->u_ssav: uarrp->u_rsav);
}

/*
 * Create a new process-- the internal version of
 * sys fork.
 * Epoch68 constructs the child's saved trap frame directly and returns only
 * in the parent; the child later resumes from that frame with fork() == 0.
 */
i32
newproc(parentp)
struct proc *parentp;
{
	i32 a1;
	struct proc *p, *up;
	struct user *childu;
	struct user *parentu;
	i32 child_page;
	register struct proc *rpp, *rip;
	register i32 n;

	p = NULL;
retry:
	mpid++;
	if(mpid >= 30000) {
		mpid = 0;
		goto retry;
	}
	for(rpp = &proc[0]; rpp < &proc[NPROC]; rpp++) {
		if(rpp->p_stat == NULL && p==NULL)
			p = rpp;
		if (rpp->p_pid==mpid || rpp->p_pgrp==mpid)
			goto retry;
	}
	if ((rpp = p)==NULL)
		panic("no procs");

	if (parentp == 0)
		parentp = u.u_procp;
	rip = parentp;
	up = rip;
	rpp->p_stat = SIDL;
	rpp->p_clktim = 0;
	rpp->p_flag = SLOAD;
	rpp->p_uid = rip->p_uid;
	rpp->p_pgrp = rip->p_pgrp;
	rpp->p_nice = rip->p_nice;
	rpp->p_textp = rip->p_textp;
	rpp->p_pid = mpid;
	rpp->p_ppid = rip->p_pid;
	rpp->p_sig = 0;
	rpp->p_wchan = 0;
	rpp->p_link = 0;
	rpp->p_time = 0;
	rpp->p_cpu = 0;
	rpp->p_size = rip->p_size;

	rpp = p;
	rip = up;
	parentu = uarea_for_proc(rip);
	childu = uarea_for_proc(rpp);
	n = rip->p_size;
	a1 = rip->p_addr;
	rpp->p_size = n;
	child_page = userpage_alloc();
	if (child_page == 0) {
		rpp->p_flag = 0;
		rpp->p_pri = 0;
		rpp->p_time = 0;
		rpp->p_cpu = 0;
		rpp->p_nice = 0;
		rpp->p_sig = 0;
		rpp->p_uid = 0;
		rpp->p_pgrp = 0;
		rpp->p_pid = 0;
		rpp->p_ppid = 0;
		rpp->p_addr = 0;
		rpp->p_size = 0;
		rpp->p_wchan = NULL;
		rpp->p_textp = NULL;
		rpp->p_link = NULL;
		rpp->p_clktim = 0;
		rpp->p_stat = NULL;
		u.u_error = ENOMEM;
		return(0);
	}
	rpp->p_addr = (i16)child_page;

	/* Allocation must succeed before duplicating shared references. */
	for(n=0; n<NOFILE; n++)
		if(u.u_ofile[n] != NULL)
			u.u_ofile[n]->f_count++;
	if(up->p_textp != NULL) {
		up->p_textp->x_count++;
		up->p_textp->x_ccount++;
	}
	u.u_cdir->i_count++;
	if (u.u_rdir)
		u.u_rdir->i_count++;

	bcopy((caddr_t)parentu, (caddr_t)childu, sizeof(struct user));
	childu->u_procp = rpp;

	epoch68_copy_user_page(a1, rpp->p_addr);
	epoch68_copy_kernel_stack(rip, rpp);
	epoch68_setup_child_irq_frame(rpp);
	for(n = 0; n < 13; n++)
		childu->u_ssav[n] = childu->u_rsav[n];
	childu->u_r.r_reg.r_val1 = 0;
	childu->u_r.r_reg.r_val2 = rip->p_pid;
	childu->u_start = time;
	childu->u_cstime = 0;
	childu->u_stime = 0;
	childu->u_cutime = 0;
	childu->u_utime = 0;
	childu->u_acflag = AFORK;
	rpp->p_stat = SRUN;
	setrq(rpp);
	return(0);
}

/*
 * Change the accounted size of a process.  Every Epoch68 process already has
 * a fixed 256 KiB user window, so expansion only validates that the requested
 * text, data and stack fit in that window.
 */
void expand(newsize)
i32 newsize;
{
	register struct proc *p;
	i32 userclicks;

	p = u.u_procp;
	/*
	 * p_size keeps historical core accounting and includes USIZE, but the
	 * Epoch68 user window contains only user text/data/stack.
	 */
	userclicks = newsize - USIZE;
	if (newsize < USIZE || ctob((long)userclicks) > (256L * 1024L)) {
		u.u_error = ENOMEM;
		return;
	}
	p->p_size = newsize;
}
