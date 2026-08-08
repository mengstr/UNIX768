/*
 * login [ name ]
 */

#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <local.h>
#include <utmp.h>

#define SCPYN(a, b)	strncpy(a, b, sizeof(a))

#include <sgtty.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>

struct passwd *getpwnam(char *name);
int setpwent(void);
char *rindex(char *sp, i32 c);
char *ttyname(i32 fd);
int ttyslot(void);
char homedir[64] = "HOME=";
char *envinit[] = {homedir, "TERM=vt100", "PATH=:/bin:/etc:/local/bin:/usr/bin", 0};
extern char **environ;

char	maildir[30] =	"/usr/spool/mail/";
struct	passwd nouser = {"", "nope"};
struct	sgttyb ttyb;
struct	passwd *pwd;
char	*crypt(char *pw, char *salt);
char	*getpass(char *prompt);
void	showmotd(void);
void	catch(i16 signo);
i32	execlp(char *file, char *arg0, ...);

static void
fd_puts(i32 fd, char *s)
{
	register long n;

	n = (long)strlen(s);
	write(fd, s, n);
}

static void
fail1(char *msg)
{
	fd_puts((i32)1, msg);
	fd_puts((i32)1, "\n");
	exit(1);
}

static void
fail2(char *prefix, char *value)
{
	fd_puts((i32)1, prefix);
	fd_puts((i32)1, value);
	fd_puts((i32)1, "\n");
	exit(1);
}

static void
set_home(char *dir)
{
	register char *hp;
	register char *dp;
	register int n;

	hp = homedir + 5;
	dp = dir;
	n = sizeof(homedir) - 6;
	while (n-- > 0 && *dp)
		*hp++ = *dp++;
	*hp = '\0';
}

int
main(int argc, char **argv)
{
	register char *namep;
	char *shell;
	char minusnam[32];
	struct utmp utmp;
	int t, f, c;
	char *ttyn;
	char *linep;

	alarm(60);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	nice(-100);
	nice(20);
	nice(0);
	gtty(0, &ttyb);
	ttyb.sg_erase = '#';
	ttyb.sg_kill = '@';
	stty(0, &ttyb);
	for (t=3; t<20; t++)
		close(t);
	ttyn = ttyname(0);
	if (ttyn==0)
		ttyn = "/dev/tty??";

loop:
	SCPYN(utmp.ut_name, "");
	if (argc>1) {
		SCPYN(utmp.ut_name, argv[1]);
		argc = 0;
	}
	while (utmp.ut_name[0] == '\0') {
		namep = utmp.ut_name;
		printf("login: ");
		while ((c = getchar()) != '\n') {
			if(c == ' ')
				c = '_';
			if (c == EOF)
				exit(0);
			if (namep < utmp.ut_name+8)
				*namep++ = c;
		}
	}
	setpwent();
	if ((pwd = getpwnam(utmp.ut_name)) == 0)
		pwd = &nouser;
	endpwent();
	if (*pwd->pw_passwd != '\0') {
		namep = crypt(getpass("Password:"),pwd->pw_passwd);
		if (strcmp(namep, pwd->pw_passwd)) {
			printf("Login incorrect\n");
			goto loop;
		}
	}

	if(chdir(pwd->pw_dir) < 0) {
		printf("No directory\n");
		goto loop;
	}

	time(&utmp.ut_time);
	t = ttyslot();
	if (t>0 && (f = open("/etc/utmp", 1)) >= 0) {
		lseek(f, (long)(t*sizeof(utmp)), 0);
		linep = rindex(ttyn, '/');
		if (linep)
			linep++;
		else
			linep = ttyn;
		SCPYN(utmp.ut_line, linep);
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	if (t>0 && (f = open("/usr/adm/wtmp", 1)) >= 0) {
		lseek(f, 0L, 2);
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	chown(ttyn, pwd->pw_uid, pwd->pw_gid);
	setgid(pwd->pw_gid);
	setuid(pwd->pw_uid);

	shell = pwd->pw_shell;
	if (shell == 0 || shell[0] == '\0')
		shell = "/bin/sh";

	environ = envinit;
	set_home(pwd->pw_dir);

	if ((namep = rindex(shell, '/')) == 0)
		namep = shell;
	else
		namep++;
	minusnam[0] = '-';
	strcpy(minusnam + 1, namep);
	alarm(0);
	umask(02);
	showmotd();
	strcat(maildir, pwd->pw_name);
	if(access(maildir,4)==0) {
		struct stat statb;
		stat(maildir, &statb);
		if (statb.st_size)
			printf("You have mail.\n");
	}
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	execlp(shell, minusnam, 0);
	printf("No shell\n");
	exit(0);
}

int	stopmotd;
void
catch(i16 signo)
{
	(void)signo;
	signal(SIGINT, SIG_IGN);
	stopmotd++;
}

void
showmotd(void)
{
	FILE *mf;
	register c;

	signal(SIGINT, catch);
	if((mf = fopen("/etc/motd","r")) != NULL) {
		while((c = getc(mf)) != EOF && stopmotd == 0)
			putchar(c);
		fclose(mf);
	}
	signal(SIGINT, SIG_IGN);
}
