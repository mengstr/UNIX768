#include "types.h"

#define BUFSIZ 512
#define _NFILE 20
#define NULL 0
#define EOF (-1)
#define DUPFLG 0100

typedef struct _iobuf {
	char *_ptr;
	int _cnt;
	char *_base;
	char _flag;
	char _file;
} FILE;

struct passwd {
	char *pw_name;
	char *pw_passwd;
	int pw_uid;
	int pw_gid;
	int pw_quota;
	char *pw_comment;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
};

extern FILE _iob[];

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

extern i32 write(i32 fd, const char *buf, i32 count);
extern i32 read(i32 fd, char *buf, i32 count);
extern i32 open(const char *path, i32 mode);
extern i32 close(i32 fd);
extern i32 creat(const char *path, i32 mode);
extern int unlink(const char *path);
extern int pipe();
extern int dup();
extern int fork();
extern int wait();
extern int exit();
extern int execv();
extern int atoi(const char *s);
extern i32 atol(const char *s);
extern void *calloc(usize count, usize size);
extern void *realloc(void *old, usize n);
extern usize strlen(const char *s);
extern char *strcpy(char *dst, const char *src);
extern char *strcat(char *dst, const char *src);
extern int strcmp(const char *a, const char *b);
extern void *memset(void *dst, int c, usize n);
extern void *memcpy(void *dst, const void *src, usize n);
extern void *memmove(void *dst, const void *src, usize n);
extern int memcmp(const void *a, const void *b, usize n);
extern void *malloc(usize n);
extern void free(void *p);
extern FILE *fopen();
extern FILE *freopen();
extern FILE *fdopen();
extern int fclose();
extern int fflush();
extern int fgetc();
extern int fputc();
extern int fputs();
extern char *fgets();
extern int fread();
extern int fwrite();
extern off32 ftell();
extern int fseek();
extern void rewind();
extern int setbuf();
extern int ungetc();
extern int puts();
extern int putchar();
extern char *sprintf();
extern char *crypt();
extern int getpw();
extern struct passwd *getpwnam();
extern int errno;

static void outstr(const char *s);
static void putnum(u32 n);
static void ok(const char *name, int pass);
static void size_line(const char *name, u32 value);
static int test_strings(void);
static int test_memory(void);
static int test_malloc(void);
static int test_fileio(void);
static int test_pipe(void);
static int test_errno(void);
static int test_getpw(void);
static int test_getpwnam(void);
static int test_crypt(void);
static int test_wait_nochild(void);
static int test_forkpipe(void);
static int test_execpipe(void);
static int test_execpipe_stdio(void);
static int test_execpipe_stdio_dup(void);
static int test_stdio_rw(void);
static int test_stdio_seek(void);
static int test_stdio_reopen(void);
static int test_stdio_console(void);
static int test_printf(void);
static int expect_line(FILE *fp, const char *want);

int
main(argc, argv)
int argc;
char **argv;
{
	if (argc > 1 && strcmp(argv[1], "--readpipe") == 0) {
		char buf[4];
		int n;

		n = read(0, buf, 2);
		return n == 2 && buf[0] == 'e' && buf[1] == 'x' ? 0 : 7;
	}
	if (argc > 1 && strcmp(argv[1], "--fgetcpipe") == 0) {
		int c0;
		int c1;
		int c2;

		c0 = fgetc(stdin);
		c1 = fgetc(stdin);
		c2 = fgetc(stdin);
		return c0 == 's' && c1 == 't' && c2 == EOF ? 0 : 11;
	}

	outstr("ack libc test\n");
	size_line("char", sizeof(char));
	size_line("short", sizeof(short));
	size_line("int", sizeof(int));
	size_line("long", sizeof(long));
	ok("strings", test_strings());
	ok("memory", test_memory());
	ok("malloc", test_malloc());
	ok("fileio", test_fileio());
	ok("pipe", test_pipe());
	ok("errno", test_errno());
	ok("getpw", test_getpw());
	ok("getpwnam", test_getpwnam());
	ok("crypt", test_crypt());
	outstr("wait-nochild: ");
	outstr(test_wait_nochild() ? "ok\n" : "FAIL\n");
	ok("forkpipe", test_forkpipe());
	ok("execpipe", test_execpipe());
	ok("execpipe-stdio", test_execpipe_stdio());
	ok("execpipe-stdio-dup", test_execpipe_stdio_dup());
	ok("stdio-rw", test_stdio_rw());
	ok("stdio-seek", test_stdio_seek());
	ok("stdio-reopen", test_stdio_reopen());
	ok("printf", test_printf());
	ok("stdio-console", test_stdio_console());
	return 0;
}

static void
outstr(const char *s)
{
	write(1, s, strlen(s));
}

static void
putnum(u32 n)
{
	char c;

	if (n > 9) {
		write(1, "?", 1);
		return;
	}
	c = (char)('0' + n);
	write(1, &c, 1);
}

static void
ok(const char *name, int pass)
{
	outstr(name);
	outstr(pass ? ": ok\n" : ": FAIL\n");
}

static void
size_line(const char *name, u32 value)
{
	outstr(name);
	outstr("=");
	putnum(value);
	outstr("\n");
}

static int
test_strings(void)
{
	char buf[32];

	strcpy(buf, "ack");
	strcat(buf, " libc");
	if (strlen(buf) != 8)
		return 0;
	if (strcmp(buf, "ack libc") != 0)
		return 0;
	if (atoi("123") != 123)
		return 0;
	if (atol(" -45") != -45)
		return 0;
	return 1;
}

static int
test_memory(void)
{
	char a[8];
	char b[8];
	char c[16];

	memset(a, 'x', sizeof(a));
	memcpy(b, a, sizeof(a));
	if (memcmp(a, b, sizeof(a)) != 0)
		return 0;

	strcpy(c, "abcde");
	memmove(c + 1, c, 6);
	if (strcmp(c, "aabcde") != 0)
		return 0;

	return 1;
}

static int
test_malloc(void)
{
	char *p;
	char *q;
	usize i;

	p = (char *)malloc(16);
	if (p == (char *)0)
		return 0;
	strcpy(p, "heap");
	if (strcmp(p, "heap") != 0)
		return 0;

	q = (char *)calloc(4, 4);
	if (q == (char *)0)
		return 0;
	for (i = 0; i < 16; i++) {
		if (q[i] != '\0')
			return 0;
	}
	strcpy(q, "zero");
	q = (char *)realloc(q, 32);
	if (q == (char *)0)
		return 0;
	if (strcmp(q, "zero") != 0)
		return 0;

	free(p);
	free(q);
	return 1;
}

static int
test_fileio(void)
{
	int fd;
	char buf[8];
	int n;

	unlink("acktmp");
	fd = creat("acktmp", 0644);
	if (fd < 0)
		return 0;
	if (write(fd, "file\n", 5) != 5) {
		close(fd);
		return 0;
	}
	close(fd);
	fd = open("acktmp", 0);
	if (fd < 0)
		return 0;
	n = read(fd, buf, 5);
	close(fd);
	unlink("acktmp");
	if (n != 5)
		return 0;
	buf[5] = '\0';
	return strcmp(buf, "file\n") == 0;
}

static int
test_pipe(void)
{
	int pv[2];
	char buf[8];
	int n;

	if (pipe(pv) < 0)
		return 0;
	if (write(pv[1], "xy", 2) != 2) {
		close(pv[0]);
		close(pv[1]);
		return 0;
	}
	close(pv[1]);
	n = read(pv[0], buf, 2);
	close(pv[0]);
	if (n != 2 || buf[0] != 'x' || buf[1] != 'y')
		return 0;

	if (pipe(pv) < 0)
		return 0;
	if (dup(pv[1] | DUPFLG, 8) != 8) {
		close(pv[0]);
		close(pv[1]);
		return 0;
	}
	close(pv[1]);
	if (write(8, "z", 1) != 1) {
		close(pv[0]);
		close(8);
		return 0;
	}
	close(8);
	n = read(pv[0], buf, 1);
	close(pv[0]);
	return n == 1 && buf[0] == 'z';
}

static int
test_errno(void)
{
	int fd;
	int status;

	errno = 0;
	fd = open("ack_missing_errno", 0);
	if (fd != -1 || errno != 2)
		return 0;

	errno = 0;
	if (wait(&status) != -1 || errno != 10)
		return 0;

	return 1;
}

static int
test_getpw(void)
{
	char buf[200];
	char *p;

	if (getpw(0, buf) != 0)
		return 0;
	for (p = buf; *p; p++)
		if (*p == ':')
			return 1;
	return 0;
}

static int
test_getpwnam(void)
{
	struct passwd *pwd;

	pwd = getpwnam("dmr");
	if (pwd == (struct passwd *)0)
		return 0;
	if (strcmp(pwd->pw_name, "dmr") != 0)
		return 0;
	if (strcmp(pwd->pw_passwd, "dmx2xIJKTmA/A") != 0)
		return 0;
	if (pwd->pw_uid != 7 || pwd->pw_gid != 3)
		return 0;
	if (strcmp(pwd->pw_dir, "/usr/dmr") != 0)
		return 0;
	if (strcmp(pwd->pw_shell, "/bin/sh") != 0)
		return 0;
	return getpwnam("apa") == (struct passwd *)0;
}

static int
test_crypt(void)
{
	char *p;

	p = crypt("dmr", "dm");
	return p != (char *)0 && strcmp(p, "dmx2xIJKTmA/A") == 0;
}

static int
test_wait_nochild(void)
{
	int pid;
	int status;

	pid = fork();
	if (pid < 0)
		return 0;
	if (pid == 0)
		exit(0);
	if (wait(&status) != pid)
		return 0;
	return wait(&status) == -1;
}

static int
test_forkpipe(void)
{
	int pv[2];
	char buf[4];
	int pid;
	int status;
	int n;

	if (pipe(pv) < 0)
		return 0;
	pid = fork();
	if (pid < 0) {
		close(pv[0]);
		close(pv[1]);
		return 0;
	}
	if (pid == 0) {
		close(pv[1]);
		n = read(pv[0], buf, 2);
		close(pv[0]);
		if (n == 2 && buf[0] == 'p' && buf[1] == 'q')
			exit(0);
		exit(3);
	}

	close(pv[0]);
	if (write(pv[1], "pq", 2) != 2) {
		close(pv[1]);
		wait(&status);
		return 0;
	}
	close(pv[1]);
	if (wait(&status) != pid)
		return 0;
	return status == 0;
}

static int
test_execpipe(void)
{
	int pv[2];
	int pid;
	int status;
	char *args[3];

	if (pipe(pv) < 0)
		return 0;
	pid = fork();
	if (pid < 0) {
		close(pv[0]);
		close(pv[1]);
		return 0;
	}
	if (pid == 0) {
		close(pv[1]);
		if (dup(pv[0] | DUPFLG, 0) != 0)
			exit(8);
		close(pv[0]);
		args[0] = "libctest";
		args[1] = "--readpipe";
		args[2] = (char *)0;
		execv("libctest", args);
		exit(9);
	}

	close(pv[0]);
	if (write(pv[1], "ex", 2) != 2) {
		close(pv[1]);
		wait(&status);
		return 0;
	}
	close(pv[1]);
	if (wait(&status) != pid)
		return 0;
	return status == 0;
}

static int
test_execpipe_stdio(void)
{
	int pv[2];
	int pid;
	int status;
	char *args[3];

	if (pipe(pv) < 0)
		return 0;
	pid = fork();
	if (pid < 0) {
		close(pv[0]);
		close(pv[1]);
		return 0;
	}
	if (pid == 0) {
		close(pv[1]);
		if (dup(pv[0] | DUPFLG, 0) != 0)
			exit(8);
		close(pv[0]);
		args[0] = "libctest";
		args[1] = "--fgetcpipe";
		args[2] = (char *)0;
		execv("libctest", args);
		exit(9);
	}

	close(pv[0]);
	if (write(pv[1], "st", 2) != 2) {
		close(pv[1]);
		wait(&status);
		return 0;
	}
	close(pv[1]);
	if (wait(&status) != pid)
		return 0;
	return status == 0;
}

static int
test_execpipe_stdio_dup(void)
{
	int pv[2];
	int pid;
	int status;
	char *args[3];

	if (pipe(pv) < 0)
		return 0;
	pid = fork();
	if (pid < 0) {
		close(pv[0]);
		close(pv[1]);
		return 0;
	}
	if (pid == 0) {
		close(pv[1]);
		close(0);
		if (dup(pv[0]) != 0)
			exit(8);
		close(pv[0]);
		args[0] = "libctest";
		args[1] = "--fgetcpipe";
		args[2] = (char *)0;
		execv("libctest", args);
		exit(9);
	}

	close(pv[0]);
	if (write(pv[1], "st", 2) != 2) {
		close(pv[1]);
		wait(&status);
		return 0;
	}
	close(pv[1]);
	if (wait(&status) != pid)
		return 0;
	return status == 0;
}

static int
expect_line(FILE *fp, const char *want)
{
	char buf[128];

	if (fgets(buf, sizeof(buf), fp) == NULL)
		return 0;
	return strcmp(buf, want) == 0;
}

static int
test_stdio_rw(void)
{
	FILE *fp;
	char buf[128];

	unlink("ackstdio");

	fp = fopen("ackstdio", "w");
	if (fp == NULL)
		return 0;
	setbuf(fp, NULL);
	if (fputs("alpha\n", fp) < 0)
		return 0;
	if (fwrite("beta\n", 1, 5, fp) != 5)
		return 0;
	if (fputs("gamma 7 a\n", fp) < 0)
		return 0;
	if (fflush(fp) == EOF)
		return 0;
	if (ftell(fp) <= 0)
		return 0;
	if (fclose(fp) == EOF)
		return 0;

	fp = fopen("ackstdio", "r");
	if (fp == NULL)
		return 0;
	if (!expect_line(fp, "alpha\n"))
		return 0;
	if (!expect_line(fp, "beta\n"))
		return 0;
	if (!expect_line(fp, "gamma 7 a\n"))
		return 0;
	if (fclose(fp) == EOF)
		return 0;
	unlink("ackstdio");
	return 1;
}

static int
test_stdio_seek(void)
{
	FILE *fp;
	char buf[128];

	unlink("ackstdio");
	fp = fopen("ackstdio", "w");
	if (fp == NULL)
		return 0;
	if (fputs("alpha\nbeta\ngamma\n", fp) < 0)
		return 0;
	if (fclose(fp) == EOF)
		return 0;

	fp = fopen("ackstdio", "r");
	if (fp == NULL)
		return 0;
	rewind(fp);
	if (fgetc(fp) != 'a') {
		fclose(fp);
		return 0;
	}
	if (ungetc('a', fp) != 'a') {
		fclose(fp);
		return 0;
	}
	if (fgetc(fp) != 'a') {
		fclose(fp);
		return 0;
	}
	if (fseek(fp, 6L, 0) != 0) {
		fclose(fp);
		return 0;
	}
	if (ftell(fp) != 6) {
		fclose(fp);
		return 0;
	}
	if (fread(buf, 1, 5, fp) != 5) {
		fclose(fp);
		return 0;
	}
	buf[5] = '\0';
	if (strcmp(buf, "beta\n") != 0) {
		fclose(fp);
		return 0;
	}
	if (!expect_line(fp, "gamma\n")) {
		fclose(fp);
		return 0;
	}
	if (fclose(fp) == EOF)
		return 0;
	unlink("ackstdio");
	return 1;
}

static int
test_stdio_reopen(void)
{
	FILE *fp;
	int fd;

	unlink("ackstdio");
	fp = fopen("ackstdio", "w");
	if (fp == NULL)
		return 0;
	if (fputs("alpha\n", fp) < 0)
		return 0;
	if (freopen("ackstdio", "a", fp) == NULL)
		return 0;
	if (fputs("beta\n", fp) < 0)
		return 0;
	if (fclose(fp) == EOF)
		return 0;

	fd = open("ackstdio", 1);
	if (fd < 0)
		return 0;
	fp = fdopen(fd, "a");
	if (fp == NULL) {
		close(fd);
		return 0;
	}
	if (fputs("gamma\n", fp) < 0)
		return 0;
	if (fclose(fp) == EOF)
		return 0;

	fp = fopen("ackstdio", "r");
	if (fp == NULL)
		return 0;
	if (!expect_line(fp, "alpha\n"))
		return 0;
	if (!expect_line(fp, "beta\n"))
		return 0;
	if (!expect_line(fp, "gamma\n"))
		return 0;
	if (fclose(fp) == EOF)
		return 0;
	unlink("ackstdio");
	return 1;
}

static int
test_stdio_console(void)
{
	if (puts("puts: ok") < 0)
		return 0;
	if (putchar('!') == EOF)
		return 0;
	if (putchar('\n') == EOF)
		return 0;
	if (fflush(stdout) == EOF)
		return 0;

	return 1;
}

static int
test_printf(void)
{
	char buf[64];

	sprintf(buf, "%.3s:%s:%d:%ld", "abcdef", "ok", 7, 1234L);
	return strcmp(buf, "abc:ok:7:1234") == 0;
}
