/*
 * Anachronistic POSIX/C helper for target-building ACK tools.
 * Keep this file minimal; it is not historical V7 userland API.
 */
#ifndef _STDDEF_H_
#define _STDDEF_H_

#include "sys/inttypes.h"

#define NULL	0

typedef u16	size_t;
typedef i32	ptrdiff_t;

#endif
