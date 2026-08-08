#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <unistd.h>

daddr_t	blkno	= 1;
char	*dargv[] = {
	0,
	"/dev/rd0",
	"/dev/sd0",
	0
};


struct	filsys sblock;

int	fi;

static void dfree(char *file);
static daddr_t alloc(void);
static void bread(daddr_t bno, char *buf, i32 cnt);

int
main(int argc, char **argv)
{
	int i;

	if(argc <= 1) {
		for(argc = 1; dargv[argc]; argc++);
		argv = dargv;
	}

	for(i=1; i<argc; i++) {
		dfree(argv[i]);
	}
	return(0);
}

static void
dfree(file)
char *file;
{
	daddr_t i;

	blkno = 1;
	fi = open(file, 0);
	if(fi < 0) {
		fprintf(stderr,"cannot open %s\n", file);
		return;
	}
	sync();
	bread(1L, (char *)&sblock, sizeof(sblock));
	i = 0;
	while(alloc())
		i++;
	printf("%s %ld\n", file, i);
	close(fi);
}

static daddr_t
alloc()
{
	int i;
	daddr_t b;
	union {
		char data[BSIZE];
		struct fblk fb;
	} buf;

	if(sblock.s_nfree <= 0)
		return(0);
	if(sblock.s_nfree > NICFREE) {
		printf("bad free count, b=%ld\n", blkno);
		return(0);
	}
	i = --sblock.s_nfree;
	b = sblock.s_free[i];
	sblock.s_free[i] = (daddr_t)0;
	if(b == 0)
		return(0);
	if(b<sblock.s_isize || b>=sblock.s_fsize) {
		printf("bad free block (%ld)\n", b);
		return(0);
	}
	if(sblock.s_nfree <= 0) {
		bread(b, buf.data, BSIZE);
		blkno = b;
		sblock.s_nfree = buf.fb.df_nfree;
		if(sblock.s_nfree < 0 || sblock.s_nfree > NICFREE) {
			printf("bad free count, b=%ld\n", blkno);
			sblock.s_nfree = 0;
			return(0);
		}
		for(i=0; i<NICFREE; i++)
			sblock.s_free[i] = buf.fb.df_free[i];
	}
	return(b);
}

static void
bread(bno, buf, cnt)
daddr_t bno;
char *buf;
i32 cnt;
{
	int n;

	lseek(fi, bno<<BSHIFT, 0);
	if((n=read(fi, buf, cnt)) != cnt) {
		printf("read error %ld\n", bno);
		printf("count = %d; errno = %d\n", n, errno);
		exit(0);
	}
}
