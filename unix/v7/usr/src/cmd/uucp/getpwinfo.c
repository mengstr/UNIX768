#include "uucp.h"
#include <pwd.h>


/*******
 *	guinfo(uid, name, path)	get passwd file info for uid
 *	int uid;
 *	char *path, *name;
 *
 *	return codes:  0  |  FAIL
 */

int
guinfo (int uid, char *name, char *path)
{
	struct passwd *pwd;

	if ((pwd = getpwuid(uid)) == NULL) {
		/* can not find uid in passwd file */
		*path = '\0';
		return(FAIL);
	}
	strcpy(path, pwd->pw_dir);
	strcpy(name, pwd->pw_name);
	return(0);
}


/***
 *	gninfo(name, uid, path)	get passwd file info for name
 *	char *path, *name;
 *	int *uid;
 *
 *	return codes:  0  |  FAIL
 */

int
gninfo (char *name, int *uid, char *path)
{
	struct passwd *pwd;

	if ((pwd = getpwnam(name)) == NULL) {
		/* can not find name in passwd file */
		*path = '\0';
		return(FAIL);
	}

	strcpy(path, pwd->pw_dir);
	*uid = pwd->pw_uid;
	return(0);
}
