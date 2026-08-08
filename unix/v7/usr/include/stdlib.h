#include "stddef.h"
#include "sys/inttypes.h"

#ifndef _STDLIB_H_
#define _STDLIB_H_
#ifndef _STDLIB_H
#define _STDLIB_H 1
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef i32 (*qsort_cmp_t)(const void *, const void *);

i32	atoi(const char *);
void	abort(void);
void	exit(int status);
void	free(void *p);
void	*calloc(unsigned n, unsigned size);
char	*getenv(const char *name);
void	*malloc(unsigned size);
void	qsort(void *a, size_t n, i32 es, qsort_cmp_t fc);
i32	rand(void);
void	srand(u32 seed);
int	system(char *command);
void	*realloc(void *p, unsigned size);

#endif
