/*
 * Run programs submitted by at.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/inttypes.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

# define DIR "/usr/spool/at"
# define PDIR	"past"
# define LASTF "/usr/spool/at/lasttimedone"

int	nowtime;
int	nowdate;
int	nowyear;

struct tm *localtime(time_t *);
static void makenowtime(void);
static void run(char *file);
static void updatetime(i32 t);

int
main(int argc, char **argv)
{
	i16 tt, day, year, uniq;
	struct direct dirent;
	char file[DIRSIZ+1];
	FILE *dirf;

	chdir(DIR);
	makenowtime();
	if ((dirf = fopen(".", "r")) == NULL) {
		fprintf(stderr, "Cannot read at directory\n");
		exit(1);
	}
	while (fread((char *)&dirent, sizeof(dirent), 1, dirf) == 1) {
		if (dirent.d_ino==0)
			continue;
		strncpy(file, dirent.d_name, DIRSIZ);
		file[DIRSIZ] = '\0';
		if (sscanf(file, "%2d.%3d.%4d.%2d", &year, &day, &tt, &uniq) != 4)
			continue;
		if (nowyear < year)
			continue;
		if (nowyear==year && nowdate < day)
			continue;
		if (nowyear==year && nowdate==day && nowtime < tt)
			continue;
		run(file);
	}
	fclose(dirf);
	updatetime(nowtime);
	exit(0);
}

static void
makenowtime(void)
{
	long t;
	register struct tm *tp;

	time(&t);
	tp = localtime(&t);
	nowtime = tp->tm_hour*100 + tp->tm_min;
	nowdate = tp->tm_yday;
	nowyear = tp->tm_year % 100;
}

static void
updatetime(i32 t)
{
	FILE *tfile;

	tfile = fopen(LASTF, "w");
	if (tfile == NULL) {
		fprintf(stderr, "can't write lastfile\n");
		exit(1);
	}
	fprintf(tfile, "%04d\n", t);
}

static void
run(char *file)
{
	struct stat stbuf;
	register pid, i;
	int rc;
	char tbuf[64];

	if (fork()!=0)
		return;
	sprintf(tbuf, "%s/%s", PDIR, file);
	rc = link(file, tbuf);
	if (rc == 0) {
		rc = unlink(file);
	}
	chdir(PDIR);
	rc = stat(file, &stbuf);
	if (rc == -1)
		exit(1);
	for (i=0; i<15; i++)
		close(i);
	dup(dup(open("/dev/null", 0)));
	setgid(stbuf.st_gid);
	setuid(stbuf.st_uid);
	if (pid = fork()) {
		if (pid == -1)
			exit(1);
		wait((i16 *)0);
		unlink(file);
		exit(0);
	}
	nice(3);
	execl("/bin/sh", "sh", file, (char *)0);
	execl("/usr/bin/sh", "sh", file, (char *)0);
	fprintf(stderr, "Can't execl shell\n");
	exit(1);
}
