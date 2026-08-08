#include <stdio.h>
#include <string.h>
#include <unistd.h>

int opterr = 1;
int optind = 1;
int optopt;
char *optarg;

static int scan = 1;

static void
option_error(char *program, char *message, int option)
{
	if (!opterr)
		return;
	fputs(program, stderr);
	fputs(message, stderr);
	fputc(option, stderr);
	fputc('\n', stderr);
}

int
getopt(int argc, char **argv, char *options)
{
	int c;
	char *option;

	if (scan == 1) {
		if (optind >= argc || argv[optind][0] != '-' ||
		    argv[optind][1] == '\0')
			return EOF;
		if (strcmp(argv[optind], "--") == 0) {
			optind++;
			return EOF;
		}
	}
	optopt = c = argv[optind][scan];
	option = strchr(options, c);
	if (c == ':' || option == NULL) {
		option_error(argv[0], ": illegal option -- ", c);
		if (argv[optind][++scan] == '\0') {
			optind++;
			scan = 1;
		}
		return '?';
	}
	if (option[1] == ':') {
		if (argv[optind][scan + 1] != '\0')
			optarg = &argv[optind++][scan + 1];
		else if (++optind >= argc) {
			option_error(argv[0], ": option requires an argument -- ", c);
			scan = 1;
			return '?';
		} else
			optarg = argv[optind++];
		scan = 1;
	} else {
		if (argv[optind][++scan] == '\0') {
			scan = 1;
			optind++;
		}
		optarg = NULL;
	}
	return c;
}
