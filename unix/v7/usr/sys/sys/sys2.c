#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/reg.h"
#include "../h/file.h"
#include "../h/inode.h"

void read(void);
void write(void);
void rdwr(i32 mode);
void open(void);
void creat(void);
void open1(struct inode *ip, i32 mode, i32 trf);
void close(void);
void seek(void);
void link(void);
void unlink(void);
void mknod(void);
void saccess(void);
i32 access(struct inode *ip, i32 mode);
i32 suser(void);
void openi(struct inode *ip, i32 rw);
void prele(struct inode *ip);
void readp(struct file *fp);
void wdir(struct inode *ip);
void writep(struct file *fp);


/*
 * read system call
 */
void read(void)
{
	rdwr(FREAD);
}

/*
 * write system call
 */
void write(void)
{
	rdwr(FWRITE);
}

/*
 * common code for read and write calls:
 * check permissions, set base, count, and offset,
 * and switch out to readi, writei, or pipe code.
 */
void rdwr(register i32 mode)
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		i32	fdes;
		char	*cbuf;
		u32	count;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if((fp->f_flag&mode) == 0) {
		u.u_error = EBADF;
		return;
	}
	u.u_base = (caddr_t)uap->cbuf;
	u.u_count = uap->count;
	u.u_segflg = 0;
	if((fp->f_flag&FPIPE) != 0) {
		if(mode == FREAD)
			readp(fp);
		else
			writep(fp);
	} else {
		ip = fp->f_inode;
		if (fp->f_flag&FMP)
			u.u_offset = 0;
		else
			u.u_offset = fp->f_un.f_offset;
		if((ip->i_mode&(IFCHR&IFBLK)) == 0)
			plock(ip);
		if(mode == FREAD)
			readi(ip);
		else
			writei(ip);
		if((ip->i_mode&(IFCHR&IFBLK)) == 0)
			prele(ip);
		if ((fp->f_flag&FMP) == 0)
			fp->f_un.f_offset += uap->count-u.u_count;
	}
	u.u_r.r_reg.r_val1 = uap->count-u.u_count;
}

/*
 * open system call
 */
void open(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		i32	rwmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL) {
		return;
	}
	uap->rwmode++;
	open1(ip, uap->rwmode, 0);
}

/*
 * creat system call
 */
void creat(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		i32	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 1);
	if(ip == NULL) {
		if(u.u_error)
			return;
		ip = maknode(uap->fmode&07777&(~ISVTX));
		if (ip==NULL)
			return;
		open1(ip, FWRITE, 2);
	} else
		open1(ip, FWRITE, 1);
}

/*
 * common code for open and creat.
 * Check permissions, allocate an open file structure,
 * and call the device open routine if any.
 */
void open1(register struct inode *ip, register i32 mode, register i32 trf)
{
	register struct file *fp;
	i32 i;

	if(trf != 2) {
		if(mode&FREAD)
			(void)access(ip, IREAD);
		if(mode&FWRITE) {
			(void)access(ip, IWRITE);
			if((ip->i_mode&IFMT) == IFDIR)
				u.u_error = EISDIR;
		}
	}
	if(u.u_error) {
		goto out;
	}
	if(trf == 1)
		itrunc(ip);
	prele(ip);
	if ((fp = falloc()) == NULL) {
		goto out;
	}
	fp->f_flag = mode&(FREAD|FWRITE);
	fp->f_inode = ip;
	i = u.u_r.r_reg.r_val1;
	openi(ip, mode&FWRITE);
	if(u.u_error == 0) {
		return;
	}
	u.u_ofile[i] = NULL;
	fp->f_count--;

out:
	iput(ip);
}

/*
 * close system call
 */
void close(void)
{
	register struct file *fp;
	register struct a {
		i32	fdes;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	u.u_ofile[uap->fdes] = NULL;
	closef(fp);
}

/*
 * seek system call
 */
void seek(void)
{
	register struct file *fp;
	register struct a {
		i32	fdes;
		off_t	off;
		i32	sbase;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if(fp->f_flag&(FPIPE|FMP)) {
		u.u_error = ESPIPE;
		return;
	}
	if(uap->sbase == 1)
		uap->off += fp->f_un.f_offset;
	else if(uap->sbase == 2)
		uap->off += fp->f_inode->i_size;
	fp->f_un.f_offset = uap->off;
	u.u_r.r_off = uap->off;
}

/*
 * link system call
 */
void link(void)
{
	register struct inode *ip, *xp;
	register struct a {
		char	*target;
		char	*linkname;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	if((ip->i_mode&IFMT)==IFDIR && !suser())
		goto out;
	/*
	 * Unlock to avoid possibly hanging the namei.
	 * Sadly, this means races. (Suppose someone
	 * deletes the file in the meantime?)
	 * Nor can it be locked again later
	 * because then there will be deadly
	 * embraces.
	 */
	prele(ip);
	u.u_dirp = (caddr_t)uap->linkname;
	xp = namei(uchar, 1);
	if(xp != NULL) {
		u.u_error = EEXIST;
		iput(xp);
		goto out;
	}
	if (u.u_error)
		goto out;
	if(u.u_pdir->i_dev != ip->i_dev) {
		iput(u.u_pdir);
		u.u_error = EXDEV;
		goto out;
	}
	wdir(ip);
	if (u.u_error==0) {
		ip->i_nlink++;
		ip->i_flag |= ICHG;
	}

out:
	iput(ip);
}

/*
 * mknod system call
 */
void mknod(void)
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		i32	fmode;
		i32	dev;
	} *uap;

	uap = (struct a *)u.u_ap;
	if(suser()) {
		ip = namei(uchar, 1);
		if(ip != NULL) {
			u.u_error = EEXIST;
			goto out;
		}
	}
	if(u.u_error)
		return;
	ip = maknode(uap->fmode);
	if (ip == NULL)
		return;
	switch (ip->i_mode & IFMT) {
	case IFCHR:
	case IFBLK:
	case IFMPC:
	case IFMPB:
		ip->i_un.i_special.i_rdev = (dev_t)uap->dev;
	}

out:
	iput(ip);
}

/*
 * access system call
 */
void saccess(void)
{
	register i32 svuid, svgid;
	register struct inode *ip;
	register struct a {
		char	*fname;
		i32	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	svuid = u.u_uid;
	svgid = u.u_gid;
	u.u_uid = u.u_ruid;
	u.u_gid = u.u_rgid;
	ip = namei(uchar, 0);
	if (ip != NULL) {
		if (uap->fmode&(IREAD>>6))
			(void)access(ip, IREAD);
		if (uap->fmode&(IWRITE>>6))
			(void)access(ip, IWRITE);
		if (uap->fmode&(IEXEC>>6))
			(void)access(ip, IEXEC);
		iput(ip);
	}
	u.u_uid = svuid;
	u.u_gid = svgid;
}
