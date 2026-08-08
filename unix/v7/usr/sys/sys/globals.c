#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/inode.h"
#include "../h/proc.h"
#include "../h/seg.h"
#include "../h/map.h"
#include "../h/mount.h"

char	canonb[CANBSIZ];
struct inode *rootdir;
struct proc *runq;
i32	cputype;
i32	lbolt;
time_t	time;

i32	nblkdev;
i32	nchrdev;
i32	mpid;
char	runin;
char	runout;
char	runrun;
char	curpri;
i32	maxmem;
physadr	lks;
volatile u32 kticks;
i32	updlock;
daddr_t	rablock;
char	msgbuf[MSGBUFS];

i32	dk_busy;
long	dk_time[32];
long	dk_numb[3];
long	dk_wds[3];
long	tk_nin;
long	tk_nout;

struct map coremap[CMAPSIZ];
struct map swapmap[SMAPSIZ];
struct map userpagemap[CMAPSIZ];
struct inode *mpxip;
physadr	ka6;

/* struct user u; */
/* the plain old u will still be accessible as usual from a #define u (*uarrp) */
extern struct user uarea[NPROC];	/* user area for each process held in kernel */
struct user *uarrp = &uarea[0];	/* pointer to current user area */
extern unsigned long ukstack[NPROC][EPOCH68_KSTACK_LONGS];
char epoch68_uarea_size_check[(((long)sizeof(struct user) * (long)NPROC) == 46368L) ? 1 : -1];
char epoch68_ukstack_size_check[(((long)sizeof(unsigned long) * (long)NPROC * (long)EPOCH68_KSTACK_LONGS) == 129024L) ? 1 : -1];

struct mount mount[NMOUNT];
