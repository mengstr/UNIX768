#ifndef _USER_STR_H
#define _USER_STR_H

#include "inttypes.h"
/*
 * struct user -- shared layout fragment.
 *
 * Included verbatim by the kernel <user.h> (usr/sys/h) and by the userland
 * <sys/user.h> (usr/include/sys) so the two can never disagree on the layout
 * (ps and core-file readers rely on it matching the kernel exactly).
 *
 * Assumes the includer has already pulled in param.h (DIRSIZ/NOFILE/NSIG and
 * label_t) and the relevant type/struct headers -- which both wrappers do
 * before including this, exactly the context the inline struct required.
 */
struct	user
{
	/*
	 * label_t is defined in param.h as u32[13].
	 * Slot usage is:
	 *   [0] saved PC
	 *   [1] saved SP
	 *   [2..] saved d2-d7/a2-a6
	 */
	label_t	u_rsav;				/* save info when exchanging stacks */
	i32	u_fper;					/* FP error register */
	i32	u_fpsaved;				/* FP regs saved for this proc */
	struct {
		i32	u_fpsr;				/* FP status register */
		double	u_fpregs[6];	/* FP registers */
	} u_fps;
	char	u_segflg;			/* IO flag: 0:user D; 1:system; 2:user I */
	char	u_error;			/* return error code */
	i16	u_uid;				/* effective user id */
	i16	u_gid;				/* effective group id */
	i16	u_ruid;				/* real user id */
	i16	u_rgid;				/* real group id */
	struct proc *u_procp;		/* pointer to proc structure */
	i32	*u_ap;					/* pointer to arglist */
	union {						/* syscall return values */
		struct	{
			i32	r_val1;
			i32	r_val2;
		} r_reg; /* 768 - Anonymous union member is not allowed */
		off_t	r_off;
		time_t	r_time;
	} u_r;
	caddr_t	u_base;					/* base address for IO */
	u32	u_count;			/* bytes remaining for IO */
	off_t	u_offset;				/* offset in file for IO */
	struct inode *u_cdir;			/* pointer to inode of current directory */
	struct inode *u_rdir;			/* root directory of current process */
	char	u_dbuf[DIRSIZ];			/* current pathname component */
	caddr_t	u_dirp;					/* pathname pointer */
	struct direct u_dent;			/* current directory entry */
	struct inode *u_pdir;			/* inode of parent directory of dirp */
	i32	u_uisa[16];					/* prototype of segmentation addresses */
	i32	u_uisd[16];					/* prototype of segmentation descriptors */
	struct file *u_ofile[NOFILE];	/* pointers to file structures of open files */
	char	u_pofile[NOFILE];		/* per-process flags of open files */
	i32	u_arg[5];					/* arguments to current system call */
	u32	u_tsize;				/* text size (clicks) */
	u32	u_dsize;				/* data size (clicks) */
	u32	u_ssize;				/* stack size (clicks) */
	label_t	u_qsav;					/* label variable for quits and interrupts */
	label_t	u_ssav;					/* label variable for swapping */
	i32	u_signal[NSIG];				/* disposition of signals */
	i32	u_sigtramp;					/* 68k signal return trampoline */
	time_t	u_utime;				/* this process user time */
	time_t	u_stime;				/* this process system time */
	time_t	u_cutime;				/* sum of childs' utimes */
	time_t	u_cstime;				/* sum of childs' stimes */
	i32	*u_ar0;						/* address of users saved R0 */
	struct {						/* profile arguments */
		i16	*pr_base;			/* buffer base */
		u32	pr_size;			/* buffer size */
		u32	pr_off;			/* pc offset */
		u32	pr_scale;			/* pc scaling */
	} u_prof;
	char	u_intflg;				/* catch intr from sys */
	char	u_sep;					/* flag for I and D separation */
	struct tty *u_ttyp;				/* controlling tty pointer */
	dev_t	u_ttyd;					/* controlling tty dev */
	struct {						/* header of executable file */
		u32	ux_mag;					/* magic number */
		u32	ux_tsize;			/* text size */
		u32	ux_dsize;			/* data size */
		u32	ux_bsize;			/* bss size */
		u32	ux_ssize;			/* symbol table size */
		u32	ux_entloc;			/* entry location */
		u32	ux_unused;
		u32	ux_relflg;
	} u_exdata;
	char	u_comm[DIRSIZ];
	time_t	u_start;
	char	u_acflag;
	i16	u_fpflag;				/* unused now, will be later */
	i16	u_cmask;				/* mask for file creation */
	i32	u_dummy;                /* traditionally used for top of stack */
};
#endif /* _USER_STR_H */
