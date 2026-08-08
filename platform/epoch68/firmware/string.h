#ifndef EPOCH68_STRING_H
#define EPOCH68_STRING_H

#include <sys/inttypes.h>

#ifdef __clang__
#include <stddef.h>
#else
/*
 * The ACK userland/monitor libc passes size-like arguments as target
 * unsigned-int width (16-bit on 68kunix), not full pointer width.
 * These prototypes must match libc exactly or stack cleanup corrupts
 * the caller frame.
 */
typedef u16 size_t;
#endif

void *memcpy(void *dst, const void *src, size_t count);
void *memset(void *dst, i32 value, size_t count);
i32 memcmp(const void *left, const void *right, size_t count);
size_t strlen(const char *s);
char *strchr(const char *s, i32 ch);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
i32 atoi(const char *s);
i32 atoo(const char *s);
i32 strcmp(const char *left, const char *right);
i32 strncmp(const char *left, const char *right, size_t count);
i32 strncasecmp(const char *left, const char *right, size_t count);
char *next_word(char **line);
void split_cmdbuf(char *cmdbuf, char **cmd, char **arg1, char **arg2);

#endif
