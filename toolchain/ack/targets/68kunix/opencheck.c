#include <stdio.h>

extern int open();
extern int close();
extern int creat();
extern int write();
extern int unlink();

static void
show_open(name)
char *name;
{
	int fd;
	FILE *fp;

	fd = open(name, 0);
	printf("open %s = %d\n", name, fd);
	if (fd >= 0)
		close(fd);

	fp = fopen(name, "r");
	printf("fopen %s = %s\n", name, fp ? "non-null" : "null");
	if (fp)
		fclose(fp);
}

int
main()
{
	int fd;

	printf("opencheck\n");

	unlink("ack_missing_2880");
	show_open("2880");
	show_open("ack_missing_2880");

	fd = creat("ack_present_2880", 0666);
	printf("creat ack_present_2880 = %d\n", fd);
	if (fd >= 0) {
		write(fd, "x\n", 2);
		close(fd);
	}
	show_open("ack_present_2880");
	unlink("ack_present_2880");

	return 0;
}
