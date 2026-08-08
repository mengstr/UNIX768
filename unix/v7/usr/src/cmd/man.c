#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "sys/inttypes.h"

static char *suffixes[] = {
	"",
	"m",
	"c",
	"g",
	"s",
	"x",
	0
};

static i32 exists(char *path);
static i32 find_page(char *out, char *name, char *section);
static void usage(void);

static i32
exists(char *path)
{
	return(access(path, 0) == 0);
}

static i32
find_page(char *out, char *name, char *section)
{
	char secbuf[2];
	char **suf;
	i32 sec;

	if (section != 0) {
		for (suf = suffixes; *suf != 0; suf++) {
			sprintf(out, "/usr/man/man%s/%s.%s%s", section, name, section, *suf);
			if (exists(out))
				return(1);
		}
		return(0);
	}
	for (sec = 1; sec <= 8; sec++) {
		secbuf[0] = '0' + sec;
		secbuf[1] = '\0';
		if (find_page(out, name, secbuf))
			return(1);
	}
	return(0);
}

static void
usage(void)
{
	fputs("usage: man [section] name ...\n", stderr);
}

int
main(int argc, char **argv)
{
	char path[128];
	char *section;
	char *args[4];
	i32 i;
	i32 status;

	if (argc < 2) {
		usage();
		return(1);
	}
	section = 0;
	i = 1;
	if (argv[i][0] >= '1' && argv[i][0] <= '8' && argv[i][1] == '\0') {
		section = argv[i];
		i++;
	}
	if (i >= argc) {
		usage();
		return(1);
	}
	status = 0;
	for (; i < argc; i++) {
		if (!find_page(path, argv[i], section)) {
			fprintf(stderr, "man: no entry for %s\n", argv[i]);
			status = 1;
			continue;
		}
		args[0] = "cawf";
		args[1] = "-man";
		args[2] = path;
		args[3] = 0;
		if (fork() == 0) {
			execv("/local/bin/cawf", args);
			execv("/bin/cawf", args);
			perror("cawf");
			exit(1);
		}
		wait(0);
	}
	return(status);
}
