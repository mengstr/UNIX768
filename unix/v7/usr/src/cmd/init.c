#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <utmp.h>
#include <setjmp.h>
#include <unistd.h>
#include <local.h>

struct tab;

void shutdown(void);
void single(void);
void runcom(void);
void multiple(void);
void term(struct tab *p);
i32 rline(void);
void maktty(char *lin);
i32 get(void);
void merge(void);
void dfork(struct tab *p);
void rmut(struct tab *p);
void reset(i16 signo);
i32 idletrap(void);
void default_console(void);

#define	TABSIZ	100
#define	ALL	p = &itab[0]; p < &itab[TABSIZ]; p++
#define	EVER	;;

char	shell[]	= "/bin/osh";
char	runshell[] = "/bin/sh";
char	getty[]	 = "/etc/getty";
char	minus[]	= "-";
char	runc[]	= "/etc/rc";
char	ifile[]	= "/etc/ttys";
char	utmp[]	= "/etc/utmp";
char	wtmpf[]	= "/usr/adm/wtmp";
char	ctty[]	= "/dev/console";
char	dev[]	= "/dev/";

struct utmp wtmp;
struct
{
	char	line[8];
	char	comn;
	char	flag;
} line;
struct	tab
{
	char	line[8];
	char	comn;
	i32	pid;
} itab[TABSIZ];

i32	fi;
char	tty[20];
jmp_buf	sjbuf;
i32	initial_multi;

int
main(int argc, char **argv)
{
	initial_multi = argc >= 2 && strcmp(argv[1], "-m") == 0;
	setjmp(sjbuf);
	signal(SIGHUP, reset);
	for(EVER) {
		shutdown();
		if(initial_multi)
			initial_multi = 0;
		else
			single();
		runcom();
		merge();
		multiple();
	}
    return 0;
}

void shutdown(void)
{
	register i32 i;
	register struct tab *p;

	signal(SIGINT, SIG_IGN);
	for(ALL)
		term(p);
	signal(SIGALRM, reset);
	alarm(60);
	for(i=0; i<5; i++)
		kill(-1, SIGKILL);
	while(wait((i16 *)0) != -1)
		;
	alarm(0);
	signal(SIGALRM, SIG_DFL);
	for(i=0; i<10; i++)
		close(i);
}

void single(void)
{
	register i32 pid;

	pid = fork();
	if(pid == 0) {
/*
		alarm(300);
*/
		signal(SIGHUP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGALRM, SIG_DFL);
		open(ctty, 2);
		dup(0);
		dup(0);
		execl(shell, minus, (char *)0);
		exit(0);
	}
	while(wait((i16 *)0) != pid)
		;

}
void runcom(void)
{
	register i32 pid;

	pid = fork();
	if(pid == 0) {
		open("/", 0);
		dup(0);
		dup(0);
		execl(runshell, runshell, runc, (char *)0);
		exit(0);
	}
	while(wait((i16 *)0) != pid)
		;
}

void multiple(void)
{
	register struct tab *p;
	register i32 pid;

	for(EVER) {
		pid = wait((i16 *)0);
		if(pid == -1)
			return;
		for(ALL)
			if(p->pid == pid || p->pid == -1) {
				rmut(p);
				p->pid = 0;
				dfork(p);
				break;
			}
	}
}

void
term(register struct tab *p)
{
	if(p->pid != 0) {
		rmut(p);
		kill(p->pid, SIGKILL);
	}
	p->pid = 0;
	p->line[0] = 0;
}

i32 rline()
{
	register i32 c, i;

	c = get();
	if(c < 0)
		return(0);
	if(c == 0)
		goto bad;
	line.flag = c;
	c = get();
	if(c <= 0)
		goto bad;
	line.comn = c;
	for(i=0; i<8; i++)
		line.line[i] = 0;
	for(i=0; i<7; i++) {
		c = get();
		if(c <= 0)
			break;
		line.line[i] = c;
	}
	while(c > 0)
		c = get();
	return(1);

bad:
	line.flag = '0';
	return(1);
}

void maktty(lin)
char *lin;
{
	register i32 i, j;


	for(i=0; dev[i]; i++)
		tty[i] = dev[i];
	for(j=0; lin[j]; j++) {
		tty[i] = lin[j];
		i++;
	}
	tty[i] = 0;
}

i32 get()
{
	char b;

	if(read(fi, &b, 1) != 1)
		return(-1);
	if(b == '\n')
		return(0);
	return(b);
}

void default_console()
{
	register struct tab *q;

	for(q = &itab[0]; q < &itab[TABSIZ]; q++) {
		q->line[0] = 0;
		q->comn = 0;
		q->pid = 0;
	}

	q = &itab[0];
	strcpy(q->line, "console");
	q->comn = '4';
	q->pid = 0;
}

void
merge(void)
{
	/* Clear out the old internal table first to prevent memory corruption */
	register struct tab *p, *q;
	register i32 i;
	register i32 loaded;


	for(q = &itab[0]; q < &itab[TABSIZ]; q++) {
		q->line[0] = 0;
		q->comn = 0;
		q->pid = 0;
	}

	loaded = 0;
	fi = open(ifile, 0);
	if(fi >= 0) {
		q = &itab[0];
		while(q < &itab[TABSIZ] && rline()) {
			if(line.flag == '0') {
				continue;
			}
			for(i=0; i<8; i++) {
				q->line[i] = line.line[i];
			}
			q->comn = line.comn;
			q->pid = 0;
			q++;
			loaded++;
		}
		close(fi);
	}

	if(loaded == 0)
		default_console();

	for(ALL) {
		if(p->line[0] != 0 && p->pid == 0) {
			dfork(p);
		}
	}
}

void
dfork(struct tab *p)
{
	register i32 pid;

	pid = fork();
	if(pid == 0) {
		signal(SIGHUP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		maktty(p->line);
		chown(tty, 0, 0);
		chmod(tty, 0622);
		open(tty, 2);
		dup(0);
		dup(0);
		tty[0] = p->comn;
		tty[1] = 0;
		execl(getty, minus, tty, (char *)0);
		exit(0);
	}
	p->pid = pid;
}

void
rmut(register struct tab *p)
{
	register i32 i, f;

	f = open(utmp, 2);
	if(f >= 0) {
		while(read(f, (char *)&wtmp, sizeof(wtmp)) == sizeof(wtmp)) {
			for(i=0; i<8; i++)
				if(wtmp.ut_line[i] != p->line[i])
					goto contin;
			lseek(f, -(i32)sizeof(wtmp), 1);
			for(i=0; i<8; i++)
				wtmp.ut_name[i] = 0;
			time(&wtmp.ut_time);
			write(f, (char *)&wtmp, sizeof(wtmp));
		contin:;
		}
		close(f);
	}
	f = open(wtmpf, 1);
	if (f >= 0) {
		for(i=0; i<8; i++) {
			wtmp.ut_name[i] = 0;
			wtmp.ut_line[i] = p->line[i];
		}
		time(&wtmp.ut_time);
		lseek(f, (i32)0, 2);
		write(f, (char *)&wtmp, sizeof(wtmp));
		close(f);
	}
}

void reset(i16 signo)
{
	(void)signo;
	longjmp(sjbuf, 1);
}
