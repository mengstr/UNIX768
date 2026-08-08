#include "monitor_cmd.h"

#include "path.h"
#include "uart.h"
#include "string.h"
#include "v7io.h"
#include "v7fs.h"
#include "ymodem.h"

i32 cat(char *path);
i32 boot(char *path, i32 flags);
i32 chgrp(char *path, i32 gid);
i32 chown(char *path, i32 uid);
i32 userboot(char *path);
i32 chmod(char *path, i32 mode);
i32 cp(char *src, char *dst);
i32 hd(char *path);
i32 ls(char *path);
i32 link(char *src, char *dst);
i32 mkdir(char *path);
i32 mknod(char *path, i32 mode, dev_t dev);
i32 mv(char *src, char *dst);
i32 rm(char *path);
i32 rmdir(char *path);
i32 makefsv7(char *devspec, i32 nblocks);
i32 atol(char *s);

enum argtype {
	ARG_NONE,
	ARG_STRING,
	ARG_OPTSTRING,
	ARG_PATH,
	ARG_OPTPATH,
	ARG_DEVSPEC,
	ARG_MODE,
	ARG_OPTMODE,
	ARG_RXMODE,
	ARG_NUMBER,
	ARG_REST
};

enum cmd_id {
	CMD_HELP,
	CMD_ATTACH,
	CMD_MKFS,
	CMD_MKNOD,
	CMD_MKDIR,
	CMD_RMDIR,
	CMD_PWD,
	CMD_RM,
	CMD_CHGRP,
	CMD_CHOWN,
	CMD_CHMOD,
	CMD_LINK,
	CMD_CD,
	CMD_LS,
	CMD_HD,
	CMD_CP,
	CMD_MV,
	CMD_CAT,
	CMD_RX,
	CMD_RXSCRIPT,
	CMD_RXSCRIPTS,
	CMD_BOOT,
	CMD_USER
};

struct command {
	char *name;
	enum cmd_id id;
	enum argtype arg1;
	enum argtype arg2;
	char *usage;
};

static struct command commands[] = {
	{"help", CMD_HELP, ARG_NONE, ARG_NONE, ""},
	{"?", CMD_HELP, ARG_NONE, ARG_NONE, ""},
	{"attach", CMD_ATTACH, ARG_DEVSPEC, ARG_NONE, " <devspec>"},
	{"mkfs", CMD_MKFS, ARG_DEVSPEC, ARG_NUMBER, " <devspec> <blocks>"},
	{"mknod", CMD_MKNOD, ARG_PATH, ARG_STRING, " <path> <c|b major minor>"},
	{"mkdir", CMD_MKDIR, ARG_PATH, ARG_OPTMODE, " <path> [mode]"},
	{"rmdir", CMD_RMDIR, ARG_PATH, ARG_NONE, " <path>"},
	{"pwd", CMD_PWD, ARG_NONE, ARG_NONE, ""},
	{"rm", CMD_RM, ARG_PATH, ARG_NONE, " <path>"},
	{"chgrp", CMD_CHGRP, ARG_NUMBER, ARG_PATH, " <gid> <path>"},
	{"chown", CMD_CHOWN, ARG_NUMBER, ARG_PATH, " <uid> <path>"},
	{"chmod", CMD_CHMOD, ARG_MODE, ARG_PATH, " <mode> <path>"},
	{"ln", CMD_LINK, ARG_PATH, ARG_PATH, " <existing-path> <new-path>"},
	{"cd", CMD_CD, ARG_PATH, ARG_NONE, " <path>"},
	{"ls", CMD_LS, ARG_OPTPATH, ARG_NONE, " [path]"},
	{"hd", CMD_HD, ARG_PATH, ARG_NONE, " <path>"},
	{"cp", CMD_CP, ARG_PATH, ARG_PATH, " <src> <dst>"},
	{"mv", CMD_MV, ARG_PATH, ARG_PATH, " <src> <dst>"},
	{"cat", CMD_CAT, ARG_PATH, ARG_NONE, " <path>"},
	{"rx", CMD_RX, ARG_OPTSTRING, ARG_RXMODE, " [path] [mode]"},
	{"rxscript", CMD_RXSCRIPT, ARG_OPTSTRING, ARG_NONE, " [file]"},
	{".", CMD_RXSCRIPTS, ARG_OPTSTRING, ARG_REST, " [file ...]"},
	{"boot", CMD_BOOT, ARG_OPTSTRING, ARG_OPTSTRING, " [path] [-m]"},
	{"user", CMD_USER, ARG_PATH, ARG_NONE, " <path>"},
	{0, 0, ARG_NONE, ARG_NONE, 0}
};

static void
print_help(void)
{
	register struct command *cmd;

	for (cmd = commands; cmd->name != 0; cmd++) {
		if (strcmp(cmd->name, "?") == 0 || strcmp(cmd->name, ".") == 0)
			continue;
		uart_puts(cmd->name);
		uart_puts(cmd->usage);
		uart_putc('\n');
	}
}

static void
print_pwd(char *cwd)
{
	if (cwd == 0 || *cwd == '\0' || (cwd[0] == '.' && cwd[1] == '\0')) {
		uart_puts("/\n");
		return;
	}
	uart_putc('/');
	uart_puts(cwd);
	uart_putc('\n');
}

/*
 * Give the host-side tooling a small breather between scripted commands.
 * This helps the YMODEM helper and monitor-side consumers avoid tripping
 * over back-to-back script lines during bootstrap.
 */
static void
monitor_script_pause(void)
{
	volatile u32 i;

	for (i = 0; i < 20000UL; i++)
		;
}

static i32 monitor_run_command_mode(char *cmdbuf, char *attached,
	i32 attachedsz, char *cwd, i32 cwdsz, i32 scripted);

static struct command *
find_command(char *name)
{
	register struct command *cmd;
	i32 n;

	if (name == 0)
		return(0);

	for (cmd = commands; cmd->name != 0; cmd++) {
		n = strlen(cmd->name);
		if (strncasecmp(name, cmd->name, n) == 0 && name[n] == '\0')
			return(cmd);
	}

	return(0);
}

static i32
is_octal_mode(char *s)
{
	if (s == 0 || *s == '\0')
		return(0);
	while (*s != '\0') {
		if (*s < '0' || *s > '7')
			return(0);
		s++;
	}
	return(1);
}

static i32
is_decimal_number(char *s)
{
	if (s == 0 || *s == '\0')
		return(0);
	while (*s != '\0') {
		if (*s < '0' || *s > '9')
			return(0);
		s++;
	}
	return(1);
}

static i32
is_rx_mode(char *s)
{
	i32 n;

	if (!is_octal_mode(s))
		return(0);
	n = strlen(s);
	return(n == 4 || n == 5);
}

/*
 * Parse the monitor form:
 *
 *     mknod <path> c <major> <minor>
 *     mknod <path> b <major> <minor>
 *
 * arg2 is the remainder of the command line after the path, so we split it
 * locally into exactly three words here.
 */
static i32
parse_mknod_spec(char *spec, i32 *modep, dev_t *devp)
{
	char *p;
	char *type;
	char *major_s;
	char *minor_s;
	i32 major_n;
	i32 minor_n;

	if (spec == 0 || *spec == '\0') {
		uart_puts("mknod: missing type/major/minor\n");
		return(-1);
	}

	p = spec;
	type = next_word(&p);
	major_s = next_word(&p);
	minor_s = next_word(&p);
	while (*p == ' ')
		p++;

	if (type == 0 || major_s == 0 || minor_s == 0 || *p != '\0') {
		uart_puts("mknod: usage mknod <name> <c|b major minor>\n");
		return(-1);
	}
	if (!is_decimal_number(major_s) || !is_decimal_number(minor_s)) {
		uart_puts("mknod: bad major/minor\n");
		return(-1);
	}

	if (type[0] == 'c' && type[1] == '\0')
		*modep = IFCHR;
	else if (type[0] == 'b' && type[1] == '\0')
		*modep = IFBLK;
	else {
		uart_puts("mknod: type must be c or b\n");
		return(-1);
	}

	major_n = atoi(major_s);
	minor_n = atoi(minor_s);
	if (major_n < 0 || major_n > 255 || minor_n < 0 || minor_n > 255) {
		uart_puts("mknod: major/minor out of range\n");
		return(-1);
	}

	*devp = makedev(major_n, minor_n);
	return(0);
}

static i32
validate_arg(char *cmd, enum argtype type, char *arg, char *dst, i32 dstsz,
	char *attached, char *cwd)
{
	switch (type) {
	case ARG_NONE:
	case ARG_REST:
		return(0);

	case ARG_STRING:
	case ARG_DEVSPEC:
		if (arg == 0 || *arg == '\0') {
			uart_puts(cmd);
			uart_puts(": missing argument\n");
			return(-1);
		}
		return(0);

	case ARG_OPTSTRING:
		return(0);

	case ARG_PATH:
		if (arg == 0 || *arg == '\0') {
			uart_puts(cmd);
			uart_puts(": missing path\n");
			return(-1);
		}
		if (v7fs_resolve_path(dst, dstsz, attached, cwd, arg) < 0) {
			uart_puts(cmd);
			uart_puts(": bad path\n");
			return(-1);
		}
		return(0);

	case ARG_OPTPATH:
		if (v7fs_resolve_path(dst, dstsz, attached, cwd, arg) < 0) {
			uart_puts(cmd);
			uart_puts(": bad path\n");
			return(-1);
		}
		return(0);

	case ARG_MODE:
		if (arg == 0 || *arg == '\0') {
			uart_puts(cmd);
			uart_puts(": missing mode\n");
			return(-1);
		}
		if (!is_octal_mode(arg)) {
			uart_puts(cmd);
			uart_puts(": bad mode\n");
			return(-1);
		}
		return(0);

	case ARG_OPTMODE:
		if (arg == 0 || *arg == '\0')
			return(0);
		if (!is_octal_mode(arg)) {
			uart_puts(cmd);
			uart_puts(": bad mode\n");
			return(-1);
		}
		return(0);

	case ARG_RXMODE:
		if (arg == 0 || *arg == '\0')
			return(0);
		if (!is_rx_mode(arg)) {
			uart_puts(cmd);
			uart_puts(": mode must be 4 or 5 octal digits\n");
			return(-1);
		}
		return(0);

	case ARG_NUMBER:
		if (arg == 0 || *arg == '\0') {
			uart_puts(cmd);
			uart_puts(": missing number\n");
			return(-1);
		}
		if (!is_decimal_number(arg)) {
			uart_puts(cmd);
			uart_puts(": bad number\n");
			return(-1);
		}
		return(0);
	}

	return(-1);
}

i32
monitor_run_script(char *script, char *attached, i32 attachedsz, char *cwd,
	i32 cwdsz)
{
	/*
	 * Receive commands can use a deep stack (notably YMODEM's packet buffer),
	 * so do not retain line pointers in an active stack frame while one runs.
	 * Advance the fixed cursor first, then execute an isolated line copy.
	 */
	static char *script_cursor;
	static char linebuf[256];
	i32 n;
	i32 rc;

	if (script == 0)
		return(-1);

	script_cursor = script;
	while (*script_cursor != '\0') {
		n = 0;
		while (*script_cursor != '\0' && *script_cursor != '\n' &&
		    *script_cursor != '\r') {
			if (n + 1 >= sizeof(linebuf)) {
				uart_puts("script: line too long\n");
				return(-1);
			}
			linebuf[n++] = *script_cursor++;
		}
		linebuf[n] = '\0';
		while (*script_cursor == '\n' || *script_cursor == '\r')
			script_cursor++;
		if (linebuf[0] != '\0') {
			uart_puts("+ ");
			uart_puts(linebuf);
			uart_putc('\n');
			rc = monitor_run_command_mode(linebuf, attached, attachedsz, cwd, cwdsz, 1);
			if (rc < 0)
				return(-1);
			monitor_script_pause();
		}
	}

	return(0);
}

static i32
monitor_run_command_mode(char *cmdbuf, char *attached, i32 attachedsz,
	char *cwd, i32 cwdsz, i32 scripted)
{
	struct command *entry;
	char *cmd;
	char *arg1;
	char *arg2;
	char buf[128];
	char buf2[128];
	i32 status;

	split_cmdbuf(cmdbuf, &cmd, &arg1, &arg2);
	if (cmd == 0)
		return(0);
	if (cmd[0] == '#')
		return(0);
	if (arg1 != 0 && arg1[0] == '#') {
		arg1 = 0;
		arg2 = 0;
	} else if (arg2 != 0 && arg2[0] == '#')
		arg2 = 0;

	entry = find_command(cmd);
	if (entry == 0) {
		uart_puts("Unknown command\n");
		return(-1);
	}

	if (validate_arg(entry->name, entry->arg1, arg1, buf, sizeof(buf), attached, cwd) < 0)
		return(-1);
	if (validate_arg(entry->name, entry->arg2, arg2, buf2, sizeof(buf2), attached, cwd) < 0)
		return(-1);

	status = 0;
	switch (entry->id) {
	case CMD_HELP:
		print_help();
		break;

	case CMD_ATTACH:
		status = attach(arg1);
		if (status == 0) {
			if (strlen(arg1) + 1 > attachedsz) {
				uart_puts("attach: devspec too long\n");
				return(-1);
			}
			strcpy(attached, arg1);
			strcpy(cwd, ".");
		}
		break;

	case CMD_MKFS:
		makefsv7(arg1, atol(arg2));
		break;

	case CMD_MKNOD: {
		i32 mode;
		dev_t dev;

		if (parse_mknod_spec(arg2, &mode, &dev) < 0)
			return(-1);
		status = mknod(buf, mode, dev);
		break;
	}

	case CMD_MKDIR: {
		i32 mode;

		mode = arg2 == 0 ? 0755 : atoo(arg2);
		/* Stage loaders are intentionally restartable.  An existing path
		 * already satisfies a scripted mkdir, but still apply its requested
		 * mode just as a following chmod command would have done. */
		if (scripted && exists(buf) > 0) {
			status = chmod(buf, mode);
			break;
		}
		status = mkdir(buf);
		if (status == 0)
			status = chmod(buf, mode);
		if (scripted && status < 0)
			status = 0;
		break;
	}

	case CMD_RMDIR:
		status = rmdir(buf);
		break;

	case CMD_PWD:
		print_pwd(cwd);
		break;

	case CMD_RM:
		status = rm(buf);
		break;

	case CMD_CHGRP:
		status = chgrp(buf2, atoi(arg1));
		break;

	case CMD_CHOWN:
		status = chown(buf2, atoi(arg1));
		break;

	case CMD_CHMOD:
		status = chmod(buf2, atoo(arg1));
		break;

	case CMD_LINK:
		/* The compatibility links in loader stages are likewise idempotent. */
		if (scripted && exists(buf2) > 0)
			break;
		status = link(buf, buf2);
		break;

	case CMD_CD: {
		char newcwd[64];
		i32 fd;

		if (normalize_cwd(newcwd, sizeof(newcwd), cwd, arg1) < 0) {
			uart_puts("cd: bad path\n");
			break;
		}
		if (v7fs_resolve_path(buf, sizeof(buf), attached, newcwd, ".") < 0) {
			uart_puts("cd: bad path\n");
			break;
		}
		fd = v7fs_isdir(buf);
		if (fd < 0) {
			status = -1;
			break;
		}
		if (fd == 0) {
			uart_puts("cd: not a directory\n");
			break;
		}
		if (strlen(newcwd) + 1 > cwdsz) {
			uart_puts("cd: path too long\n");
			break;
		}
		strcpy(cwd, newcwd);
		break;
	}

	case CMD_LS:
		ls(buf);
		break;

	case CMD_HD:
		status = hd(buf);
		break;

	case CMD_CP:
		status = cp(buf, buf2);
		break;

	case CMD_MV:
		status = mv(buf, buf2);
		break;

	case CMD_CAT:
		status = cat(buf);
		break;

	case CMD_RX:
		if (arg1 != 0 && *arg1 != '\0') {
			if (v7fs_resolve_path(buf, sizeof(buf), attached, cwd, arg1) < 0) {
				uart_puts("rx: bad path\n");
				status = -1;
				break;
			}
			/* open() splits nested paths in place while creating the file. */
			if (arg2 != 0 && *arg2 != '\0')
				strcpy(buf2, buf);
			if (scripted)
				status = ymodem_receive_monitor_quiet(EPOCH68_UART1, attached, cwd, arg1, buf);
			else
				status = ymodem_receive_monitor(EPOCH68_UART1, attached, cwd, arg1, buf);
			if (status == 0 && arg2 != 0 && *arg2 != '\0')
				status = chmod(buf2, atoo(arg2));
			break;
		}
		status = ymodem_receive_monitor(EPOCH68_UART1, attached, cwd, 0, 0);
		break;

	case CMD_RXSCRIPT: {
		/*
		 * Stage loader scripts now include full manpage trees, so keep
		 * enough room for roughly 18KB today with some growth headroom.
		 */
		static char scriptbuf[32768];

		status = ymodem_receive_buffer(EPOCH68_UART1, arg1, scriptbuf, sizeof(scriptbuf));
		if (status < 0)
			break;
		status = monitor_run_script(scriptbuf, attached, attachedsz, cwd, cwdsz);
		break;
	}

	case CMD_RXSCRIPTS: {
		char *request;
		/*
		 * A received script may exercise enough monitor and filesystem code
		 * to reuse the command-line stack frame.  Keep the complete queue in
		 * fixed storage, rather than retaining pointers into that frame or the
		 * UART input buffer while the script runs.
		 */
		static char requestbuf[256];
		static char *requests;
		/* Reuse one receive buffer after each script has completed. */
		static char scriptbuf[32768];

		if (arg1 == 0)
			break;
		if (strlen(arg1) + (arg2 != 0 && *arg2 != '\0' ? strlen(arg2) + 2 : 1) >
		    sizeof(requestbuf)) {
			uart_puts(".: script list too long\n");
			status = -1;
			break;
		}
		strcpy(requestbuf, arg1);
		if (arg2 != 0 && *arg2 != '\0') {
			strcat(requestbuf, " ");
			strcat(requestbuf, arg2);
		}
		requests = requestbuf;
		request = next_word(&requests);
		do {
			if (request != 0 && request[0] == '#')
				break;
			status = ymodem_receive_buffer(EPOCH68_UART1, request, scriptbuf,
				sizeof(scriptbuf));
			if (status < 0)
				break;
			status = monitor_run_script(scriptbuf, attached, attachedsz, cwd,
				cwdsz);
			if (status < 0)
				break;
			if (requests != 0)
				request = next_word(&requests);
			else
				request = 0;
		} while (request != 0);
		break;
	}

	case CMD_BOOT:
		status = 0;
		if (arg1 != 0 && strcmp(arg1, "-m") == 0) {
			arg2 = arg1;
			arg1 = 0;
		}
		if (arg2 != 0 && *arg2 != '\0') {
			if (strcmp(arg2, "-m") != 0) {
				uart_puts("boot: bad option\n");
				status = -1;
				break;
			}
			status = 1;
		}
		if (arg1 != 0 && *arg1 != '\0') {
			if (v7fs_resolve_path(buf, sizeof(buf), attached, cwd, arg1) < 0) {
				uart_puts("boot: bad path\n");
				status = -1;
				break;
			}
		} else {
			if (v7fs_resolve_path(buf, sizeof(buf), attached, cwd, "/unix") < 0) {
				uart_puts("boot: bad path\n");
				status = -1;
				break;
			}
		}
		status = boot(buf, status);
		break;

	case CMD_USER:
		status = userboot(buf);
		break;
	}

	if (status < 0) {
		uart_puts(entry->name);
		uart_puts(" failed\n");
	}

	return(status);
}

i32
monitor_run_command(char *cmdbuf, char *attached, i32 attachedsz, char *cwd,
	i32 cwdsz)
{
	return(monitor_run_command_mode(cmdbuf, attached, attachedsz, cwd, cwdsz, 0));
}
