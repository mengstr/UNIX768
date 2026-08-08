#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/reg.h"
#include "../h/inode.h"
#include "../h/seg.h"
#include "../h/acct.h"


void exec(void);
void exece(void);
i32 getxfile(struct inode *ip, i32 nargc);
void setregs(void);
void rexit(void);
void exit(i32 rv);
void wait(void);
void fork(void);
void sbreak(void);
void xmark(struct inode *ip);

#define AOUT_HDR_SIZE 32
#define AOUT_ZMAGIC 0413
#define AOUT_MID_SUN010 1
#define USER_STACK_TOP 0x0003fffcL
#define EXEC_STACK_SLOT_SIZE 4
#define EPOCH68_EXECARGS 0x0003fff8L
static long aout_get32(unsigned char *p);
static i32 aout_getmid(unsigned char *p);
static i32 aout_getmagic(unsigned char *p);
static u32 exec_btoc32(u32 bytes);
static u32 epoch68_exec_fuint(caddr_t addr);
static i32 epoch68_exec_suint(caddr_t addr, u32 value);
static void exec_arg_acquire(void);
static void exec_arg_release(void);

/*
 * exec system call, with and without environments.
 */
struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

/*
 * The current disk image has no blocks reserved outside its filesystem for
 * V7 swap-backed exec arguments.  Serialize use of the kernel staging buffer
 * so an exec that sleeps during file I/O cannot be overwritten by another.
 */
static char exec_argbuf[NCARGS];
static i32 exec_argbusy;
static i32 exec_argwant;

static void
exec_arg_acquire()
{
	while(exec_argbusy) {
		exec_argwant = 1;
		sleep((caddr_t)&exec_argbusy, PZERO);
	}
	exec_argbusy = 1;
}

static void
exec_arg_release()
{
	exec_argbusy = 0;
	if(exec_argwant) {
		exec_argwant = 0;
		wakeup((caddr_t)&exec_argbusy);
	}
}

static u32
epoch68_exec_fuint(addr)
caddr_t addr;
{
	u32 value;

	if (copyin(addr, (caddr_t)&value, sizeof(value)) < 0)
		return((u32)-1);
	return(value);
}

static i32
epoch68_exec_suint(addr, value)
caddr_t addr;
u32 value;
{
	if (copyout((caddr_t)&value, addr, sizeof(value)) < 0)
		return(-1);
	return(0);
}

void exec(void)
{
	((struct execa *)u.u_ap)->envp = NULL;
	exece();
}

void exece(void)
{
	register i32 nc;
	register struct execa *uap;
	i32 na, ne, c, arglocked;
	u32 ucp, ap, argp, argbase;
	struct inode *ip;

	ip = namei(uchar, 0);
	if (ip == NULL)
		return;
	arglocked = 0;
	if(access(ip, IEXEC))
		goto bad;
	if((ip->i_mode & IFMT) != IFREG ||
	   (ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0) {
		u.u_error = EACCES;
		goto bad;
	}
	/*
	 * Collect arguments in the serialized staging buffer.
	 */
	na = 0;
	ne = 0;
	nc = 0;
	uap = (struct execa *)u.u_ap;
	exec_arg_acquire();
	arglocked = 1;
	if (uap->argp) for (;;) {
		argp = 0;
		if (uap->argp) {
			argp = epoch68_exec_fuint((caddr_t)uap->argp);
			uap->argp++;
		}
		if (argp==0 && uap->envp) {
			uap->argp = NULL;
			if ((argp = epoch68_exec_fuint((caddr_t)uap->envp)) == 0)
				break;
			uap->envp++;
			ne++;
		}
		if (argp==0)
			break;
		na++;
		if(argp == (u32)-1)
			u.u_error = EFAULT;
		do {
			if (nc >= NCARGS-1)
				u.u_error = E2BIG;
			if ((c = fubyte((caddr_t)argp++)) < 0)
				u.u_error = EFAULT;
			if (u.u_error)
				goto bad;
			exec_argbuf[nc++] = c;
		} while (c>0);
	}
	nc = (nc + EXEC_STACK_SLOT_SIZE-1) & ~(EXEC_STACK_SLOT_SIZE-1);
	if (getxfile(ip, nc) || u.u_error)
		goto bad;

	/*
	 * copy back arglist
	 */

	/*
	 * The historical PDP-11 code built the exec argument block using
	 * negative offsets from the top of user space. On Epoch68 we have a
	 * flat user address space, and userland startup now preserves the
	 * kernel-built stack instead of resetting SP, so build the stack at
	 * the real absolute top of the user stack.
	 *
	 * Keep argc, argv[] and envp[] in 32-bit slots to match libc crt0.s.
	 */
	ucp = USER_STACK_TOP - (u32)nc - EXEC_STACK_SLOT_SIZE;
	ap = ucp - ((u32)na * EXEC_STACK_SLOT_SIZE) - (3 * EXEC_STACK_SLOT_SIZE);
	argbase = ap + EXEC_STACK_SLOT_SIZE;
	u.u_ar0[R6] = (i32)ap;
	if (epoch68_exec_suint((caddr_t)ap, na-ne) < 0) {
		u.u_error = EFAULT;
		goto bad;
	}
	nc = 0;
	for (;;) {
		ap += EXEC_STACK_SLOT_SIZE;
		if (na==ne) {
			if (epoch68_exec_suint((caddr_t)ap, 0) < 0) {
				u.u_error = EFAULT;
				goto bad;
			}
			ap += EXEC_STACK_SLOT_SIZE;
		}
		if (--na < 0)
			break;
		if (epoch68_exec_suint((caddr_t)ap, ucp) < 0) {
			u.u_error = EFAULT;
			goto bad;
		}
		do {
			c = exec_argbuf[nc];
			if (subyte((caddr_t)ucp++, c) < 0)
				u.u_error = EFAULT;
			nc++;
			if (u.u_error)
				goto bad;
		} while(c&0377);
	}
	if (epoch68_exec_suint((caddr_t)ap, 0) < 0) {
		u.u_error = EFAULT;
		goto bad;
	}
	if (epoch68_exec_suint((caddr_t)ucp, 0) < 0 ||
	    epoch68_exec_suint((caddr_t)EPOCH68_EXECARGS, argbase) < 0) {
			u.u_error = EFAULT;
			goto bad;
	}
	setregs();
bad:
	if(arglocked)
		exec_arg_release();
	iput(ip);
}

/*
 * Read in and set up memory for executed file.
 * Zero return is normal.
 */
i32 getxfile(register struct inode *ip, i32 nargc)
{
	register u32 ds;
	register i32 sep;
	register u32 ts, ss;
	register i32 i;
	u32 image_size, old_size;

	/*
	 * Read and decode the big-endian SUN010 0413 header produced by the
	 * Epoch68 toolchain.
	 */

	u.u_base = (caddr_t)&u.u_exdata;
	u.u_count = sizeof(u.u_exdata);
	u.u_offset = 0;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if(u.u_error)
		goto bad;
	if (u.u_count!=0) {
		u.u_error = ENOEXEC;
		goto bad;
	}
	sep = 0;
	if (aout_getmid((unsigned char *)&u.u_exdata) != AOUT_MID_SUN010 ||
	    aout_getmagic((unsigned char *)&u.u_exdata) != AOUT_ZMAGIC) {
		u.u_error = ENOEXEC;
		goto bad;
	}
	u.u_exdata.ux_mag = 0410;
	u.u_exdata.ux_tsize = aout_get32((unsigned char *)&u.u_exdata + 4) - AOUT_HDR_SIZE;
	u.u_exdata.ux_dsize = aout_get32((unsigned char *)&u.u_exdata + 8);
	u.u_exdata.ux_bsize = aout_get32((unsigned char *)&u.u_exdata + 12);
	u.u_exdata.ux_ssize = 0;
	u.u_exdata.ux_entloc = aout_get32((unsigned char *)&u.u_exdata + 20);
	u.u_exdata.ux_unused = 0;
	u.u_exdata.ux_relflg = 1;
	if ((long)u.u_exdata.ux_tsize < 0) {
		u.u_error = ENOEXEC;
		goto bad;
	}
	if(u.u_exdata.ux_tsize!=0 && (ip->i_flag&ITEXT)==0 && ip->i_count!=1) {
		u.u_error = ETXTBSY;
		goto bad;
	}

	/*
	 * find text and data sizes
	 * try them out for possible
	 * overflow of max sizes
	 */
	image_size = u.u_exdata.ux_tsize;
	old_size = image_size;
	image_size += u.u_exdata.ux_dsize;
	if (image_size < old_size) {
		u.u_error = ENOMEM;
		goto bad;
	}
	old_size = image_size;
	image_size += u.u_exdata.ux_bsize;
	if (image_size < old_size) {
		u.u_error = ENOMEM;
		goto bad;
	}
	ts = exec_btoc32(u.u_exdata.ux_tsize);
	ds = exec_btoc32(image_size);
	ss = SSIZE + exec_btoc32((u32)nargc);
	if(estabur(ts, ds, ss, sep, RO))
		goto bad;

	/*
	 * Fixed-window port: load the new image directly into the
	 * process page and skip the old shared-text/swap machinery.
	 */
	u.u_prof.pr_scale = 0;
	xfree();
	u.u_procp->p_textp = NULL;
	i = USIZE+ds+ss;
	expand(i);
	i = ds;
	while(--i >= 0)
		clearseg(i);
	if (u.u_error)
		goto bad;

	/*
	 * Read the text at user address 0 when the executable has
	 * a separate text segment.
	 */
	if (u.u_exdata.ux_tsize != 0) {
		estabur(ts, (unsigned)0, (unsigned)0, 0, RW);
		u.u_base = 0;
		u.u_offset = sizeof(u.u_exdata);
		u.u_count = u.u_exdata.ux_tsize;
		u.u_segflg = 2;
		readi(ip);
		u.u_segflg = 0;
		if (u.u_error)
			goto bad;
	}

	/*
	 * Data follows the text in the unified user window.
	 */
	estabur((unsigned)0, ds, (unsigned)0, 0, RO);
	u.u_base = (caddr_t)u.u_exdata.ux_tsize;
	u.u_offset = sizeof(u.u_exdata)+u.u_exdata.ux_tsize;
	u.u_count = u.u_exdata.ux_dsize;
	readi(ip);
	if (u.u_error)
		goto bad;
	xmark(ip);
	/*
	 * set SUID/SGID protections, if no tracing
	 */
	if ((u.u_procp->p_flag&STRC)==0) {
		if(ip->i_mode&ISUID)
			if(u.u_uid != 0) {
				u.u_uid = ip->i_uid;
				u.u_procp->p_uid = ip->i_uid;
			}
		if(ip->i_mode&ISGID)
			u.u_gid = ip->i_gid;
	} else
		psignal(u.u_procp, SIGTRC);
	u.u_tsize = ts;
	u.u_dsize = ds;
	u.u_ssize = ss;
	u.u_sep = sep;
	estabur(ts, ds, ss, sep, RO);
bad:
	return(0);
}

static long
aout_get32(p)
unsigned char *p;
{
	return(((long)p[0] << 24) |
	    ((long)p[1] << 16) |
	    ((long)p[2] << 8) |
	    (long)p[3]);
}

static i32
aout_getmid(p)
unsigned char *p;
{
	return((aout_get32(p) >> 16) & 0x3ff);
}

static i32
aout_getmagic(p)
unsigned char *p;
{
	return(aout_get32(p) & 0xffff);
}

static u32
exec_btoc32(bytes)
u32 bytes;
{
	return((bytes + 63L) >> 6);
}

/*
 * Clear registers on exec
 */
void setregs(void)
{
	register i32 *rp;
	register char *cp;
	register i32 i;

	for(rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++)
		if((*rp & 1) == 0)
			*rp = 0;
	for(cp = &regloc[0]; cp < &regloc[6];)
		u.u_ar0[*cp++] = 0;
	u.u_ar0[PC] = u.u_exdata.ux_entloc & ~01;
	for(rp = (i32 *)&u.u_fps; rp < (i32 *)&u.u_fps.u_fpregs[6];)
		*rp++ = 0;
	for(i=0; i<NOFILE; i++) {
		if (u.u_pofile[i]&EXCLOSE) {
			closef(u.u_ofile[i]);
			u.u_ofile[i] = NULL;
			u.u_pofile[i] &= ~EXCLOSE;
		}
	}
	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag &= ~AFORK;
	bcopy((caddr_t)u.u_dbuf, (caddr_t)u.u_comm, DIRSIZ);
}

/*
 * exit system call:
 * pass back caller's arg
 */
void rexit(void)
{
	register struct a {
		i32	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0377) << 8);
}

/*
 * Release resources.
 * Save u. area for parent to look at.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
void exit(i32 rv)
{
	register i32 i, s;
	register struct proc *p, *q;
	register struct file *f;
	i32 userpage;

	p = u.u_procp;
	p->p_flag &= ~(STRC|SULOCK);
	p->p_clktim = 0;
	for(i=0; i<NSIG; i++)
		u.u_signal[i] = 1;
	for(i=0; i<NOFILE; i++) {
		f = u.u_ofile[i];
		u.u_ofile[i] = NULL;
		closef(f);
	}
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	xfree();
	acct();
	userpage = p->p_addr;
	s = spl6();
	for (q = runq; q != NULL; q = q->p_link)
		if (q == p)
			panic("zombie on runq");
	userpage_free(userpage);
	((struct xproc *)p)->xp_xstat = rv;
	((struct xproc *)p)->xp_utime = u.u_cutime + u.u_utime;
	((struct xproc *)p)->xp_stime = u.u_cstime + u.u_stime;
	p->p_stat = SZOMB;
	for(q = &proc[0]; q < &proc[NPROC]; q++) {
		if(q->p_ppid == p->p_pid) {
			wakeup((caddr_t)&proc[1]);
			q->p_ppid = 1;
			if (q->p_stat==SSTOP)
				setrun(q);
		}
	}
	for(q = &proc[0]; q < &proc[NPROC]; q++) {
		if(p->p_ppid == q->p_pid) {
			wakeup((caddr_t)q);
			swtch();
			panic("zombie resumed");
		}
	}
	swtch();
	splx(s);
	panic("zombie resumed");
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
void wait(void)
{
	register i32 f;
	register struct proc *p;

	f = 0;

loop:
	for(p = &proc[0]; p < &proc[NPROC]; p++)
	if(p->p_ppid == u.u_procp->p_pid) {
		f++;
		if(p->p_stat == SZOMB) {
			u.u_r.r_reg.r_val1 = p->p_pid;
			u.u_r.r_reg.r_val2 = ((struct xproc *)p)->xp_xstat;
			u.u_cutime += ((struct xproc *)p)->xp_utime;
			u.u_cstime += ((struct xproc *)p)->xp_stime;
			p->p_pid = 0;
			p->p_ppid = 0;
			p->p_pgrp = 0;
			p->p_sig = 0;
			p->p_flag = 0;
			p->p_wchan = 0;
			p->p_stat = NULL;
			return;
		}
		if(p->p_stat == SSTOP) {
			if((p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_r.r_reg.r_val1 = p->p_pid;
				u.u_r.r_reg.r_val2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if(f) {
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

/*
 * fork system call.
 */
void fork(void)
{
	register struct proc *p1, *p2;
	struct proc *parentp;
	register i32 a;

	a = 0;
	p2 = NULL;
	for(p1 = &proc[0]; p1 < &proc[NPROC]; p1++) {
		if (p1->p_stat==NULL && p2==NULL)
			p2 = p1;
		else {
			if (p1->p_uid==u.u_uid && p1->p_stat!=NULL)
				a++;
		}
	}
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned; or
	 *  not su and would take last slot.
	 */
	if (p2==NULL || (u.u_uid!=0 && (p2==&proc[NPROC-1] || a>MAXUPRC))) {
		u.u_error = EAGAIN;
		goto out;
	}
	parentp = u.u_procp;
	newproc(parentp);
	if (u.u_error)
		goto out;
	u.u_r.r_reg.r_val1 = p2->p_pid;

out:
	return;
}

/*
 * break system call.
 *  -- bad planning: "break" is a dirty word in C.
 */
void sbreak(void)
{
	struct a {
		caddr_t	nsiz;
	};
	register i32 n;
	i32 oldn, total;
	u32 nsiz;
	u32 usp;

	/*
	 * Epoch68 has a fixed user window: the heap grows upward and the stack
	 * remains at the top.  Changing the break therefore changes accounting
	 * and clears newly exposed clicks, but never relocates the stack.
	 */
	nsiz = (u32)((struct a *)u.u_ap)->nsiz;
	usp = u.u_ar0 ? (u32)u.u_ar0[R6] : 0;
	if (epoch68_heap_stack_would_collide(nsiz, usp, "brk")) {
		u.u_error = ENOMEM;
		return;
	}
	n = exec_btoc32(nsiz);
	if (n < (i32)u.u_tsize) {
		u.u_error = ENOMEM;
		return;
	}
	total = USIZE+n+u.u_ssize;
	if(estabur(u.u_tsize, n, u.u_ssize, u.u_sep, RO))
		return;
	expand(total);
	if (u.u_error)
		return;
	oldn = u.u_dsize;
	while(oldn < n)
		clearseg(oldn++);
	u.u_dsize = n;
}
