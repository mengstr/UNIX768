/*
 * Read the ten-byte key/salt input used by crypt(1) and ed(1), then emit
 * the thirteen-byte result of the V7 crypt(3) routine.
 */

#include <unistd.h>

extern char *crypt(char *pw, char *salt);

i32
main(void)
{
	char key[9];
	char salt[3];

	if (read(0, key, 8) != 8 || read(0, salt, 2) != 2)
		return(1);
	key[8] = '\0';
	salt[2] = '\0';
	if (write(1, crypt(key, salt), 13) != 13)
		return(1);
	return(0);
}
