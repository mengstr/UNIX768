#ifndef UTIL_H_
#define UTIL_H_

#include "arith.h"

struct localvar
{
	struct localvar* next;
	arith t_offset; /* offset from LocalBase */
	arith t_size;
	int t_align;
	int t_regtype;
	int t_count;
	int t_sc; /* storage class */
};


/* allocation definitions of struct localvar */
extern struct localvar *h_localvar;
#ifdef DEBUG
extern int cnt_localvar;
extern char *std_alloc();
#define new_localvar() ((struct localvar *) std_alloc((char **)&h_localvar, sizeof(struct localvar), 10, &cnt_localvar))
#else
#define	new_localvar() ((struct localvar *) st_alloc((char **)&h_localvar, sizeof(struct localvar), 10))
#endif
#define	free_localvar(p) st_free(p, &h_localvar, sizeof(struct localvar))


void LocalInit(void);
arith LocalSpace(arith sz, int al);
arith NewLocal(arith sz, int al, int regtype, int sc);
void FreeLocal(arith off);
void LocalFinish(void);
void RegisterAccount(arith offset, arith size, int regtype, int sc);
void LoadLocal(arith off, arith sz);
void StoreLocal(arith off, arith sz);

#ifndef LINT
void AddrLocal(arith off);
#endif

#endif