#ifndef _LOCAL_H_
#define _LOCAL_H_

#pragma dontwarn 213

#include "sys/inttypes.h"

void	TRACE(char *);
void	TRACE_STR(char *, char *);
void	TRACE_HEX(char *, i32);
void	TRACE_PROCSTAT(void);

#define PROCSTAT TRACE_PROCSTAT

#endif
