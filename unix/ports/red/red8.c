/*
 * RED operating-system adaptation for platform/epoch68/emulator UNIX V7.
 */

#include "red.h"

#include <signal.h>

static struct sgttyb saved_tty;
static int tty_saved;
static char input_buffer[64];
static int input_count;
static int input_index;
static char pushback[2];
static int pushback_count;
static char output_buffer[512];
static int output_count;

static void
terminal_signal(i16 signo)
{
	sysend();
	_exit(128 + signo);
}

static int
flush_output(void)
{
	int done;
	int n;

	done = 0;
	while (done < output_count) {
		n = write(1, output_buffer + done, output_count - done);
		if (n <= 0)
			return ERROR;
		done += n;
	}
	output_count = 0;
	return OK;
}

static int
fill_input(void)
{
	flush_output();
	input_count = read(0, input_buffer, sizeof(input_buffer));
	input_index = 0;
	return input_count;
}

static int
next_input(int *was_pushed)
{
	if (pushback_count != 0) {
		*was_pushed = YES;
		return pushback[--pushback_count] & 0x7f;
	}
	*was_pushed = NO;
	if (input_index >= input_count && fill_input() <= 0)
		return CPMEOF;
	return input_buffer[input_index++] & 0x7f;
}

int
sysinit(void)
{
	struct sgttyb raw;
	i32 pid;

	pid = getpid();
	sprintf(red_temp_file, "/tmp/rt%ld", (long)pid);
	sprintf(red_data_file, "/tmp/rd%ld", (long)pid);

	tty_saved = gtty(0, &saved_tty) == 0;
	if (tty_saved) {
		raw = saved_tty;
		raw.sg_flags |= RAW;
		raw.sg_flags &= ~(ECHO | CRMOD);
		if (stty(0, &raw) < 0)
			tty_saved = NO;
	}
	if (tty_saved) {
		signal(SIGHUP, terminal_signal);
		signal(SIGQUIT, terminal_signal);
		signal(SIGTERM, terminal_signal);
	}
	input_count = 0;
	input_index = 0;
	pushback_count = 0;
	output_count = 0;
	return OK;
}

void
sysend(void)
{
	flush_output();
	if (tty_saved) {
		stty(0, &saved_tty);
		tty_saved = NO;
	}
	unlink(TEMP_FILE);
	unlink(DATA_FILE);
}

char *
sysalloc(int n)
{
	return malloc((unsigned)n);
}

int
sysmove(char source[], char dest[], int count)
{
	memmove(dest, source, (unsigned)count);
	return OK;
}

int
sysintr(int systl, int systy, int sysn)
{
	(void)systl;
	(void)systy;
	(void)sysn;
	return OK;
}

int
syscstat(void)
{
	if (pushback_count != 0)
		return pushback[--pushback_count] & 0x7f;
	if (input_index >= input_count)
		return ERROR;
	return input_buffer[input_index++] & 0x7f;
}

int
syscin(void)
{
	int c;
	int c1;
	int c2;
	int was_pushed;

	c = next_input(&was_pushed);
	if (c == ESCAPE && !was_pushed) {
		c1 = next_input(&was_pushed);
		if (c1 != '[') {
			pushback[pushback_count++] = c1;
			return ESCAPE;
		}
		c2 = next_input(&was_pushed);
		switch (c2) {
		case 'A':
			return UP;
		case 'B':
			return DOWN;
		case 'C':
			return RIGHT;
		case 'D':
			return LEFT;
		}
		pushback[pushback_count++] = c2;
		pushback[pushback_count++] = c1;
	}
	return c;
}

int
syswait(void)
{
	return flush_output();
}

int
syscout(int c)
{
	if (output_count == sizeof(output_buffer) && flush_output() == ERROR)
		return ERROR;
	output_buffer[output_count++] = c;
	return c;
}

int
syslout(int c)
{
	return syscout(c);
}

FILE *
sysfcreat(char *filename, FILE *buffer)
{
	(void)buffer;
	return fopen(filename, "w");
}

FILE *
sysfopen(char *filename, FILE *buffer)
{
	(void)buffer;
	return fopen(filename, "r");
}

int
sysfclose(FILE *fd)
{
	return fclose(fd);
}

int
sysfflush(FILE *fd)
{
	return fflush(fd);
}

int
sysgetc(FILE *fd)
{
	int c;

	c = getc(fd);
	return c == EOF ? ERROR : c & 0x7f;
}

int
sysfgets(FILE *fd, char *buffer, int maxlen)
{
	int c;
	int count;

	count = 0;
	for (;;) {
		c = sysgetc(fd);
		if (c == CR)
			continue;
		if (c == ERROR || c == CPMEOF) {
			if (count == 0)
				return ERROR;
			break;
		}
		if (c == NEWLINE)
			break;
		if (count < maxlen - 2)
			buffer[count] = c;
		count++;
	}
	buffer[count < maxlen - 2 ? count : maxlen - 2] = NEWLINE;
	buffer[count < maxlen - 1 ? count + 1 : maxlen - 1] = EOS;
	return count;
}

int
sysputc(int c, FILE *fd)
{
	return putc(c, fd);
}

int
syscreat(char *filename)
{
	return creat(filename, 0666);
}

int
sysopen(char *name, int mode)
{
	return open(name, mode);
}

int
sysclose(int fd)
{
	return close(fd);
}

int
sysunlink(char *filename)
{
	return unlink(filename);
}

int
sysrename(char *oldname, char *newname)
{
	unlink(newname);
	if (link(oldname, newname) < 0)
		return ERROR;
	return unlink(oldname);
}

int
sysseek(int fd, int block)
{
	i32 offset;

	offset = (i32)block * DATA_SIZE;
	return lseek(fd, offset, 0) < 0 ? ERROR : OK;
}

int
sysread(int fd, char *buffer)
{
	int n;
	int sectors;

	n = read(fd, buffer, DATA_SIZE);
	if (n < 0)
		return ERROR;
	if (n == DATA_SIZE)
		return READ_SIZE;
	sectors = (n + CPM_SIZE - 1) / CPM_SIZE;
	while (n < sectors * CPM_SIZE)
		buffer[n++] = CPMEOF;
	return sectors;
}

int
syswrite(int fd, char *buffer, int n)
{
	int bytes;

	bytes = n * CPM_SIZE;
	return write(fd, buffer, bytes) == bytes ? n : ERROR;
}

int
sysexists(char *filename)
{
	int fd;

	fd = sysopen(filename, 0);
	if (fd == ERROR)
		return NO;
	sysclose(fd);
	return YES;
}

int
syscopfn(char *args, char *buffer)
{
	int n;

	for (n = 0; n < SYSFNMAX - 1 && args[n] != EOS &&
	    args[n] != ' '; n++)
		buffer[n] = args[n];
	buffer[n] = EOS;
	return n;
}
