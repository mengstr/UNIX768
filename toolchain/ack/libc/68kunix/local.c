#include <sys/inttypes.h>

void user_trace_puts(char *s);
void user_trace_hex32(u32 value);

#ifndef EPOCH68_USER_TRACE
#define EPOCH68_USER_TRACE 0
#endif

void TRACE(char *s)
{
#if EPOCH68_USER_TRACE
	user_trace_puts(s);
#else
	s = s;
#endif
}

void TRACE_STR(char *label, char *s)
{
#if EPOCH68_USER_TRACE
	user_trace_puts(label);
	if (s)
		user_trace_puts(s);
	else
		user_trace_puts("(null)");
	user_trace_puts("\n");
#else
	label = label;
	s = s;
#endif
}

void TRACE_HEX(char *label, i32 value)
{
#if EPOCH68_USER_TRACE
	user_trace_puts(label);
	user_trace_hex32((u32)value);
	user_trace_puts("\n");
#else
	label = label;
	value = value;
#endif
}

void TRACE_PROCSTAT(void)
{
}
