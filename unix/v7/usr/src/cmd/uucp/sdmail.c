#include "uucp.h"
#include <pwd.h>


/*******
 *	sdmail(file, uid)
 *	char *file, *uid;
 *
 *	sdmail  -  this routine will determine the owner
 *	of the file (file), create a message string and
 *	call "mailst" to send the cleanup message.
 *	This is only implemented for local system
 *	mail at this time.
 */

int
sdmail(char *file, i32 uid)
{
	static struct passwd *pwd;
	char mstr[40];

	sprintf(mstr, "uuclean deleted file %s\n", file);
	if (pwd == NULL || pwd->pw_uid != uid) {
		setpwent();
		pwd = getpwuid(uid);
	}
	if (pwd != NULL)
		mailst(pwd->pw_name, mstr);
	return(0);
}


/***
 *	mailst(user, str)
 *	char *user, *str;
 *
 *	mailst  -  this routine will fork and execute
 *	a mail command sending string (str) to user (user).
 */

int
mailst (char *user, char *str)
{
	FILE *fp;
	char cmd[100];

	sprintf(cmd, "mail %s", user);
	if ((fp = popen(cmd, "w")) == NULL)
		return;
	fprintf(fp, "%s", str);
	pclose(fp);
	return;
}
