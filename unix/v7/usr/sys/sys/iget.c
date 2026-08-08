#include "../h/param.h"
#include "../h/systm.h"
#include "../h/mount.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"
#include "../h/inode.h"
#include "../h/ino.h"
#include "../h/filsys.h"
#include "../h/conf.h"
#include "../h/buf.h"

#pragma dontwarn 213

void iexpand(struct inode *ip, struct dinode *dp);
void iput(struct inode *ip);
void iupdat(struct inode *ip, time_t *ta, time_t *tm);
void itrunc(struct inode *ip);
void tloop(i32 dev, daddr_t bn, i32 f1, i32 f2);
void wdir(struct inode *ip);

static void iaddr_from_disk(daddr_t *lp, char *cp);
static void iaddr_to_disk(char *cp, daddr_t *lp);

static void
iaddr_from_disk(lp, cp)
register daddr_t *lp;
register char *cp;
{
	register i32 i;
	register unsigned char *p;

	for(i=0; i<NADDR; i++) {
		p = (unsigned char *)&lp[i];
		p[0] = 0;
		p[1] = *cp++;
		p[2] = *cp++;
		p[3] = *cp++;
	}
}

static void
iaddr_to_disk(cp, lp)
register char *cp;
register daddr_t *lp;
{
	register i32 i;
	register unsigned char *p;

	for(i=0; i<NADDR; i++) {
		p = (unsigned char *)&lp[i];
		if(p[0] != 0)
			printf("iaddress > 2^24\n");
		*cp++ = p[1];
		*cp++ = p[2];
		*cp++ = p[3];
	}
}

/*
 * Look up an inode by device,inumber.
 * If it is in core (in the inode structure),
 * honor the locking protocol.
 * If it is not in core, read it in from the
 * specified device.
 * If the inode is mounted on, perform
 * the indicated indirection.
 * In all cases, a pointer to a locked
 * inode structure is returned.
 *
 * printf warning: no inodes -- if the inode
 *	structure is full
 * panic: no imt -- if the mounted file
 *	system is not in the mount table.
 *	"cannot happen"
 */
struct inode *
iget(dev, ino)
i32 dev;
i32 ino;
{
	register struct inode *ip;
	register struct mount *mp;
	register struct inode *oip;
	register struct buf *bp;
	register struct dinode *dp;

loop:
	oip = NULL;
	for(ip = &inode[0]; ip < &inode[NINODE]; ip++) {
		if(ino == ip->i_number && dev == ip->i_dev) {
			if((ip->i_flag&ILOCK) != 0) {
				ip->i_flag |= IWANT;
				sleep((caddr_t)ip, PINOD);
				goto loop;
			}
			if((ip->i_flag&IMOUNT) != 0) {
				for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++) {
					if(mp->m_inodp == ip) {
						dev = mp->m_dev;
						ino = ROOTINO;
						goto loop;
					}
				}
				panic("no imt");
			}
			ip->i_count++;
			ip->i_flag |= ILOCK;
			return(ip);
		}
		if(oip==NULL && ip->i_count==0)
			oip = ip;
	}
	ip = oip;
	if(ip == NULL) {
		printf("Inode table overflow\n"); 
		u.u_error = ENFILE;
		return(NULL);
	}
	ip->i_dev = dev;
	ip->i_number = ino;
	ip->i_flag = ILOCK;
	ip->i_count++;
	ip->i_un.i_file.i_lastr = 0;
	bp = bread(dev, itod(ino));
	/*
	 * Check I/O errors
	 */
	if((bp->b_flags&B_ERROR) != 0) {
		brelse(bp);
		iput(ip);
		return(NULL);
	}
	dp = bp->b_un.b_dino;
	dp += itoo(ino);
	if(ino == 11) {
	}
	iexpand(ip, dp);
	if(ino == 11) {
	}
	brelse(bp);
	return(ip);
}

void iexpand(ip, dp)
register struct inode *ip;
register struct dinode *dp;
{

	ip->i_mode = dp->di_mode;
	ip->i_nlink = dp->di_nlink;
	ip->i_uid = dp->di_uid;
	ip->i_gid = dp->di_gid;
	ip->i_size = dp->di_size;
	iaddr_from_disk(ip->i_un.i_file.i_addr, dp->di_addr);
}

/*
 * Decrement reference count of
 * an inode structure.
 * On the last reference,
 * write the inode out and if necessary,
 * truncate and deallocate the file.
 */
void iput(ip)
register struct inode *ip;
{

	if(ip->i_count == 1) {
		ip->i_flag |= ILOCK;
		if(ip->i_nlink <= 0) {
			itrunc(ip);
			ip->i_mode = 0;
			ip->i_flag |= IUPD|ICHG;
			ifree(ip->i_dev, ip->i_number);
		}
		iupdat(ip, &time, &time);
		prele(ip);
		ip->i_flag = 0;
		ip->i_number = 0;
	}
	ip->i_count--;
	prele(ip);
}

/*
 * Check accessed and update flags on
 * an inode structure.
 * If any are on, update the inode
 * with the current time.
 */
void iupdat(ip, ta, tm)
	register struct inode *ip;
	time_t *ta, *tm;
{
	register struct buf *bp;
	register struct dinode *dp;

	if((ip->i_flag&(IUPD|IACC|ICHG)) != 0) {
		if(getfs(ip->i_dev)->s_ronly)
			return;
		bp = bread(ip->i_dev, itod(ip->i_number));
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			return;
		}
		dp = bp->b_un.b_dino;
		dp += itoo(ip->i_number);
		dp->di_mode = ip->i_mode;
		dp->di_nlink = ip->i_nlink;
		dp->di_uid = ip->i_uid;
		dp->di_gid = ip->i_gid;
		dp->di_size = ip->i_size;
		iaddr_to_disk(dp->di_addr, ip->i_un.i_file.i_addr);
		if(ip->i_flag&IACC)
			dp->di_atime = *ta;
		if(ip->i_flag&IUPD)
			dp->di_mtime = *tm;
		if(ip->i_flag&ICHG)
			dp->di_ctime = time;
		ip->i_flag &= ~(IUPD|IACC|ICHG);
		bdwrite(bp);
	}
}

/*
 * Free all the disk blocks associated
 * with the specified inode structure.
 * The blocks of the file are removed
 * in reverse order. This FILO
 * algorithm will tend to maintain
 * a contiguous free list much longer
 * than FIFO.
 */
void itrunc(ip)
register struct inode *ip;
{
	register i;
	dev_t dev;
	daddr_t bn;

	i = ip->i_mode & IFMT;
	if (i!=IFREG && i!=IFDIR)
		return;
	dev = ip->i_dev;
	for(i=NADDR-1; i>=0; i--) {
		bn = ip->i_un.i_file.i_addr[i];
		if(bn == (daddr_t)0)
			continue;
		ip->i_un.i_file.i_addr[i] = (daddr_t)0;
		switch(i) {

		default:
			free(dev, bn);
			break;

		case NADDR-3:
			tloop(dev, bn, 0, 0);
			break;

		case NADDR-2:
			tloop(dev, bn, 1, 0);
			break;

		case NADDR-1:
			tloop(dev, bn, 1, 1);
		}
	}
	ip->i_size = 0;
	ip->i_flag |= ICHG|IUPD;
}

void tloop(dev, bn, f1, f2)
i32 dev;
daddr_t bn;
i32 f1;
i32 f2;
{
	register i;
	register struct buf *bp;
	register daddr_t *bap;
	daddr_t nb;

	bp = NULL;
	for(i=NINDIR-1; i>=0; i--) {
		if(bp == NULL) {
			bp = bread(dev, bn);
			if (bp->b_flags & B_ERROR) {
				brelse(bp);
				return;
			}
			bap = bp->b_un.b_daddr;
		}
		nb = bap[i];
		if(nb == (daddr_t)0)
			continue;
		if(f1) {
			brelse(bp);
			bp = NULL;
			tloop(dev, nb, f2, 0);
		} else
			free(dev, nb);
	}
	if(bp != NULL)
		brelse(bp);
	free(dev, bn);
}

/*
 * Make a new file.
 */
struct inode *
maknode(mode)
i32 mode;
{
	register struct inode *ip;

	ip = ialloc(u.u_pdir->i_dev);
	if(ip == NULL) {
		iput(u.u_pdir);
		return(NULL);
	}
	ip->i_flag |= IACC|IUPD|ICHG;
	if((mode&IFMT) == 0)
		mode |= IFREG;
	ip->i_mode = mode & ~u.u_cmask;
	ip->i_nlink = 1;
	ip->i_uid = u.u_uid;
	ip->i_gid = u.u_gid;
	wdir(ip);
	return(ip);
}

/*
 * Write a directory entry with
 * parameters left as side effects
 * to a call to namei.
 */
void wdir(ip)
struct inode *ip;
{

	if (u.u_pdir->i_nlink <= 0) {
		u.u_error = ENOTDIR;
		goto out;
	}
	u.u_dent.d_ino = ip->i_number;
	bcopy((caddr_t)u.u_dbuf, (caddr_t)u.u_dent.d_name, DIRSIZ);
	u.u_count = sizeof(struct direct);
	u.u_segflg = 1;
	u.u_base = (caddr_t)&u.u_dent;
	writei(u.u_pdir);
out:
	iput(u.u_pdir);
}
