#include "sys/inttypes.h"

/*
 * Structure of a symbol table entry
 */

struct	symbol {
	char	sy_name[8];
	char	sy_type;
	i32	sy_value;
};
