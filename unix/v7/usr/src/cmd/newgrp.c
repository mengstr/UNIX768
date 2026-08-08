#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct group *getgrnam(char *name), *grp;
struct passwd *getpwuid(i32 uid), *pwd;
char *getpass(char *prompt), *crypt(char *key, char *salt);
int done(void);

int
main (int argc, char **argv)
{
	register i;
	if(argc != 2) {
		printf("usage: newgrp groupname\n");
		done();
	}
	if((grp=getgrnam(argv[1])) == NULL) {
		printf("%s: no such group\n", argv[1]);
		done();
	}
	if((pwd=getpwuid(getuid())) == NULL) {
		printf("You do not exist!\n");
		done();
	}
	for(i=0;grp->gr_mem[i];i++)
		if(strcmp(grp->gr_mem[i], pwd->pw_name) == 0)
			break;
	if(grp->gr_mem[i] == 0 && strcmp(grp->gr_name,"other")) {
		printf("Sorry\n");
		done();
	}

	if(grp->gr_passwd[0] != '\0' && pwd->pw_passwd[0] == '\0') {
		if(strcmp(grp->gr_passwd, crypt(getpass("Password:"),grp->gr_passwd)) != 0) {
			printf("Sorry\n");
			done();
		}
	}
	if(setgid(grp->gr_gid) < 0)
		perror("setgid");
	done();
}

int
done (void)
{
	register i;

	setuid(getuid());
	for (i=3; i<15; i++)
		close(i);
	execl("/bin/sh", "sh", (char *)0);
	printf("No shell!\n");
	exit(0);
}
