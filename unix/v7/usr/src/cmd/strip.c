#include <a.out.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char	*tname;
char	*mktemp(char *template);
struct exec head;
int	status;
int	tf;

static void strip(char *name);
static int copy(char *name, int fr, int to, long size);

int
main (int argc, char *argv[])
{
	register i;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	tname = mktemp("/tmp/sXXXXX");
	close(creat(tname, 0600));
	tf = open(tname, 2);
	if(tf < 0) {
		printf("cannot create temp file\n");
		exit(2);
	}
	for(i=1; i<argc; i++) {
		strip(argv[i]);
		if(status > 1)
			break;
	}
	close(tf);
	unlink(tname);
	exit(status);
}

static void
strip (char *name)
{
	register f;
	long size;

	f = open(name, 0);
	if(f < 0) {
		printf("cannot open %s\n", name);
		status = 1;
		goto out;
	}
	read(f, (char *)&head, sizeof(head));
	if(A_GETMID(head) != A_MID_SUN010 || A_GETMAGIC(head) != A_MAGIC5) {
		printf("%s not in a.out format\n", name);
		status = 1;
		goto out;
	}
	if(head.a_syms == 0) {
		printf("%s already stripped\n", name);
		goto out;
	}
	size = (long)head.a_text + head.a_data;
	head.a_syms = 0;
	head.a_unused = 0;
	head.a_flag = 0;

	lseek(tf, (long)0, 0);
	write(tf, (char *)&head, sizeof(head));
	if(copy(name, f, tf, size - sizeof(head))) {
		status = 1;
		goto out;
	}
	close(f);
	f = creat(name, 0666);
	if(f < 0) {
		printf("%s cannot recreate\n", name);
		status = 1;
		goto out;
	}
	lseek(tf, (long)0, 0);
	if(copy(name, tf, f, size))
		status = 2;

out:
	close(f);
}

static int
copy (char *name, int fr, int to, long size)
{
	register s, n;
	char buf[512];

	while(size != 0) {
		s = 512;
		if(size < 512)
			s = size;
		n = read(fr, buf, s);
		if(n != s) {
			printf("%s unexpected eof\n", name);
			return(1);
		}
		n = write(to, buf, s);
		if(n != s) {
			printf("%s unexpected write eof\n", name);
			return(1);
		}
		size -= s;
	}
	return(0);
}
