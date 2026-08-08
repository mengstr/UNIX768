#ifndef _INTTYPES_H
#define _INTTYPES_H

#ifndef V7_SYS_INTTYPES_H
#define V7_SYS_INTTYPES_H

/*
 * Fixed-width integer types for the V7 / 68000 port.
 *
 * Widths are anchored to fixed 68000 ABI types: char = 8, short = 16,
 * long = 32 bits. u32/i32 are 'long', NEVER 'int' -- 'int' width is a
 * compiler policy decision, so using it for ABI data silently corrupts
 * on-disk structures (daddr_t/off_t) and setjmp buffers (label_t) when the
 * compiler changes. The asserts below turn any such mismatch into a compile
 * error instead of silent corruption.
 */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef signed char    i8;
typedef signed short   i16;
typedef signed long    i32;

/*
 * Compile-time assertion.  The negative array size makes any C compiler
 * (including non-ANSI / K&R) reject the translation unit when 'cond' is
 * false, and the typedef 'name' identifies the broken invariant in the
 * error message.  Uses only macro parameter substitution (no ## or #), so
 * it works with pre-ANSI preprocessors.  Costs nothing at runtime.
 */
#ifndef STATIC_ASSERT
#define STATIC_ASSERT(name, cond) typedef char name[1 - 2 * !(cond)]
#endif

/* Sizes must match the names. */
STATIC_ASSERT(assert_u8_size,  sizeof(u8)  == 1);
STATIC_ASSERT(assert_u16_size, sizeof(u16) == 2);
STATIC_ASSERT(assert_u32_size, sizeof(u32) == 4);
STATIC_ASSERT(assert_i8_size,  sizeof(i8)  == 1);
STATIC_ASSERT(assert_i16_size, sizeof(i16) == 2);
STATIC_ASSERT(assert_i32_size, sizeof(i32) == 4);

/* Signedness must match (portable: unsigned wraps -1 to a positive value). */
STATIC_ASSERT(assert_u8_unsigned,  (u8)-1  > 0);
STATIC_ASSERT(assert_u16_unsigned, (u16)-1 > 0);
STATIC_ASSERT(assert_u32_unsigned, (u32)-1 > 0);
STATIC_ASSERT(assert_i8_signed,    (i8)-1  < 0);
STATIC_ASSERT(assert_i16_signed,   (i16)-1 < 0);
STATIC_ASSERT(assert_i32_signed,   (i32)-1 < 0);

#endif

#endif /* _INTTYPES_H */
