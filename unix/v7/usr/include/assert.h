#ifndef _ASSERT_H_
#define _ASSERT_H_

#include "stdio.h"
#include "stdlib.h"

# ifndef NDEBUG
# define _assert(ex) {if (!(ex)){fprintf(stderr,"Assertion failed: file %s, line %d\n", __FILE__, __LINE__);exit(1);}}
# define assert(ex) {if (!(ex)){fprintf(stderr,"Assertion failed: file %s, line %d\n", __FILE__, __LINE__);exit(1);}}
# else
# define _assert(ex) ;
# define assert(ex) ;
# endif

#endif
