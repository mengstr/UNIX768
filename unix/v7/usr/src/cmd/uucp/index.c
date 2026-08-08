#include <stdio.h>
#include <sys/types.h>


/*******
 *	char *
 *	index(str, c)	return pointer to character c
 *	char c, *str;
 *
 *	return codes:
 *		NULL  -  character not found
 *		pointer  -  pointer to character
 */

char *
index (char *str, i32 c)
{
	for (; *str != '\0'; str++) {
		if (*str == c)
			return(str);
	}

	return(NULL);
}
