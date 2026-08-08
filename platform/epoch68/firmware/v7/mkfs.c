/*
 * Make a simple empty V7 filesystem.
 */
#include "../string.h"
#include "../v7time.h"
#include "v7io.h"
#include <sys/dir.h>
#include <sys/fblk.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/param.h>

#pragma dontwarn 213

/*
 * Global mkfs state. This file is still structured like the old V7 tool:
 * helpers communicate through shared state rather than an explicit context
 * object.
 */
i32 fsfd = -1; /* raw read/write descriptor for the target device */
ino_t ino;    /* inode number currently being assigned */

/*
 * Working buffers and on-disk structures under construction.
 */
char buf[BSIZE];
struct fblk fbuf;
struct filsys filsys;

/*
 * External helpers provided by the firmware/V7 runtime.
 */
void ltol3(char *cp, i32 *lp, i32 n);

/*
 * Public entry point.
 */
i32 makefsv7(char *devspec, i32 nblocks);

/*
 * Local mkfs helpers.
 */
static daddr_t alloc(void);
static void bflist(void);
static void bfree(daddr_t bno);
static void iput(struct inode *ip, i32 *aibc, daddr_t *ib);
static void rdfs(daddr_t bno, char *bf);
static void write_root(void);
static void wtfs(daddr_t bno, char *bf);

#define MKFS_MIN_BLOCKS 2048L
#define MKFS_MAX_BLOCKS (1024L * 1024L * 1024L / BSIZE)
#define MKFS_MAX_ISIZE_BLOCKS 8192

i32
makefsv7(char *devspec, i32 nblocks)
{
	i32 n;

	v7io_close_all();

	if (nblocks < MKFS_MIN_BLOCKS || nblocks > MKFS_MAX_BLOCKS) {
		printf("mkfs: bad block count\n");
		return -1;
	}
	n = nblocks / 25;
	if (n > MKFS_MAX_ISIZE_BLOCKS - 2)
		n = MKFS_MAX_ISIZE_BLOCKS - 2;
	filsys.s_fsize = nblocks;
	filsys.s_isize = 2 + n;
	filsys.s_m = 1; /* No interleave */
	filsys.s_n = 1;
	filsys.s_tfree = 0;
	filsys.s_tinode = 0;

	fsfd = open(devspec, 2);
	if (fsfd < 0)
		_stop("mkfs: open failed");

	memset(buf, 0, BSIZE);
	for (n = 2; n != filsys.s_isize; n++) {
		wtfs(n, buf);
		filsys.s_tinode += INOPB;
	}

	ino = 0;
	bflist();
	write_root();
	filsys.s_time = EPOCH68_V7_SUPER_TIME;
	wtfs((daddr_t)1, (char *)&filsys);
	close(fsfd);
	fsfd = -1;
	return 0;
}

static void
rdfs(bno, bf)
daddr_t bno;
char *bf;
{
  i32 n;

	lseek(fsfd, bno * BSIZE, 0);
	n = read(fsfd, bf, BSIZE);
	if (n != BSIZE) {
		printf("read error: %D\n", bno);
		_stop("mkfs: read failed");
	}
}

static void
wtfs(bno, bf)
daddr_t bno;
char *bf;
{
  i32 n;

	lseek(fsfd, bno * BSIZE, 0);
	n = write(fsfd, bf, BSIZE);
	if (n != BSIZE) {
		printf("write error: %D\n", bno);
		_stop("mkfs: write failed");
	}
}

static daddr_t
alloc()
{
  i32 i;
  daddr_t bno;

  filsys.s_tfree--;
  bno = filsys.s_free[--filsys.s_nfree];
  if (filsys.s_nfree <= 0) {
    rdfs(bno, (char *)&fbuf);
    filsys.s_nfree = fbuf.df_nfree;
    for (i = 0; i < NICFREE; i++)
      filsys.s_free[i] = fbuf.df_free[i];
  }
  return (bno);
}

static void
bfree(bno)
daddr_t bno;
{
  i32 i;

  if (bno != (daddr_t)0)
    filsys.s_tfree++;
  if (filsys.s_nfree >= NICFREE) {
    fbuf.df_nfree = filsys.s_nfree;
    for (i = 0; i < NICFREE; i++)
      fbuf.df_free[i] = filsys.s_free[i];
    wtfs(bno, (char *)&fbuf);
    filsys.s_nfree = 0;
  }
  filsys.s_free[filsys.s_nfree++] = bno;
}

static void
bflist()
{
  struct inode in;
  daddr_t ib[NINDIR];
  i32 ibc;
  i32 i;
  daddr_t f;

  ino++;
  memset(&in, 0, sizeof(in));
  in.i_number = ino;
  in.i_mode = IFREG;

  memset(ib, 0, sizeof(ib));
  ibc = 0;
  bfree((daddr_t)0);
  for (f = filsys.s_fsize - 1; f >= filsys.s_isize; f--)
    bfree(f);
  iput(&in, &ibc, ib);
}

static void
iput(ip, aibc, ib)
struct inode *ip;
i32 *aibc;
daddr_t *ib;
{
  struct dinode *dp;
  daddr_t d;
  i32 i;

  filsys.s_tinode--;
  d = itod(ip->i_number);
  rdfs(d, buf);
  dp = (struct dinode *)buf;
  dp += itoo(ip->i_number);
  memset(dp, 0, sizeof(*dp));

  dp->di_mode = ip->i_mode;
  dp->di_nlink = ip->i_nlink;
  dp->di_uid = ip->i_uid;
  dp->di_gid = ip->i_gid;
  dp->di_size = ip->i_size;
  dp->di_atime = EPOCH68_V7_FILE_TIME;
  dp->di_mtime = EPOCH68_V7_FILE_TIME;
  dp->di_ctime = EPOCH68_V7_FILE_TIME;
  for (i = 0; i < *aibc; i++) {
    if (i >= NADDR)
      break;
    ip->i_un.i_file.i_addr[i] = ib[i];
  }
  ltol3(dp->di_addr, ip->i_un.i_file.i_addr, NADDR);
  wtfs(d, buf);
}

static void
write_root()
{
  struct inode in;
  struct direct *dp;
  daddr_t ib[NINDIR];
  daddr_t bno;
  i32 i;
  i32 ibc;

  ino++;
  {
    char *zp;
    zp = (char *)&in;
    for (i = 0; i < sizeof(in); i++)
      zp[i] = 0;
  }
  in.i_number = ino;
  in.i_mode = IFDIR | 0777;
  in.i_nlink = 2;
  in.i_size = 2 * sizeof(struct direct);
  for (i = 0; i < NINDIR; i++)
    ib[i] = 0;
  for (i = 0; i < BSIZE; i++)
    buf[i] = 0;

  dp = (struct direct *)buf;
  dp[0].d_ino = in.i_number;
  for (i = 0; i < DIRSIZ; i++)
    dp[0].d_name[i] = 0;
  dp[0].d_name[0] = '.';

  dp[1].d_ino = in.i_number;
  for (i = 0; i < DIRSIZ; i++)
    dp[1].d_name[i] = 0;
  dp[1].d_name[0] = '.';
  dp[1].d_name[1] = '.';

  bno = alloc();
  wtfs(bno, buf);
  ib[0] = bno;
  ibc = 1;
  iput(&in, &ibc, ib);
}
