#include "sys/inttypes.h"

struct	passwd { /* see getpwent(3) */
	char	*pw_name;
	char	*pw_passwd;
	i32	pw_uid;
	i32	pw_gid;
	i32	pw_quota;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

struct passwd	*getpwnam(char *name);
struct passwd	*getpwuid(i32 uid);
struct passwd	*getpwent(void);
int		getpw(i32 uid, char *buf);
int		setpwent(void);
int		endpwent(void);
