#ifndef EPOCH68_LIBC_TYPES_H
#define EPOCH68_LIBC_TYPES_H

/*
 * ACK target configuration for Epoch68 is currently:
 *   char=1, short=2, int=2, long=4, pointer=4.
 *
 * Keep these aliases local to the brought-up libc/runtime so width-sensitive
 * code does not silently depend on host C spelling.
 *
 * `usize` deliberately follows the target C unsigned-int width, not the full
 * pointer width. V7/K&R application code normally calls libc without
 * prototypes, so size-like int expressions are passed as one 16-bit word.
 * Syscall-sized address arithmetic uses `isize`/`off32` explicitly.
 */
#ifndef V7_SYS_INTTYPES_H
typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed long i32;
typedef unsigned long u32;
#endif

typedef u16 usize;
typedef i32 isize;
typedef i32 off32;
typedef i32 time32;

#endif
