/* Epoch68 package extractor: fail unless both pcat and tar succeed. */
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
	i16 f[2];
	int pcat, tar;
	i16 status;
	i16 pid;
	int i;
	int pcat_status;
	int tar_status;

	if (argc != 3)
		return 1;
	fprintf(stderr, "instar: unpacking %s\n", argv[1]);
	if (pipe(f) < 0)
		return 1;
	if ((pcat = fork()) == 0) {
		close(1);
		dup(f[1]);
		close(f[0]);
		close(f[1]);
		execl("/local/bin/pcat", "pcat", argv[1], (char *)0);
		exit(1);
	}
	if (pcat < 0)
		return 1;
	if ((tar = fork()) == 0) {
		close(0);
		dup(f[0]);
		close(f[0]);
		close(f[1]);
		if (chdir(argv[2]) < 0)
			exit(1);
		execl("/bin/tar", "tar", "xf", "-", (char *)0);
		exit(1);
	}
	close(f[0]);
	close(f[1]);
	if (tar < 0)
		return 1;
	pcat_status = -1;
	tar_status = -1;
	for (i = 0; i < 2; i++) {
		pid = wait(&status);
		if (pid == pcat)
			pcat_status = status;
		else if (pid == tar)
			tar_status = status;
	}
	if (pcat_status != 0) {
		fprintf(stderr, "instar: pcat failed for %s\n", argv[1]);
		return 1;
	}
	if (tar_status != 0) {
		fprintf(stderr, "instar: tar failed for %s\n", argv[1]);
		return 1;
	}
	fprintf(stderr, "instar: unpacked %s\n", argv[1]);
	return 0;
}
