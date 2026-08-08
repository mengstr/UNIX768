#include "../h/param.h"
#include "../h/systm.h"
#include "../h/callo.h"
#include "../h/seg.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"

#define	SCHMAG	8/10
#define EPOCH68_POLL_UART_IN_CLOCK 1
#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_TIMER_CONTROL_REG 0x20
#define EPOCH68_TIMER_ENABLE 0x01

void epoch68_uart_poll_rx(void);
void psig(void);
i32 issig(void);

u32 epoch68_clock_return_pc;
u32 epoch68_clock_return_usp;
u16 epoch68_clock_return_sr;

/*
 * Epoch68 clock-port plan:
 *
 * - The PDP-11 line-clock restart and display-register calls are hardware
 *   specific.  The MMIO timer rearm below replaces the former; the latter has
 *   no Epoch68 device to drive.  Remove those dead historical blocks when the
 *   clock port is otherwise settled.
 *
 * - Restore V7 idle accounting semantically by charging ticks running as
 *   proc[0] to the dk_time idle bucket.  Do not restore the PDP-11 waitloc PC
 *   comparison: Epoch68 has no corresponding wait instruction or symbol.
 *
 * - Restore profil(2) as a separate complete change.  It requires a 32-bit
 *   safe addupc(), syscall 44 trap/libc plumbing, clock-tick sampling, and
 *   syscall-return system-time sampling before the gate below can be enabled.
 *
 * - Keep deferred UART polling here until tty work can safely be moved out of
 *   the shallow hardware-interrupt stack into a dedicated deferred path.
 */

/*
 * clock is called straight from
 * the real time clock interrupt.
 *
 * Functions:
 *	reprime clock
 *	copy *switches to display
 *	implement callouts
 *	maintain user/system times
 *	maintain date
 *	profile
 *	lightning bolt wakeup (every second)
 *	alarm clock signals
 *	jab the scheduler
 */

void
clock(dev_t dev, caddr_t sp, i32 r1, i32 nps, i32 r0, caddr_t pc, i32 ps)
{
	register struct callo *p1, *p2;
	register struct proc *pp;
	i32 a;
#if 0 /* needs historical waitloc accounting hook */
	extern caddr_t waitloc;
#endif
#if EPOCH68_POLL_UART_IN_CLOCK
	/*
	 * A clock interrupt can arrive while a process already has a deep syscall
	 * call chain on its 2 KiB supervisor stack.  Adding tty input/output work
	 * there can cross into the preceding process slot.  Poll after user-mode
	 * execution; proc0 polls from sched() without the clock call chain beneath
	 * it.  Defer polling for an ordinary process interrupted inside the kernel.
	 */
	if (USERMODE(ps))
		epoch68_uart_poll_rx();
#endif
	EPOCH68_MMIO_BASE[EPOCH68_TIMER_CONTROL_REG] = EPOCH68_TIMER_ENABLE;
	/*
	 * restart clock
	 */

#if 0 /* needs PDP-11 line clock register */
	lks->r[0] = 0115;
#endif

	/*
	 * display register
	 */

#if 0 /* needs PDP-11 display register */
	display();
#endif
	/*
	 * callouts
	 * if none, just continue
	 * else update first non-zero time
	 */

	if(callout[0].c_func == NULL)
		goto out;
	p2 = &callout[0];
	while(p2->c_time<=0 && p2->c_func!=NULL)
		p2++;
	p2->c_time--;

	/*
	 * if ps is high, just return
	 */
	if (BASEPRI(ps))
		goto out;

	/*
	 * callout
	 */

	spl5();
	if(callout[0].c_time <= 0) {
		p1 = &callout[0];
		while(p1->c_func != 0 && p1->c_time <= 0) {
			(*p1->c_func)(p1->c_arg);
			p1++;
		}
		p2 = &callout[0];
		while(p2->c_func = p1->c_func) {
			p2->c_time = p1->c_time;
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}

	/*
	 * lightning bolt time-out
	 * and time of day
	 */
out:
	a = dk_busy&07;
	if (USERMODE(ps)) {
		u.u_utime++;
#if 0 /* needs profiling syscall support */
		if(u.u_prof.pr_scale)
			addupc(pc, &u.u_prof, 1);
#endif
		if(u.u_procp->p_nice > NZERO)
			a += 8;
	} else {
		a += 16;
#if 0 /* needs historical waitloc accounting hook */
		if (pc == waitloc)
			a += 8;
#endif
		u.u_stime++;
	}
	dk_time[a] += 1;
	pp = u.u_procp;
	if(++pp->p_cpu == 0)
		pp->p_cpu--;
	if(++lbolt >= HZ) {
		if (BASEPRI(ps))
			return;
		lbolt -= HZ;
		++time;
		spl1();
		runrun++;
		wakeup((caddr_t)&lbolt);
		for(pp = &proc[0]; pp < &proc[NPROC]; pp++) {
			if (pp->p_stat && pp->p_stat<SZOMB) {
				if(pp->p_time != 127)
					pp->p_time++;
				if(pp->p_clktim) {
					if(--pp->p_clktim == 0) {
						psignal(pp, SIGCLK);
					}
				}
				a = (pp->p_cpu & 0377)*SCHMAG + pp->p_nice - NZERO;
				if(a < 0)
					a = 0;
				if(a > 255)
					a = 255;
				pp->p_cpu = a;
				if(pp->p_pri >= PUSER)
					setpri(pp);
			}
		}
		if(runin!=0) {
			runin = 0;
			wakeup((caddr_t)&runin);
		}
	}
}

/*
 * Epoch68 interrupt-frame adapter.
 *
 * The historical PDP-11 clock entry receives pc/ps as part of its interrupt
 * calling convention.  The 68k autovector stub extracts the saved SR/PC from
 * the exception frame and passes them here, then the generic V7 clock()
 * accounting/scheduler code runs with the shape it expects.
 */
void
epoch68_clock_intr(unsigned long saved_sr, unsigned long saved_pc)
{
	clock((dev_t)0, (caddr_t)0, 0, 0, 0, (caddr_t)saved_pc, (i32)saved_sr);
}

/*
 * Deliver signals before a clock interrupt returns to user mode.  The
 * syscall path already does this, but a CPU-bound process may make no
 * syscalls at all.  Build the small u_ar0 view needed by sendsig(), then
 * return any changed PC, SR, and USP to the 68000 interrupt frame.
 */
void
epoch68_clock_signals(u32 saved_sr, u32 saved_pc, u32 saved_usp)
{
	i32 regs[12];
	i32 *saved_ar0;
	i32 i;

	epoch68_clock_return_sr = (u16)saved_sr;
	epoch68_clock_return_pc = saved_pc;
	epoch68_clock_return_usp = saved_usp;
	if (!USERMODE(saved_sr) || u.u_procp == NULL ||
	    u.u_procp->p_stat == SZOMB)
		return;

	for (i = 0; i < 12; i++)
		regs[i] = 0;
	saved_ar0 = u.u_ar0;
	u.u_ar0 = &regs[9];
	u.u_ar0[R6] = (i32)saved_usp;
	u.u_ar0[PC] = (i32)saved_pc;
	u.u_ar0[RPS] = (i32)saved_sr;
	if (issig())
		psig();
	epoch68_clock_return_sr = (u16)(u.u_ar0[RPS] & ~0x2000);
	epoch68_clock_return_pc = (u32)u.u_ar0[PC];
	epoch68_clock_return_usp = (u32)u.u_ar0[R6];
	u.u_ar0 = saved_ar0;
}

/*
 * timeout is called to arrange that
 * fun(arg) is called in tim/HZ seconds.
 * An entry is sorted into the callout
 * structure. The time in each structure
 * entry is the number of HZ's more
 * than the previous entry.
 * In this way, decrementing the
 * first entry has the effect of
 * updating all entries.
 *
 * The panic is there because there is nothing
 * intelligent to be done if an entry won't fit.
 */
void
timeout(i32 (*fun)(caddr_t), caddr_t arg, i32 tim)
{
	register struct callo *p1, *p2;
	register i32 t;
	i32 s;

	t = tim;
	p1 = &callout[0];
	s = spl7();
	while(p1->c_func != 0 && p1->c_time <= t) {
		t -= p1->c_time;
		p1++;
	}
	if (p1 >= &callout[NCALL-1])
		panic("Timeout table overflow");
	p1->c_time -= t;
	p2 = p1;
	while(p2->c_func != 0)
		p2++;
	while(p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		p2--;
	}
	p1->c_time = t;
	p1->c_func = fun;
	p1->c_arg = arg;
	splx(s);
}
