/*
 * PWB/UNIX xargs, adapted to ANSI C and the platform/epoch68/emulator V7 ABI.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define MAXSBUF 255
#define MAXIBUF 512
#define MAXINSERTS 5
#define BUFSIZE 570
#define MAXARGS 255

struct inserts {
	char **p_argv;
	char *p_skel;
};

static char *arglist[MAXARGS + 1];
static char argbuf[BUFSIZE + 1];
static char *next = argbuf;
static char *lastarg = "";
static char **argp = arglist;
static char *leof = "_";
static char *inspat = "{}";
static struct inserts saveargv[MAXINSERTS];
static char ins_buf[MAXIBUF];
static char *p_ibuf;
static int prompt = -1;
static int buflim = 470;
static int n_args;
static int n_args_used;
static int more = TRUE;
static int per_line;
static int err;
static int ok = TRUE;
static int legal;
static int trace;
static int insert_mode;
static int linesize;
static int ibufsize;

static void errmsg(char *s1, char *s2, char *s3);
static int substring(char *s1, char *s2);
static char *checklen(char *arg);
static char *addarg(char *arg);
static char *getarg(void);
static int echoargs(void);
static char *insert(char *pattern, char *subst);
static void addibuf(struct inserts *p);
static int getchr(void);
static int runcommand(char *command, char **args);

int
main(int argc, char **argv)
{
	char *cmdname;
	char *initbuf;
	char **initlist;
	int initsize;
	int j;
	int n_inserts;
	struct inserts *psave;

	argc--;
	argv++;
	n_inserts = 0;
	psave = saveargv;

	while (argc > 0 && argv[0][0] == '-') {
		switch (argv[0][1]) {
		case 'x':
			legal = TRUE;
			break;
		case 'l':
			per_line = legal = TRUE;
			n_args = 0;
			insert_mode = FALSE;
			break;
		case 'i':
			insert_mode = per_line = legal = TRUE;
			n_args = 0;
			if (argv[0][2])
				inspat = &argv[0][2];
			break;
		case 't':
			trace = TRUE;
			break;
		case 'e':
			leof = &argv[0][2];
			break;
		case 's':
			buflim = atoi(&argv[0][2]);
			if (buflim > 470 || buflim <= 0) {
				errmsg("0 < max-line-size <= 470: ", argv[0], "\n");
				ok = FALSE;
			}
			break;
		case 'n':
			n_args = atoi(&argv[0][2]);
			if (n_args <= 0) {
				errmsg("#args must be positive int: ", argv[0], "\n");
				ok = FALSE;
			} else {
				if (per_line)
					legal = FALSE;
				insert_mode = per_line = FALSE;
			}
			break;
		case 'p':
			prompt = open("/dev/tty", 0);
			if (prompt == -1) {
				errmsg("can't read from tty for -p\n", 0, 0);
				ok = FALSE;
			} else {
				trace = TRUE;
			}
			break;
		default:
			errmsg("unknown option: ", argv[0], "\n");
			ok = FALSE;
			break;
		}
		argv++;
		argc--;
	}
	if (!ok)
		err = TRUE;

	if (argc == 0) {
		cmdname = "/bin/echo";
		*argp++ = addarg(cmdname);
	} else {
		cmdname = *argv;
	}

	while (ok && argc-- > 0) {
		if (insert_mode && !err && substring(*argv, inspat) != -1) {
			n_inserts++;
			if (n_inserts > MAXINSERTS) {
				errmsg("too many args with ", inspat, "\n");
				err = TRUE;
				ok = FALSE;
			} else {
				psave->p_argv = argp;
				psave->p_skel = *argv;
				psave++;
			}
		}
		if (argp >= &arglist[MAXARGS]) {
			errmsg("too many initial arguments\n", 0, 0);
			err = TRUE;
			ok = FALSE;
			break;
		}
		*argp++ = addarg(*argv++);
	}

	initbuf = next;
	initlist = argp;
	initsize = linesize;

	while (ok && more) {
		next = initbuf;
		argp = initlist;
		linesize = initsize;
		if (*lastarg)
			*argp++ = addarg(lastarg);

		while (argp < &arglist[MAXARGS] && (*argp++ = getarg()) && ok)
			;
		if (argp >= &arglist[MAXARGS] && ok) {
			errmsg("too many arguments\n", 0, 0);
			err = TRUE;
			ok = FALSE;
		}
		if (!more && (per_line || (n_args && n_args_used == 0)))
			return 0;

		if (insert_mode) {
			p_ibuf = ins_buf;
			argp--;
			ibufsize = 0;
			for (j = 0, psave = saveargv; j < n_inserts; j++, psave++) {
				addibuf(psave);
				if (err)
					break;
			}
		}
		*argp = 0;

		if (!err) {
			ok = TRUE;
			j = trace ? echoargs() : TRUE;
			if (j) {
				if (runcommand(cmdname, arglist) != -1)
					continue;
				errmsg(cmdname, " not executed or returned -1\n", 0);
				ok = FALSE;
			}
		}
	}
	return ok ? 0 : 1;
}

static char *
checklen(char *arg)
{
	linesize += strlen(arg) + 1;
	if (linesize > buflim) {
		lastarg = arg;
		if (n_args)
			n_args_used = 1;
		ok = FALSE;
		if (legal) {
			err = TRUE;
			errmsg("arg list too long\n", 0, 0);
		}
		return 0;
	}
	return arg;
}

static char *
addarg(char *arg)
{
	unsigned len;
	char *result;

	len = strlen(arg) + 1;
	if (next + len > &argbuf[BUFSIZE]) {
		errmsg("argument buffer overflow\n", 0, 0);
		err = TRUE;
		ok = FALSE;
		return 0;
	}
	result = next;
	strcpy(next, arg);
	next += len;
	return checklen(result);
}

static char *
getarg(void)
{
	int c;
	int quote;
	char *arg;

	do {
		c = getchr();
	} while (c == ' ' || c == '\n' || c == '\t');
	if (c == '\0') {
		more = FALSE;
		return 0;
	}

	arg = next;
	for (;;) {
		switch (c) {
		case '\t':
		case ' ':
			if (insert_mode) {
				if (next >= &argbuf[BUFSIZE])
					goto overflow;
				*next++ = c;
				break;
			}
			/* FALLTHROUGH */
		case '\n':
		case '\0':
			if (next >= &argbuf[BUFSIZE])
				goto overflow;
			*next++ = '\0';
			if (strcmp(arg, leof) == 0) {
				more = FALSE;
				if (c == ' ' || c == '\t')
					while ((c = getchr()) != '\0' && c != '\n')
						;
				return 0;
			}
			if ((per_line && c == '\n') ||
			    (n_args && ++n_args_used >= n_args)) {
				n_args_used = 0;
				lastarg = "";
				ok = FALSE;
			}
			return checklen(arg);
		case '\\':
			c = getchr();
			if (next >= &argbuf[BUFSIZE])
				goto overflow;
			*next++ = c;
			break;
		case '"':
		case '\'':
			quote = c;
			while ((c = getchr()) != quote) {
				if (c == '\0' || c == '\n') {
					if (next < &argbuf[BUFSIZE])
						*next++ = '\0';
					errmsg("missing quote?: ", arg, "\n");
					ok = FALSE;
					err = TRUE;
					return 0;
				}
				if (next >= &argbuf[BUFSIZE])
					goto overflow;
				*next++ = c;
			}
			break;
		default:
			if (next >= &argbuf[BUFSIZE])
				goto overflow;
			*next++ = c;
			break;
		}
		c = getchr();
	}

overflow:
	errmsg("argument buffer overflow\n", 0, 0);
	err = TRUE;
	ok = FALSE;
	return 0;
}

static void
errmsg(char *s1, char *s2, char *s3)
{
	char *messages[4];
	char **message;

	messages[0] = s1;
	messages[1] = s2;
	messages[2] = s3;
	messages[3] = 0;
	for (message = messages; *message; message++)
		write(2, *message, strlen(*message));
}

static int
echoargs(void)
{
	char **anarg;
	char answer;
	char junk;
	int n;

	for (anarg = arglist; *anarg; anarg++) {
		write(2, *anarg, strlen(*anarg));
		write(2, " ", 1);
	}
	if (prompt == -1) {
		write(2, "\n", 1);
		return TRUE;
	}
	write(2, "?...", 4);
	if (read(prompt, &answer, 1) == 0)
		exit(0);
	if (answer == '\n')
		return FALSE;
	while ((n = read(prompt, &junk, 1)) == 1 && junk != '\n')
		;
	if (n == 0)
		exit(0);
	return answer == 'y';
}

static char *
insert(char *pattern, char *subst)
{
	static char buffer[MAXSBUF + 1];
	int len;
	int patlen;
	char *pat;
	char *pbuf;
	char *bufend;

	len = strlen(subst);
	patlen = strlen(inspat);
	pat = pattern;
	pbuf = buffer;
	bufend = &buffer[MAXSBUF];

	while (*pat) {
		if (substring(pat, inspat) == 0) {
			if (pbuf + len >= bufend)
				break;
			strcpy(pbuf, subst);
			pat += patlen;
			pbuf += len;
		} else {
			if (pbuf >= bufend)
				break;
			*pbuf++ = *pat++;
		}
	}
	if (!*pat) {
		*pbuf = '\0';
		return buffer;
	}
	errmsg("max arg size with insertion via ", inspat, "'s exceeded\n");
	err = TRUE;
	ok = FALSE;
	return 0;
}

static void
addibuf(struct inserts *p)
{
	char *newarg;
	char *skel;
	char *sub;
	int len;

	skel = p->p_skel;
	sub = *argp;
	linesize -= strlen(skel) + 1;
	newarg = insert(skel, sub);
	if (newarg && checklen(newarg)) {
		len = strlen(newarg) + 1;
		ibufsize += len;
		if (ibufsize > MAXIBUF) {
			errmsg("insert-buffer overflow\n", 0, 0);
			err = TRUE;
			ok = FALSE;
			return;
		}
		strcpy(p_ibuf, newarg);
		*p->p_argv = p_ibuf;
		p_ibuf += len;
	}
}

static int
getchr(void)
{
	char c;

	if (read(0, &c, 1) == 1)
		return c & 0xff;
	return '\0';
}

static int
runcommand(char *command, char **args)
{
	i16 status;
	i32 pid;

	pid = fork();
	if (pid == 0) {
		execvp(command, args);
		_exit(255);
	}
	if (pid < 0 || wait(&status) == -1)
		return -1;
	if ((status & 0xff) != 0 || ((status >> 8) & 0xff) == 255)
		return -1;
	return (status >> 8) & 0xff;
}

static int
substring(char *s1, char *s2)
{
	char *start;
	char *p1;
	char *p2;

	if (*s2 == '\0')
		return 0;
	for (start = s1; *start; start++) {
		p1 = start;
		p2 = s2;
		while (*p1 && *p2 && *p1 == *p2) {
			p1++;
			p2++;
		}
		if (*p2 == '\0')
			return start - s1;
	}
	return -1;
}
