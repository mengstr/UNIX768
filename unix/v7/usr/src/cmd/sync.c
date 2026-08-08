#include <unistd.h>

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	sync();
	return(0);
}
