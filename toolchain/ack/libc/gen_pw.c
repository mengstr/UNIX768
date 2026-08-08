#include "types.h"

typedef i32 (*match_fn)(char *line, void *arg);

extern i32 read(i32 fd, char *buf, i32 n);
extern i32 open(char *path, i32 mode);
extern i32 close(i32 fd);
extern char *strcpy(char *dst, char *src);
extern i32 atoi(char *s);
extern char *ttyname(i32 fd);

struct passwd {
	char *pw_name;
	char *pw_passwd;
	i32 pw_uid;
	i32 pw_gid;
	i32 pw_quota;
	char *pw_comment;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
};

struct group {
	char *gr_name;
	char *gr_passwd;
	i32 gr_gid;
	char **gr_mem;
};

static char pwbuf[200];
static char grbuf[200];
static char loginbuf[9];
static struct passwd pwent;
static struct group grent;
static char *grmem[32];
static char *empty = "";
static i32 pwfd = -1;

static i32 passwd_uid(char *line, i32 uid);
static i32 line_field_eq(char *line, usize field, char *value);
static i32 line_field_num(char *line, usize field);
static i32 read_matching_line(char *path, char *line, usize linesz, match_fn match,
    void *arg);
static i32 match_pw_name(char *line, void *namep);
static i32 match_pw_uid(char *line, void *uidp);
static i32 match_gr_name(char *line, void *namep);
static char *next_field(char **pp);
static struct passwd *parse_passwd_line(char *line);
static struct group *parse_group_line(char *line);
static i32 read_passwd_line(void);

static i32
passwd_uid(char *line, i32 uid)
{
	register char *p;
	register i32 field;
	register i32 n;

	p = line;
	field = 0;
	while (*p) {
		if (field == 2)
			break;
		if (*p++ == ':')
			field++;
	}
	if (field != 2)
		return 0;

	n = 0;
	if (*p < '0' || *p > '9')
		return 0;
	while (*p >= '0' && *p <= '9')
		n = n * 10 + *p++ - '0';
	return n == uid;
}

i32
getpw(i32 uid, char *buf)
{
	i32 fd;
	i32 n;
	char c;
	char line[200];

	fd = open("/etc/passwd", 0);
	if (fd < 0)
		return 1;

	n = 0;
	while (read(fd, &c, 1) == 1) {
		if (c == '\n') {
			line[n] = '\0';
			if (passwd_uid(line, uid)) {
				strcpy(buf, line);
				close(fd);
				return 0;
			}
			n = 0;
			continue;
		}
		if (n < sizeof(line) - 1)
			line[n++] = c;
	}
	if (n != 0) {
		line[n] = '\0';
		if (passwd_uid(line, uid)) {
			strcpy(buf, line);
			close(fd);
			return 0;
		}
	}
	close(fd);
	return 1;
}

static i32
line_field_eq(char *line, usize field, char *value)
{
	register char *p;
	register char *v;

	p = line;
	while (field > 0) {
		if (*p == '\0')
			return 0;
		if (*p++ == ':')
			field--;
	}
	v = value;
	while (*p != '\0' && *p != ':' && *v != '\0') {
		if (*p++ != *v++)
			return 0;
	}
	return ((*p == '\0' || *p == ':') && *v == '\0');
}

static i32
line_field_num(char *line, usize field)
{
	register char *p;
	register i32 n;

	p = line;
	while (field > 0) {
		if (*p == '\0')
			return -1;
		if (*p++ == ':')
			field--;
	}
	n = 0;
	if (*p < '0' || *p > '9')
		return -1;
	while (*p >= '0' && *p <= '9')
		n = n * 10 + *p++ - '0';
	return n;
}

static i32
read_matching_line(char *path, char *line, usize linesz, match_fn match, void *arg)
{
	i32 fd;
	i32 n;
	char c;

	fd = open(path, 0);
	if (fd < 0)
		return 0;

	n = 0;
	while (read(fd, &c, 1) == 1) {
		if (c == '\n') {
			line[n] = '\0';
			if ((*match)(line, arg)) {
				close(fd);
				return 1;
			}
			n = 0;
			continue;
		}
		if (n < linesz - 1)
			line[n++] = c;
	}
	if (n != 0) {
		line[n] = '\0';
		if ((*match)(line, arg)) {
			close(fd);
			return 1;
		}
	}
	close(fd);
	return 0;
}

static i32
match_pw_name(char *line, void *namep)
{
	return line_field_eq(line, 0, (char *)namep);
}

static i32
match_pw_uid(char *line, void *uidp)
{
	return line_field_num(line, 2) == *(i32 *)uidp;
}

static i32
match_gr_name(char *line, void *namep)
{
	return line_field_eq(line, 0, (char *)namep);
}

static char *
next_field(char **pp)
{
	register char *p;
	char *start;

	p = *pp;
	start = p;
	while (*p != '\0' && *p != ':')
		p++;
	if (*p == ':')
		*p++ = '\0';
	*pp = p;
	return start;
}

static struct passwd *
parse_passwd_line(char *line)
{
	char *p;
	char *uid;
	char *gid;

	p = line;
	pwent.pw_name = next_field(&p);
	pwent.pw_passwd = next_field(&p);
	uid = next_field(&p);
	gid = next_field(&p);
	pwent.pw_uid = atoi(uid);
	pwent.pw_gid = atoi(gid);
	pwent.pw_quota = 0;
	pwent.pw_comment = empty;
	pwent.pw_gecos = next_field(&p);
	pwent.pw_dir = next_field(&p);
	pwent.pw_shell = next_field(&p);
	return &pwent;
}

struct passwd *
getpwnam(char *name)
{
	if (!read_matching_line("/etc/passwd", pwbuf, sizeof(pwbuf), match_pw_name, name))
		return (struct passwd *)0;
	return parse_passwd_line(pwbuf);
}

struct passwd *
getpwuid(i32 uid)
{
	if (!read_matching_line("/etc/passwd", pwbuf, sizeof(pwbuf), match_pw_uid, (char *)&uid))
		return (struct passwd *)0;
	return parse_passwd_line(pwbuf);
}

i32
setpwent(void)
{
	if (pwfd >= 0)
		close(pwfd);
	pwfd = open("/etc/passwd", 0);
	return pwfd < 0;
}

i32
endpwent(void)
{
	if (pwfd >= 0)
		close(pwfd);
	pwfd = -1;
	return 0;
}

static i32
read_passwd_line(void)
{
	i32 n;
	char c;

	n = 0;
	while (read(pwfd, &c, 1) == 1) {
		if (c == '\n') {
			pwbuf[n] = '\0';
			return 1;
		}
		if (n < sizeof(pwbuf) - 1)
			pwbuf[n++] = c;
	}
	if (n != 0) {
		pwbuf[n] = '\0';
		return 1;
	}
	return 0;
}

struct passwd *
getpwent(void)
{
	if (pwfd < 0 && setpwent())
		return (struct passwd *)0;
	if (!read_passwd_line()) {
		endpwent();
		return (struct passwd *)0;
	}
	return parse_passwd_line(pwbuf);
}

char *
getlogin(void)
{
	struct {
		char ut_line[8];
		char ut_name[8];
		i32 ut_time;
	} utmp;
	char *tty;
	char *line;
	i32 fd;
	i32 i;

	tty = ttyname(0);
	if (tty == (char *)0)
		tty = ttyname(1);
	if (tty == (char *)0)
		tty = ttyname(2);
	if (tty == (char *)0)
		return (char *)0;
	line = tty;
	for (i = 0; tty[i] != '\0'; i++)
		if (tty[i] == '/')
			line = &tty[i + 1];
	fd = open("/etc/utmp", 0);
	if (fd < 0)
		return (char *)0;
	while (read(fd, (char *)&utmp, sizeof(utmp)) == sizeof(utmp)) {
		for (i = 0; i < 8; i++) {
			if (utmp.ut_line[i] != line[i])
				break;
			if (utmp.ut_line[i] == '\0')
				break;
		}
		if (i != 8 && (utmp.ut_line[i] != '\0' || line[i] != '\0'))
			continue;
		if (line[i] != '\0' && i == 8)
			continue;
		for (i = 0; i < 8 && utmp.ut_name[i] != '\0'; i++)
			loginbuf[i] = utmp.ut_name[i];
		loginbuf[i] = '\0';
		close(fd);
		return loginbuf;
	}
	close(fd);
	return (char *)0;
}

static struct group *
parse_group_line(char *line)
{
	char *p;
	char *gid;
	char *member;
	i32 n;

	p = line;
	grent.gr_name = next_field(&p);
	grent.gr_passwd = next_field(&p);
	gid = next_field(&p);
	grent.gr_gid = atoi(gid);
	n = 0;
	while (*p != '\0' && n < (sizeof(grmem) / sizeof(grmem[0])) - 1) {
		member = p;
		while (*p != '\0' && *p != ',')
			p++;
		if (*p == ',')
			*p++ = '\0';
		if (*member != '\0')
			grmem[n++] = member;
	}
	grmem[n] = (char *)0;
	grent.gr_mem = grmem;
	return &grent;
}

struct group *
getgrnam(char *name)
{
	if (!read_matching_line("/etc/group", grbuf, sizeof(grbuf), match_gr_name, name))
		return (struct group *)0;
	return parse_group_line(grbuf);
}
