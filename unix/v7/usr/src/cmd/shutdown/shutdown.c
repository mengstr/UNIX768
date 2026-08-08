/*
 * Epoch68 local shutdown command.
 *
 * This is intentionally not historical V7 code.  It gives the emulator
 * bring-up system a manual stop path that flushes disks and asks the kernel
 * to return to the ROM monitor.
 */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

i32 epoch68_shutdown(void);

static void
syncwait(void)
{
	sync();
	(void)sleep(1);
	sync();
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if(getuid() != 0) {
		printf("shutdown: not super-user\n");
		exit(1);
	}
	signal(SIGTERM, SIG_IGN);

	printf("\nShutdown started\n");
	fflush(stdout);
	syncwait();

	printf("shutdown: returning to monitor\n");
	fflush(stdout);
	epoch68_shutdown();
	printf("shutdown: monitor return failed\n");
	exit(1);
}
