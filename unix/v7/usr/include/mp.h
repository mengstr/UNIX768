#include "sys/inttypes.h"

#define MINT struct mint
MINT
{	i32 len;
	short *val;
};
#define FREE(x) {if(x.len!=0) {free((char *)x.val); x.len=0;}}
#ifndef DBG
#define shfree(u) free((char *)u)
#else
#include "stdio.h"
#define shfree(u) { if(dbg) fprintf(stderr, "free %o\n", u); free((char *)u);}
extern i32 dbg;
#endif
struct half
{	short high;
	short low;
};
extern MINT *itom();
extern short *xalloc();

#ifdef lint
extern xv_oid;
#define VOID xv_oid =
#else
#define VOID
#endif
