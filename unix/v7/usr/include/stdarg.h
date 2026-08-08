#ifndef _STDARG_H_
#define _STDARG_H_

typedef char *va_list;

#define va_start(list, lastarg) ((list) = (char *)(&lastarg + 1))
#define va_end(list) ((list) = (va_list)0)
#define va_arg(list, mode) ((mode *)((list) += sizeof(mode)))[-1]

#endif
