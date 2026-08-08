/*
 *	ps - process status
 *	examine and print certain things about processes
 */

#include <stdio.h>
#include <core.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/tty.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KSYMS "/etc/ksyms"
#define KMEM "/dev/kmem"
#define MEM "/dev/mem"
#define EPOCH68_USER_PAGE_SIZE 0x40000L
#define EPOCH68_EXECARGS 0x3fff8L

struct	proc mproc;

struct	user u;
int	chkpid;
int	retcode=1;
int	lflg;
int	vflg;
int	kflg;
int	xflg;
char	*tptr;
char	*gettty(void);
char	*getptr(char **adr);
static int getbyte(char *adr);
static int getdev(void);
static int prcom(int puid);
int	aflg;
int	kmem;
int	mem;
long	procaddr;
long	uareaaddr;

int	ndev;
struct devl {
	char	dname[DIRSIZ];
	dev_t	dev;
} devl[256];

static int ksym(char *file, char *name, long *valuep);
static int kread(long addr, char *buf, int len);

int
main(int argc, char **argv)
{
	int i;
	char *ap;
	int uid, puid;

	if (argc>1) {
		ap = argv[1];
		while (*ap) switch (*ap++) {

		case 'v':
			vflg++;
			break;

		case 'a':
			aflg++;
			break;

		case 't':
			if(*ap)
				tptr = ap;
			aflg++;
			if (*tptr == '?')
				xflg++;
			goto bbreak;

		case 'x':
			xflg++;
			break;

		case '-':
			break;

		case 'l':
			lflg++;
			break;

		case 'k':
			kflg++;
			break;

		default:
			chkpid = atoi(ap-1);
			goto bbreak;
			break;
		}
	}

bbreak:
	if(chdir("/dev") < 0) {
		fprintf(stderr, "Can't change to /dev\n");
		exit(1);
	}
	if (kflg) {
		fprintf(stderr, "ps: crash dumps are not supported on Epoch68\n");
		exit(1);
	}
	if (ksym(argc>2? argv[2]:KSYMS, "_proc", &procaddr) < 0 ||
	    ksym(argc>2? argv[2]:KSYMS, "_uarea", &uareaaddr) < 0) {
		fprintf(stderr, "No namelist\n");
		exit(1);
	}
	if ((kmem = open(KMEM, 0)) < 0) {
		fprintf(stderr, "No kmem\n");
		exit(1);
	}
	if ((mem = open(MEM, 0)) < 0) {
		fprintf(stderr, "No mem\n");
		exit(1);
	}
	getdev();
	uid = getuid();
	if (lflg)
	printf(" F S UID   PID  PPID CPU PRI NICE  ADDR  SZ    WCHAN TTY TIME CMD\n"); else
		if (chkpid==0) printf("   PID TTY TIME CMD\n");
	for (i=0; i<NPROC; i++) {
		if (kread(procaddr + (long)i*sizeof(mproc),
		    (char *)&mproc, sizeof(mproc)) < 0)
			continue;
		if (mproc.p_stat==0)
			continue;
		if (mproc.p_pgrp==0 && xflg==0 && mproc.p_uid==0)
			continue;
		puid = mproc.p_uid;
		if ((uid != puid && aflg==0) ||
		    (chkpid!=0 && chkpid!=mproc.p_pid))
			continue;
		if (kread(uareaaddr + (long)i*sizeof(u),
		    (char *)&u, sizeof(u)) < 0)
			continue;
		if(prcom(puid)) {
			printf("\n");
			retcode=0;
		}
	}
	exit(retcode);
}

static int
getdev(void)
{
	register FILE *df;
	struct stat sbuf;
	struct direct dbuf;

	if ((df = fopen("/dev", "r")) == NULL) {
		fprintf(stderr, "Can't open /dev\n");
		exit(1);
	}
	ndev = 0;
	while (fread((char *)&dbuf, sizeof(dbuf), 1, df) == 1) {
		if(dbuf.d_ino == 0)
			continue;
		if(stat(dbuf.d_name, &sbuf) < 0)
			continue;
		if ((sbuf.st_mode&S_IFMT) != S_IFCHR)
			continue;
		strcpy(devl[ndev].dname, dbuf.d_name);
		devl[ndev].dev = sbuf.st_rdev;
		ndev++;
	}
	fclose(df);
	return(0);
}
static int
prcom(int puid)
{
	char abuf[512];
	long addr;
	register i32 *ip;
	register char *cp, *cp1;
	long tm;
	int c, nbad;
	register char *tp;
	int lw=(lflg?35:80);
	char **ap;

	addr = (long)mproc.p_addr * EPOCH68_USER_PAGE_SIZE;

	tp = gettty();
	if (tptr && strncmp(tptr, tp, 2))
		return(0);
	if (lflg) {
		printf("%2o %c%4d", mproc.p_flag,
			"0SWRIZT"[mproc.p_stat], puid);
	}
	printf("%6u", mproc.p_pid);
	if (lflg) {
		printf("%6u%4d%4d%5d%6o%4d ", mproc.p_ppid, mproc.p_cpu&0377,
			mproc.p_pri,
			mproc.p_nice,
			mproc.p_addr, (mproc.p_size+7)>>3);
		if (mproc.p_wchan)
			printf("%8lo", (long)mproc.p_wchan);
		else
			printf("        ");
	}
	printf(" %-2.2s", tp);
	if (mproc.p_stat==SZOMB) {
		printf("  <defunct>");
		return(1);
	}
	tm = (u.u_utime + u.u_stime + 30)/60;
	printf(" %2ld:", tm/60);
	tm %= 60;
	printf(tm<10?"0%ld":"%ld", tm);
	if (vflg && lflg==0) {	/* 0 == old tflg (print long times) */
		tm = (u.u_cstime + 30)/60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
		tm = (u.u_cutime + 30)/60;
		printf(" %2ld:", tm/60);
		tm %= 60;
		printf(tm<10?"0%ld":"%ld", tm);
	}
	if (mproc.p_pid == 0) {
		printf(" swapper");
		return(1);
	}
	/* look for sh special */
	lseek(mem, addr+EPOCH68_EXECARGS, 0);
	if (read(mem, (char *)&ap, sizeof(ap)) != sizeof(ap))
		return(1);
	if (ap && (unsigned long)ap < EPOCH68_USER_PAGE_SIZE) {
		char b[82];
		char *bp = b;
		while((cp=getptr(ap++)) && cp && (bp<b+lw) ) {
			nbad = 0;
			while((c=getbyte(cp++)) && (bp<b+lw)) {
				if (c<' ' || c>'~') {
					if (nbad++>3)
						break;
					continue;
				}
				*bp++ = c;
			}
			*bp++ = ' ';
		}
		*bp = 0;
		if (bp > b) {
			printf(lflg?" %.30s":" %.60s", b);
			return(1);
		}
	}

	lseek(mem, addr+EPOCH68_USER_PAGE_SIZE-sizeof(abuf), 0);
	if (read(mem, abuf, sizeof(abuf)) != sizeof(abuf))
		return(1);
	for (ip = (i32 *)&abuf[512]-2; ip > (i32 *)abuf; ) {
		if (*--ip == -1 || *ip==0) {
			cp = (char *)(ip+1);
			if (*cp==0)
				cp++;
			nbad = 0;
			for (cp1 = cp; cp1 < &abuf[512]; cp1++) {
				c = *cp1&0177;
				if (c==0) {
					if (cp1+1 == &abuf[512] || cp1[1]==0) {
						*cp1 = 0;
						break;
					}
					*cp1 = ' ';
				}
				else if (c < ' ' || c > 0176) {
					if (++nbad >= 5) {
						*cp1++ = ' ';
						break;
					}
					*cp1 = '?';
				} else if (c=='=') {
					*cp1 = 0;
					while (cp1>cp && *--cp1!=' ')
						*cp1 = 0;
					break;
				}
			}
			while (cp1 > cp && cp1[-1]==' ')
				*--cp1 = 0;
			if (cp1 > cp) {
				printf(lflg?" %.30s":" %.60s", cp);
				return(1);
			}
		}
	}
	if (u.u_comm[0])
		printf(" %.14s", u.u_comm);
	return(1);
}

char *
gettty(void)
{
	register i;
	register char *p;

	if (u.u_ttyp==0)
		return("?");
	for (i=0; i<ndev; i++) {
		if (devl[i].dev == u.u_ttyd) {
			p = devl[i].dname;
			if (p[0]=='t' && p[1]=='t' && p[2]=='y')
				p += 3;
			return(p);
		}
	}
	return("?");
}

char *
getptr(char **adr)
{
	char *ptr;
	register char *p, *pa;
	register i;

	ptr = 0;
	pa = (char *)adr;
	p = (char *)&ptr;
	for (i=0; i<sizeof(ptr); i++)
		*p++ = getbyte(pa++);
	return(ptr);
}

static int
getbyte(char *adr)
{
	char b;
	long saddr;
	u32 vaddr;

	vaddr = (u32)adr;
	if (vaddr >= EPOCH68_USER_PAGE_SIZE)
		return(0);
	saddr = (long)mproc.p_addr * EPOCH68_USER_PAGE_SIZE + vaddr;
	if(lseek(mem, saddr, 0)==-1
		   || read(mem, &b, 1)<1) {
		return(0);
	}
	return((unsigned)b);
}

static int
ksym(char *file, char *name, long *valuep)
{
	FILE *fp;
	char line[96], sym[64];
	register char *cp, *sp;
	long value;

	if ((fp = fopen(file, "r")) == NULL)
		return(-1);
	while (fgets(line, sizeof(line), fp) != NULL) {
		cp = line;
		while (*cp == ' ' || *cp == '\t')
			cp++;
		sp = sym;
		while (*cp && *cp != ' ' && *cp != '\t' && *cp != ':' &&
		    sp < &sym[sizeof(sym)-1])
			*sp++ = *cp++;
		*sp = 0;
		while (*cp && *cp != '\n' &&
		    !((*cp >= '0' && *cp <= '9') ||
		    (cp[0] == '0' && cp[1] == 'x')))
			cp++;
		while (*cp == ' ' || *cp == '\t')
			cp++;
		value = 0;
		if (cp[0] == '0' && cp[1] == 'x')
			cp += 2;
		while ((*cp >= '0' && *cp <= '9') ||
		    (*cp >= 'a' && *cp <= 'f') ||
		    (*cp >= 'A' && *cp <= 'F')) {
			value <<= 4;
			if (*cp >= '0' && *cp <= '9')
				value += *cp - '0';
			else if (*cp >= 'a' && *cp <= 'f')
				value += *cp - 'a' + 10;
			else
				value += *cp - 'A' + 10;
			cp++;
		}
		if (strcmp(sym, name) == 0) {
			fclose(fp);
			*valuep = value;
			return(0);
		}
	}
	fclose(fp);
	return(-1);
}

static int
kread(long addr, char *buf, int len)
{
	if (lseek(kmem, addr, 0) < 0)
		return(-1);
	if (read(kmem, buf, len) != len)
		return(-1);
	return(0);
}
