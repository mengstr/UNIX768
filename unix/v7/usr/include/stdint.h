/*
 * Anachronistic C99 helper for target-building ACK tools.
 * Keep this file minimal; it is not historical V7 userland API.
 */
#ifndef _STDINT_H_
#define _STDINT_H_

#include "sys/inttypes.h"

typedef i8	int8_t;
typedef u8	uint8_t;
typedef i16	int16_t;
typedef u16	uint16_t;
typedef i32	int32_t;
typedef u32	uint32_t;
#ifdef ACK_TARGET_NO_LONG_LONG
typedef i32	int64_t;
typedef u32	uint64_t;
#else
typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;
#endif
typedef i32	intptr_t;
typedef u32	uintptr_t;

#define SIZE_MAX	((u16)-1)

#endif
