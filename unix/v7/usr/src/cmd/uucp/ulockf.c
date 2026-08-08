#include "uucp.h"
#include <sys/types.h>
#include <sys/stat.h>


/*******
 *	ulockf(file, atime)
 *	char *file;
 *	time_t atime;
 *
 *	ulockf  -  this routine will create a lock file (file).
 *	If one already exists, the create time is checked for
 *	older than the age time (atime).
 *	If it is older, an attempt will be made to unlink it
 *	and create a new one.
 *
 *	return codes:  0  |  FAIL
 */

int
ulockf (char *file, time_t atime)
{
	struct stat stbuf;
	time_t ptime;
	int ret;
	static int pid = -1;
	static char tempfile[NAMESIZE];

	if (pid < 0) {
		pid = getpid();
		sprintf(tempfile, "LTMP.%d", pid);
	}
	if (onelock(pid, tempfile, file) == -1) {
		/* lock file exists */
		/* get status to check age of the lock file */
		ret = stat(file, &stbuf);
		ASSERT(ret != -1, "LOCK PROBLEM - %s", file);

		time(&ptime);
		if ((ptime - stbuf.st_ctime) < atime) {
			/* file not old enough to delete */
			return(FAIL);
		}
	
		ret = unlink(file);
		ASSERT(ret != -1, "LOCK PROBLEM - %s", file);
	
		ret = onelock(pid, tempfile, file);
		ASSERT(ret == 0, "LOCK PROBLEM - %s", file);
	}
	stlock(file);
	return(0);
}


#define MAXLOCKS 10	/* maximum number of lock files */
char *Lockfile[MAXLOCKS];
int Nlocks = 0;

/***
 *	stlock(name)	put name in list of lock files
 *	char *name;
 *
 *	return codes:  none
 */

int
stlock (char *name)
{
	char *p;
	int i;

	for (i = 0; i < Nlocks; i++) {
		if (Lockfile[i] == NULL)
			break;
	}
	ASSERT(i < MAXLOCKS, "TOO MANY LOCKS %d", i);
	if (i >= Nlocks)
		i = Nlocks++;
	p = calloc(strlen(name) + 1, sizeof (char));
	ASSERT(p != NULL, "CAN NOT ALLOCATE FOR %s", name);
	strcpy(p, name);
	Lockfile[i] = p;
	return;
}


/***
 *	rmlock(name)	remove all lock files in list
 *	char *name;	or name
 *
 *	return codes: none
 */

int
rmlock (char *name)
{
	int i;

	for (i = 0; i < Nlocks; i++) {
		if (Lockfile[i] == NULL)
			continue;
		if (name == NULL
		|| strcmp(name, Lockfile[i]) == SAME) {
			unlink(Lockfile[i]);
			free(Lockfile[i]);
			Lockfile[i] = NULL;
		}
	}
	return;
}


/*  this stuff from pjw  */
/*  /usr/pjw/bin/recover - check pids to remove unnecessary locks */
/*	isalock(name) returns 0 if the name is a lock */
/*	unlock(name)  unlocks name if it is a lock*/
/*	onelock(pid,tempfile,name) makes lock a name
	on behalf of pid.  Tempfile must be in the same
	file system as name. */
/*	lock(pid,tempfile,names) either locks all the
	names or none of them */
int
isalock (char *name)
{
	struct stat xstat;
	if(stat(name,&xstat)<0) return(0);
	if(xstat.st_size!=sizeof(int)) return(0);
	return(1);
}
int
unlock (char *name)
{
	if(isalock(name)) return(unlink(name));
	else return(-1);
}
int
onelock (int pid, char *tempfile, char *name)
{	int fd;
	fd=creat(tempfile,0444);
	if(fd<0) return(-1);
	write(fd,&pid,sizeof(int));
	close(fd);
	if(link(tempfile,name)<0)
	{	unlink(tempfile);
		return(-1);
	}
	unlink(tempfile);
	return(0);
}
int
lock (int pid, char *tempfile, char **names)
{	int i,j;
	for(i=0;names[i]!=0;i++)
	{	if(onelock(pid,tempfile,names[i])==0) continue;
		for(j=0;j<i;j++) unlink(names[j]);
		return(-1);
	}
	return(0);
}

#define LOCKPRE "LCK."

/***
 *	delock(s)	remove a lock file
 *	char *s;
 *
 *	return codes:  0  |  FAIL
 */

int
delock (char *s)
{
	char ln[30];

	sprintf(ln, "%s.%s", LOCKPRE, s);
	rmlock(ln);
}


/***
 *	mlock(sys)	create system lock
 *	char *sys;
 *
 *	return codes:  0  |  FAIL
 */

int
mlock (char *sys)
{
	char lname[30];
	sprintf(lname, "%s.%s", LOCKPRE, sys);
	return(ulockf(lname, (time_t) 24*3600 ) < 0 ? FAIL : 0);
}

