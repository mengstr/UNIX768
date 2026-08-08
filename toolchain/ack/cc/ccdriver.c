/*
 * Native driver for the 68000 ACK C toolchain.
 *
 * Keep this deliberately old-fashioned: it is compiled by both the host
 * cross compiler and the V7 native compiler.  The two installation-specific
 * directories are supplied at compile time.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef CC_TOOL_DIR
#define CC_TOOL_DIR "/usr/lib/ack"
#endif
#ifndef CC_RUNTIME_DIR
#define CC_RUNTIME_DIR "/lib"
#endif
#ifndef CC_INCLUDE_DIR
#define CC_INCLUDE_DIR "/usr/include"
#endif
#ifndef CC_CRT_OBJECT
#define CC_CRT_OBJECT "crt0.o"
#endif
#ifndef CC_END_OBJECT
#define CC_END_OBJECT "end.o"
#endif

#ifndef CC_MAXARG
#define CC_MAXARG 512
#endif
#ifndef CC_WAIT_STATUS
#define CC_WAIT_STATUS short
#endif
#define MAXARG CC_MAXARG
#define MAXSRC 64

static int verbose;
static int keep;
static int serial;
static char *objects[MAXSRC];
static int nobjects;

static void
usage(void)
{
	fprintf(stderr, "usage: cc [-O0|-O1|-O2] [-c] [-k] [-s] [-w] [-o output] [cpp flags] file ...\n");
	exit(1);
}

static int
endswith(char *s, char *tail)
{
	int a = strlen(s);
	int b = strlen(tail);
	return a >= b && strcmp(s + a - b, tail) == 0;
}

static void
run(char **av)
{
	int pid, i;
	CC_WAIT_STATUS status;
	if (verbose) {
		for (i = 0; av[i]; i++)
			fprintf(stderr, "%s%s", i ? " " : "", av[i]);
		fprintf(stderr, "\n");
	}
	pid = fork();
	if (pid < 0) {
		perror("cc: fork");
		exit(1);
	}
	if (pid == 0) {
		execv(av[0], av);
		perror(av[0]);
		_exit(1);
	}
	if (wait(&status) < 0 || status != 0) {
		fprintf(stderr, "cc: %s failed\n", av[0] + strlen(CC_TOOL_DIR) + 1);
		exit(1);
	}
}

static void
runout(char **av, char *file)
{
	int pid, fd, i;
	CC_WAIT_STATUS status;
	if (verbose) {
		for (i = 0; av[i]; i++) fprintf(stderr, "%s%s", i ? " " : "", av[i]);
		fprintf(stderr, " > %s\n", file);
	}
	pid = fork();
	if (pid < 0) { perror("cc: fork"); exit(1); }
	if (pid == 0) {
		fd = creat(file, 0666);
		if (fd < 0) { perror(file); _exit(1); }
		close(1); dup(fd); close(fd);
		execv(av[0], av); perror(av[0]); _exit(1);
	}
	if (wait(&status) < 0 || status != 0) {
		fprintf(stderr, "cc: %s failed\n", av[0] + strlen(CC_TOOL_DIR) + 1);
		exit(1);
	}
}

static void
tool(char *out, char *name)
{
	sprintf(out, "%s/%s", CC_TOOL_DIR, name);
}

static void
runtime(char *out, char *name)
{
	sprintf(out, "%s/%s", CC_RUNTIME_DIR, name);
}

static void
compile(char *source, char *output, int optlevel, int warning, char **cppflags, int ncpp)
{
	char cpp[128], c1[128], opt[128], ncg[128], as[128];
	char pre[64], ack[64], em[64], asmfile[64];
	char *av[MAXARG];
	int n, i;

	serial++;
	sprintf(pre, "/tmp/cc%d.i", serial);
	sprintf(ack, "/tmp/cc%d.k", serial);
	sprintf(em, "/tmp/cc%d.m", serial);
	sprintf(asmfile, "/tmp/cc%d.s", serial);
	tool(cpp, "cpp"); tool(c1, "c1"); tool(opt, "opt");
	tool(ncg, "ncg"); tool(as, "as");

	n = 0; av[n++] = cpp;
	/* cpp needs the option and its path as one argument. */
	{
		static char include[256];
		sprintf(include, "-I%s", CC_INCLUDE_DIR);
		av[n++] = include;
	}
	for (i = 0; i < ncpp; i++) av[n++] = cppflags[i];
	av[n++] = source; av[n++] = 0; runout(av, pre);

	n = 0; av[n++] = c1; av[n++] = "-L";
	av[n++] = "-Vw2.2i2.2p4.2f4.2s2.2l4.2d8.2x8.2";
	if (warning) av[n++] = "-a";
	av[n++] = pre; av[n++] = ack; av[n++] = 0; run(av);
	n = 0; av[n++] = opt;
	if (optlevel == 0) av[n++] = "-n";
	if (optlevel == 2) { tool(opt, "opt2"); av[0] = opt; }
	av[n++] = ack; av[n++] = 0; runout(av, em);
	n = 0; av[n++] = ncg; av[n++] = em; av[n++] = asmfile; av[n++] = 0; run(av);
	n = 0; av[n++] = as; av[n++] = "-"; av[n++] = "-o"; av[n++] = output;
	av[n++] = asmfile; av[n++] = 0; run(av);
	if (!keep) { unlink(pre); unlink(ack); unlink(em); unlink(asmfile); }
}

int
main(int argc, char **argv)
{
	char *cppflags[MAXARG], *inputs[MAXARG], *sources[MAXSRC];
	char output[128], object[128], tmpout[64], led[128], cv[128];
	char crt[128], libc[128], libfp[128], end[128];
	char *av[MAXARG];
	int i, ncpp, ninput, nsource, compileonly, strip, warning, optlevel, n;

	ncpp = ninput = nsource = compileonly = strip = warning = 0;
	optlevel = 1; output[0] = 0;
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-O0")) optlevel = 0;
		else if (!strcmp(argv[i], "-O1")) optlevel = 1;
		else if (!strcmp(argv[i], "-O2")) optlevel = 2;
		else if (!strcmp(argv[i], "-c")) compileonly = 1;
		else if (!strcmp(argv[i], "-k")) keep = 1;
		else if (!strcmp(argv[i], "-s")) strip = 1;
		else if (!strcmp(argv[i], "-v")) verbose = 1;
		else if (!strcmp(argv[i], "-w")) warning = 1;
		else if (!strcmp(argv[i], "-o")) {
			if (++i == argc) usage();
			strcpy(output, argv[i]);
		} else if (!strcmp(argv[i], "-D") || !strcmp(argv[i], "-I") || !strcmp(argv[i], "-U")) {
			if (++i == argc) usage();
			cppflags[ncpp++] = argv[i - 1]; cppflags[ncpp++] = argv[i];
		} else if (!strncmp(argv[i], "-D", 2) || !strncmp(argv[i], "-I", 2) || !strncmp(argv[i], "-U", 2))
			cppflags[ncpp++] = argv[i];
		else if (argv[i][0] == '-') usage();
		else if (endswith(argv[i], ".c") || endswith(argv[i], ".C")) sources[nsource++] = argv[i];
		else inputs[ninput++] = argv[i];
	}
	if (!nsource && !ninput) usage();
	if (compileonly && (!nsource || ninput || nsource != 1)) {
		fprintf(stderr, "cc: -c requires exactly one C source\n"); return 1;
	}
	for (i = 0; i < nsource; i++) {
		if (compileonly && output[0]) strcpy(object, output);
		else { strcpy(object, sources[i]); object[strlen(object) - 1] = 'o'; }
		compile(sources[i], object, optlevel, warning, cppflags, ncpp);
		objects[nobjects++] = strdup(object);
	}
	if (compileonly) return 0;
	if (!output[0]) strcpy(output, "a.out");
	sprintf(tmpout, "/tmp/cc%d.a", ++serial);
	tool(led, "led"); tool(cv, "cv");
	runtime(crt, CC_CRT_OBJECT); runtime(libc, "libc.a");
	runtime(libfp, "libfp.a"); runtime(end, CC_END_OBJECT);
	n = 0; av[n++] = led; if (strip) av[n++] = "-s"; av[n++] = "-o"; av[n++] = tmpout;
	av[n++] = crt;
	for (i = 0; i < nobjects; i++) av[n++] = objects[i];
	for (i = 0; i < ninput; i++) av[n++] = inputs[i];
	av[n++] = libc; av[n++] = libfp; av[n++] = libc; av[n++] = end; av[n++] = 0; run(av);
	n = 0; av[n++] = cv; av[n++] = tmpout; av[n++] = output; av[n++] = 0; run(av);
	if (!keep) unlink(tmpout);
	return 0;
}
