#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/stat.h"
#include "../h/timeb.h"
#include "../h/user.h"
#include "../h/reg.h"
#include "../h/proc.h"

void exec(void);
void exece(void);
void fork(void);
void read(void);
void write(void);
void open(void);
void creat(void);
void close(void);
void dup(void);
void fstat(void);
void chdir(void);
void seek(void);
void link(void);
void unlink(void);
void mknod(void);
void chmod(void);
void chown(void);
void smount(void);
void sumount(void);
void pipe(void);
void swtch(void);
void panic(char *s);
void alarm(void);
void gtime(void);
void kill(void);
void nice(void);
void pause(void);
void wait(void);
void sbreak(void);
void setuid(void);
void setgid(void);
void umask(void);
void saccess(void);
void ssig(void);
void stat(void);
void utime(void);
void ftime(void);
void stime(void);
void times(void);
void sync(void);
void sysacct(void);
void psig(void);
i32 issig(void);
i32 save(u32 *lp);
i32 setpri(struct proc *p);
i32 copyin(caddr_t from, caddr_t to, i32 count);
i32 copyout(caddr_t from, caddr_t to, i32 count);
void epoch68_check_current_kernel_stack(char *where);

void
epoch68_fault(i32 sig, u32 *frame, i32 supervisor)
{
	if (supervisor)
		panic("kernel fault");
	exit(sig);
	panic("fault return");
}

/*
 * Saved-register layout for traps and syscalls.
 * This used to live in mch.s as _regloc.
 */
char regloc[9] = {
	R0, R1, R2, R3, R4, R5, R6, R7, RPS
};

/* Secondary result register used by the historical V7 syscall ABI. */
i32 epoch68_trap_return_d1;

#ifndef EPOCH68_HEAP_STACK_CHECK
#define EPOCH68_HEAP_STACK_CHECK 1
#endif

#ifndef EPOCH68_HEAP_STACK_GUARD
#define EPOCH68_HEAP_STACK_GUARD 4096UL
#endif

struct execa_trap0 {
	char *fname;
	char **argp;
	char **envp;
};

struct opena_trap0 {
	char *fname;
	i32 rwmode;
};

struct creata_trap0 {
	char *fname;
	i32 fmode;
};

struct linka_trap0 {
	char *target;
	char *linkname;
};

struct unlinka_trap0 {
	char *fname;
};

struct mknoda_trap0 {
	char *fname;
	i32 fmode;
	i32 dev;
};

struct chmoda_trap0 {
	char *fname;
	i32 fmode;
};

struct chowna_trap0 {
	char *fname;
	i32 uid;
	i32 gid;
};

struct mounta_trap0 {
	char *fspec;
	char *freg;
	i32 ronly;
};

struct umounta_trap0 {
	char *fspec;
};

struct rdwra_trap0 {
	i32 fdes;
	char *cbuf;
	u32 count;
};

struct closea_trap0 {
	i32 fdes;
};

struct exita_trap0 {
	i32 rval;
};

struct dupa_trap0 {
	i32 fdes;
	i32 fdes2;
};

struct alarma_trap0 {
	i32 deltat;
};

struct umaska_trap0 {
	i32 mask;
};

struct nicea_trap0 {
	i32 niceness;
};

struct accessa_trap0 {
	char *fname;
	i32 fmode;
};

struct accta_trap0 {
	char *fname;
};

struct utimea_trap0 {
	char *fname;
	time_t *tptr;
};

struct ida_trap0 {
	i32 id;
};

struct fstata_trap0 {
	i32 fdes;
	char *sb;
};

struct seeka_trap0 {
	i32 fdes;
	off_t off;
	i32 sbase;
};

struct siga_trap0 {
	i32 signo;
	i32 fun;
	caddr_t tramp;
};

struct killa_trap0 {
	i32 pid;
	i32 signo;
};

#define EPOCH68_TRAP_EXECED 0x7fffffffL
#define EPOCH68_TRAP_ENOSYS 38
#define EPOCH68_SYSCALL_KTICKS 65500L
#define EPOCH68_SYSCALL_SHUTDOWN 65501L
#define EPOCH68_SYSCALL_IDLETRAP 65502L
#define EPOCH68_MMIO_BASE ((volatile u8 *)0x00a00000)
#define EPOCH68_SYSTEM_PAGE_REG 0
#define EPOCH68_MACHINE_CONTROL_REG 0x21
#define EPOCH68_MACHINE_RETURN_MONITOR 0x01
u32 epoch68_trap_new_pc;
u32 epoch68_trap_new_usp;
u16 epoch68_trap_new_sr;
u32 epoch68_fork_regs[15];
i32 epoch68_fork_regs_valid;
u32 epoch68_proc_usp[NPROC];
u32 epoch68_resume_usp;
static i32 epoch68_heap_stack_warned_pid = -1;

/*
 * Epoch68 currently gives each process a flat 256KB user address space.  The
 * heap grows upward from low addresses, while the user stack grows downward
 * from the top.  Historical V7 depended on segmentation faults and grow() to
 * catch stack trouble; this port needs an explicit heap/stack guard while the
 * compatibility layer is still being brought up.
 *
 * The warning is intentionally emitted once per process.  The return value
 * remains precise so brk(2) can reject a colliding heap request with ENOMEM.
 */
i32
epoch68_heap_stack_would_collide(heap_end, usp, where)
u32 heap_end;
u32 usp;
char *where;
{
#if EPOCH68_HEAP_STACK_CHECK
	i32 pid;

	if (usp == 0)
		return(0);
	if (heap_end + EPOCH68_HEAP_STACK_GUARD < heap_end)
		goto collide;
	if (heap_end + EPOCH68_HEAP_STACK_GUARD < usp)
		return(0);

collide:
	pid = u.u_procp ? u.u_procp->p_pid : -1;
	if (pid != epoch68_heap_stack_warned_pid)
		epoch68_heap_stack_warned_pid = pid;
	return(1);
#else
	return(0);
#endif
}

void
epoch68_check_heap_stack(u32 usp, char *where)
{
#if EPOCH68_HEAP_STACK_CHECK
	epoch68_heap_stack_would_collide((u32)ctob(u.u_dsize),
	    usp, where);
#endif
}

static i32
epoch68_proc_index(struct proc *p)
{
	if (p < proc || p >= &proc[NPROC])
		return(-1);
	return(p - proc);
}

static void
epoch68_sync_current_from_page(void)
{
	register i32 page;
	register struct proc *p;

	p = u.u_procp;
	if (p == 0 || p == &proc[0] || p->p_stat == SZOMB)
		return;
	if (p < &proc[0] || p >= &proc[NPROC])
		panic("current proc");
	page = EPOCH68_MMIO_BASE[EPOCH68_SYSTEM_PAGE_REG];
	if (uarrp != &uarea[p - proc] || p->p_addr != page)
		panic("current page");
}

void
epoch68_save_current_usp(u32 usp)
{
	register i32 i;

	i = epoch68_proc_index(u.u_procp);
	if (i < 0)
		return;
	epoch68_proc_usp[i] = usp;
}

void
epoch68_set_resume_usp(struct proc *p)
{
	register i32 i;

	i = epoch68_proc_index(p);
	if (i < 0) {
		epoch68_resume_usp = 0;
		return;
	}
	epoch68_resume_usp = epoch68_proc_usp[i];
}

u32
epoch68_trap_return_usp(void)
{
	if (u.u_ar0 == 0) {
		return(0);
	}
	return((u32)u.u_ar0[R6]);
}

i32
epoch68_should_qswtch(void)
{
	return(runrun);
}

long
epoch68_trap_syscall(long sysno, long d1, long d2, char *a0, char **a1,
    u32 usp, u32 sr, u32 pc, u32 *trap_regs)
{
	i32 *saved_ap;
	i32 *saved_ar0;
	caddr_t saved_dirp;
	struct execa_trap0 exec_args;
	struct opena_trap0 open_args;
	struct creata_trap0 creat_args;
	struct linka_trap0 link_args;
	struct unlinka_trap0 unlink_args;
	struct mknoda_trap0 mknod_args;
	struct chmoda_trap0 chmod_args;
	struct chowna_trap0 chown_args;
	struct mounta_trap0 mount_args;
	struct umounta_trap0 umount_args;
	struct rdwra_trap0 rdwr_args;
	struct closea_trap0 close_args;
	struct exita_trap0 exit_args;
	struct dupa_trap0 dup_args;
	struct alarma_trap0 alarm_args;
	struct umaska_trap0 umask_args;
	struct nicea_trap0 nice_args;
	struct accessa_trap0 access_args;
	struct accta_trap0 acct_args;
	struct utimea_trap0 utime_args;
	struct ida_trap0 id_args;
	struct breaka_trap0 {
		caddr_t nsiz;
	} break_args;
	struct fstata_trap0 fstat_args;
	struct stata_trap0 {
		char *fname;
		struct stat *sb;
	} stat_args;
	struct seeka_trap0 seek_args;
	struct siga_trap0 sig_args;
	struct ftimea_trap0 {
		struct timeb *tp;
	} ftime_args;
	struct stimea_trap0 {
		time_t time;
	} stime_args;
	struct timesa_trap0 {
		time_t (*times)[4];
	} times_args;
	struct ioctla_trap0 {
		i32 fdes;
		i32 cmd;
		caddr_t cmarg;
	} ioctl_args;
	struct killa_trap0 kill_args;
	i32 regsave[16];
	long rv;
	register i32 i;

	epoch68_sync_current_from_page();
	/* Preserve the incoming D1 unless this syscall defines a second result. */
	epoch68_trap_return_d1 = (i32)d1;
	epoch68_check_current_kernel_stack("sysin");
	epoch68_save_current_usp(usp);
	epoch68_check_heap_stack(usp, "syscall");
	saved_ap = u.u_ap;
	saved_ar0 = u.u_ar0;
	saved_dirp = u.u_dirp;
	u.u_ap = 0;
	u.u_dirp = 0;
	for (i = 0; i < 16; i++)
		regsave[i] = 0;
	u.u_ar0 = &regsave[9];
	u.u_ar0[R6] = (i32)usp;
	u.u_ar0[RPS] = (i32)sr;
	u.u_ar0[PC] = (i32)pc;
	epoch68_fork_regs_valid = 0;
	if (trap_regs) {
		for (i = 0; i < 15; i++)
			epoch68_fork_regs[i] = trap_regs[i];
		epoch68_fork_regs_valid = 1;
	}
	u.u_error = 0;
	rv = 0;
	exec_args.fname = a0;
	exec_args.argp = a1;
	exec_args.envp = (char **)d2;
	open_args.fname = a0;
	open_args.rwmode = (i32)d1;
	creat_args.fname = a0;
	creat_args.fmode = (i32)d1;
	link_args.target = a0;
	link_args.linkname = (char *)a1;
	unlink_args.fname = a0;
	mknod_args.fname = a0;
	mknod_args.fmode = (i32)d1;
	mknod_args.dev = (i32)d2;
	chmod_args.fname = a0;
	chmod_args.fmode = (i32)d1;
	chown_args.fname = a0;
	chown_args.uid = (i32)d1;
	chown_args.gid = (i32)d2;
	mount_args.fspec = a0;
	mount_args.freg = (char *)a1;
	mount_args.ronly = (i32)d1;
	umount_args.fspec = a0;
	rdwr_args.fdes = (i32)d1;
	rdwr_args.cbuf = a0;
	rdwr_args.count = (u32)d2;
	close_args.fdes = (i32)d1;
	exit_args.rval = (i32)d1;
	dup_args.fdes = (i32)d1;
	dup_args.fdes2 = (i32)d2;
	alarm_args.deltat = (i32)d1;
	umask_args.mask = (i32)d1;
	nice_args.niceness = (i32)d1;
	access_args.fname = a0;
	access_args.fmode = (i32)d1;
	acct_args.fname = a0;
	utime_args.fname = a0;
	utime_args.tptr = (time_t *)a1;
	id_args.id = (i32)d1;
	break_args.nsiz = (caddr_t)d1;
	fstat_args.fdes = (i32)d1;
	fstat_args.sb = a0;
	stat_args.fname = a0;
	stat_args.sb = (struct stat *)a1;
	seek_args.fdes = (i32)d1;
	seek_args.off = (off_t)d2;
	seek_args.sbase = (i32)a0;
	sig_args.signo = (i32)d1;
	sig_args.fun = (i32)a0;
	sig_args.tramp = (caddr_t)d2;
	ftime_args.tp = (struct timeb *)a0;
	ioctl_args.fdes = (i32)d1;
	ioctl_args.cmd = (i32)d2;
	ioctl_args.cmarg = (caddr_t)a0;
	kill_args.pid = (i32)d1;
	kill_args.signo = (i32)d2;

	if (save(u.u_qsav)) {
		if (u.u_error == 0)
			u.u_error = EINTR;
		rv = -(long)u.u_error;
		goto syscall_done;
	}

	epoch68_sync_current_from_page();

	switch (sysno) {
	case 1:
		u.u_ap = (i32 *)&exit_args;
		rexit();
		panic("rexit");
	case 2:
		fork();
		if (u.u_error)
			rv = -(long)u.u_error;
		else {
			rv = (long)u.u_r.r_reg.r_val1;
		}
		break;
	case 3:
		u.u_ap = (i32 *)&rdwr_args;
		read();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 4:
		u.u_ap = (i32 *)&rdwr_args;
		write();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 5:
		u.u_ap = (i32 *)&open_args;
		u.u_dirp = (caddr_t)a0;
		open();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 6:
		u.u_ap = (i32 *)&close_args;
		close();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 7:
		wait();
		if (u.u_error)
			rv = -(long)u.u_error;
		else {
			i16 wait_status;

			rv = (long)u.u_r.r_reg.r_val1;
			wait_status = (i16)u.u_r.r_reg.r_val2;
			if (a0 && copyout((caddr_t)&wait_status, (caddr_t)a0,
			    sizeof(wait_status)) < 0)
				rv = -(long)EFAULT;
		}
		break;
	case 8:
		u.u_ap = (i32 *)&creat_args;
		u.u_dirp = (caddr_t)a0;
		creat();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 9:
		u.u_ap = (i32 *)&link_args;
		u.u_dirp = (caddr_t)a0;
		link();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 10:
		u.u_ap = (i32 *)&unlink_args;
		u.u_dirp = (caddr_t)a0;
		unlink();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 11:
		u.u_ap = (i32 *)&exec_args;
		u.u_dirp = (caddr_t)a0;
		exec();
		if (u.u_error)
			rv = -(long)u.u_error;
		else {
			epoch68_trap_new_pc = (u32)u.u_ar0[PC];
			epoch68_trap_new_usp = (u32)u.u_ar0[R6];
			epoch68_trap_new_sr = (u16)(u.u_ar0[RPS] & ~0x2000);
			rv = EPOCH68_TRAP_EXECED;
		}
		break;
	case 12:
		u.u_dirp = (caddr_t)a0;
		chdir();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 13:
		gtime();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_time;
		break;
	case 14:
		u.u_ap = (i32 *)&mknod_args;
		u.u_dirp = (caddr_t)a0;
		mknod();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 15:
		u.u_ap = (i32 *)&chmod_args;
		u.u_dirp = (caddr_t)a0;
		chmod();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 16:
		u.u_ap = (i32 *)&chown_args;
		u.u_dirp = (caddr_t)a0;
		chown();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 17:
		u.u_ap = (i32 *)&break_args;
		sbreak();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 18:
		u.u_ap = (i32 *)&stat_args;
		u.u_dirp = (caddr_t)a0;
		stat();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 19:
		u.u_ap = (i32 *)&seek_args;
		seek();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_off;
		break;
	case 20:
		rv = (long)u.u_procp->p_pid;
		epoch68_trap_return_d1 = (i32)u.u_procp->p_ppid;
		break;
	case 21:
		u.u_ap = (i32 *)&mount_args;
		u.u_dirp = (caddr_t)a0;
		smount();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 22:
		u.u_ap = (i32 *)&umount_args;
		u.u_dirp = (caddr_t)a0;
		sumount();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 23:
		u.u_ap = (i32 *)&id_args;
		setuid();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 24:
		rv = (long)u.u_ruid;
		epoch68_trap_return_d1 = (i32)u.u_uid;
		break;
	case 25:
		stime_args.time = (time_t)d1;
		u.u_ap = (i32 *)&stime_args;
		stime();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 27:
		u.u_ap = (i32 *)&alarm_args;
		alarm();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 28:
		u.u_ap = (i32 *)&fstat_args;
		fstat();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 29:
		pause();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 30:
		u.u_ap = (i32 *)&utime_args;
		u.u_dirp = (caddr_t)a0;
		utime();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 31:
		u.u_arg[0] = (i32)d1;
		u.u_arg[1] = (i32)a0;
		u.u_arg[2] = 0;
		u.u_ap = u.u_arg;
		stty();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 32:
		u.u_arg[0] = (i32)d1;
		u.u_arg[1] = (i32)a0;
		u.u_arg[2] = 0;
		u.u_ap = u.u_arg;
		gtty();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 33:
		u.u_ap = (i32 *)&access_args;
		u.u_dirp = (caddr_t)a0;
		saccess();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 34:
		u.u_ap = (i32 *)&nice_args;
		nice();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 35:
		u.u_ap = (i32 *)&ftime_args;
		ftime();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 36:
		sync();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 43:
		times_args.times = (time_t (*)[4])a0;
		u.u_ap = (i32 *)&times_args;
		times();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 37:
		u.u_ap = (i32 *)&kill_args;
		kill();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 41:
		u.u_ap = (i32 *)&dup_args;
		dup();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 42:
		pipe();
		if (u.u_error)
			rv = -(long)u.u_error;
		else {
			i32 pipefds[2];

			pipefds[0] = (i32)u.u_r.r_reg.r_val1;
			pipefds[1] = (i32)u.u_r.r_reg.r_val2;
			if (copyout((caddr_t)pipefds, (caddr_t)a0, sizeof(pipefds)) < 0)
				rv = -(long)EFAULT;
			else
				rv = 0;
		}
		break;
	case 46:
		u.u_ap = (i32 *)&id_args;
		setgid();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 47:
		rv = (long)u.u_rgid;
		epoch68_trap_return_d1 = (i32)u.u_gid;
		break;
	case EPOCH68_SYSCALL_IDLETRAP:
		rv = 0;
		break;
	case 48:
		u.u_ap = (i32 *)&sig_args;
		ssig();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case 51:
		u.u_ap = (i32 *)&acct_args;
		u.u_dirp = (caddr_t)a0;
		sysacct();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 54:
		u.u_ap = (i32 *)&ioctl_args;
		ioctl();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = 0;
		break;
	case 60:
		u.u_ap = (i32 *)&umask_args;
		umask();
		if (u.u_error)
			rv = -(long)u.u_error;
		else
			rv = (long)u.u_r.r_reg.r_val1;
		break;
	case EPOCH68_SYSCALL_KTICKS:
		rv = (long)kticks;
		break;
	case EPOCH68_SYSCALL_SHUTDOWN:
		if (u.u_uid != 0) {
			rv = -(long)EPERM;
			break;
		}
		sync();
		EPOCH68_MMIO_BASE[EPOCH68_MACHINE_CONTROL_REG] = EPOCH68_MACHINE_RETURN_MONITOR;
		for (;;)
			;
	default:
		rv = -EPOCH68_TRAP_ENOSYS;
		break;
	}

syscall_done:
	if (u.u_procp && u.u_procp->p_stat != SZOMB && issig())
		psig();
	if (u.u_procp && u.u_procp->p_stat == SRUN)
		curpri = setpri(u.u_procp);
	if (rv == EPOCH68_TRAP_EXECED)
		epoch68_save_current_usp(epoch68_trap_new_usp);
	else if (u.u_ar0) {
		epoch68_trap_new_pc = (u32)u.u_ar0[PC];
		epoch68_trap_new_usp = (u32)u.u_ar0[R6];
		epoch68_trap_new_sr = (u16)(u.u_ar0[RPS] & ~0x2000);
		epoch68_save_current_usp((u32)u.u_ar0[R6]);
	} else {
		epoch68_trap_new_pc = pc;
		epoch68_trap_new_usp = usp;
		epoch68_trap_new_sr = (u16)(sr & ~0x2000);
	}
	epoch68_check_heap_stack(epoch68_trap_new_usp, "sysret");
	u.u_ap = saved_ap;
	u.u_ar0 = saved_ar0;
	u.u_dirp = saved_dirp;
	epoch68_check_current_kernel_stack("sysout");
	return(rv);
}
