/*
 * mesg -- set current tty to accept or
 *	forbid write permission.
 *
 *	mesg [y] [n]
 *		y allow messages
 *		n forbid messages
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct stat sbuf;

char *tty;
char *ttyname(i32 fd);

static void error(char *s);
static void newmode(i32 m);

int
main(int argc, char *argv[])
{
	int r=0;
	tty = ttyname(2);
	if(stat(tty, &sbuf) < 0) error("cannot stat");
	if(argc < 2) {
		if(sbuf.st_mode & 02)
			fprintf(stderr,"is y\n");
		else {	r=1;
			fprintf(stderr,"is n\n");
		}
	} else	switch(*argv[1]) {
		case 'y':
			newmode(0622); break;

		case 'n':
			newmode(0600); r=1; break;

		default:
			error("usage: mesg [y] [n]");
		}
	exit(r);
}

static void
error(char *s)
{
	fprintf(stderr,"mesg: %s\n",s);
	exit(-1);
}

static void
newmode(m)
i32 m;
{
	if(chmod(tty,m)<0)
		error("cannot change mode");
}
