#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT 8
#define SCHAR_MIN (-127 - 1)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define MB_LEN_MAX 1

#define SHRT_MIN (-32767 - 1)
#define SHRT_MAX 32767
#define USHRT_MAX 65535U

#define INT_MIN SHRT_MIN
#define INT_MAX SHRT_MAX
#define UINT_MAX USHRT_MAX

#define LONG_MIN (-2147483647L - 1L)
#define LONG_MAX 2147483647L
#define ULONG_MAX 4294967295UL

#endif
