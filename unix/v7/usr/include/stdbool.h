/*
 * Anachronistic POSIX/C99 helper for target-building ACK tools.
 * Keep this file minimal; it is not historical V7 userland API.
 */
#ifndef _STDBOOL_H_
#define _STDBOOL_H_

#include "sys/inttypes.h"

typedef u8	bool;

#define true	((bool)1)
#define false	((bool)0)
#define __bool_true_false_are_defined	1

#endif
