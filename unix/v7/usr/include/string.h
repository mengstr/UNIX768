#include "stddef.h"
#include "sys/inttypes.h"

#ifndef _STRING_H_
#define _STRING_H_

char	*strcat(char *, const char *);
char	*strcpy(char *, const char *);
char	*strdup(const char *);
char	*strncat(char *, const char *, u16);
char	*strncpy(char *, const char *, u16);
void	*memcpy(void *, const void *, size_t);
void	*memmove(void *, const void *, size_t);
void	*memset(void *, i32, size_t);
i32	memcmp(const void *, const void *, size_t);
char	*strchr(const char *, i32);
char	*strrchr(const char *, i32);
char	*index(char *, i32);
char	*rindex(char *, i32);
i32	strcmp(const char *, const char *);
i32	strncmp(const char *, const char *, u16);
u16	strlen(const char *);

#endif
