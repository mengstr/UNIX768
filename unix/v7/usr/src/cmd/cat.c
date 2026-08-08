#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

char	stdbuf[BUFSIZ];
char	catbuf[BUFSIZ];

int
main(int argc, char **argv)
{
	int fflg = 0;
	register FILE *fi;
	register n;
	register fd;
	int dev, ino = -1;
	struct stat statb;

	setbuf(stdout, stdbuf);
	for( ; argc>1 && argv[1][0]=='-'; argc--,argv++) {
		switch(argv[1][1]) {
		case 0:
			break;
		case 'u':
			setbuf(stdout, (char *)NULL);
			continue;
		}
		break;
	}
	fstat(fileno(stdout), &statb);
	statb.st_mode &= S_IFMT;
	if (statb.st_mode!=S_IFCHR && statb.st_mode!=S_IFBLK) {
		dev = statb.st_dev;
		ino = statb.st_ino;
	}
	if (argc < 2) {
		argc = 2;
		fflg++;
	}

#define ERRPFX "cat: "
	while (--argc > 0) {
		if (fflg || (*++argv)[0]=='-' && (*argv)[1]=='\0')
			fi = stdin;
		else {
			if ((fi = fopen(*argv, "r")) == NULL) {
				write(2, ERRPFX, (long)strlen(ERRPFX));
				write(2, "can't open ", 11);
				write(2, *argv, (long)strlen(*argv));
				write(2, "\n", 1);
				continue;
			}
			setbuf(fi, (char *)NULL);
		}
		fstat(fileno(fi), &statb);
		if (statb.st_dev==dev && statb.st_ino==ino) {
			write(2, ERRPFX, (long)strlen(ERRPFX));
			write(2, "input ", 6);
			write(2, fflg ? "-" : *argv, (long)strlen(fflg ? "-" : *argv));
			write(2, " is output\n", 11);
			fclose(fi);
			continue;
		}
		fd = fileno(fi);
		while ((n = read(fd, catbuf, sizeof(catbuf))) > 0)
			write(1, catbuf, n);
		if (fi!=stdin)
			fclose(fi);
	}
	return(0);
}
