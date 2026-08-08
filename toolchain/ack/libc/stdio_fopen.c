#include "stdio_private.h"

FILE *
fopen(file, mode)
	char *file, *mode;
{
	return _endopen(file, mode, _findiop());
}

