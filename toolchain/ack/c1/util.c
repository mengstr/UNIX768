/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* M I S C E L L A N E O U S   U T I L I T I E S */

/* $Id$ */

/*	Code for the allocation and de-allocation of temporary variables,
    allowing re-use.
*/

#include <stddef.h>
#include <stdbool.h>
#include "parameters.h"
#ifndef LINT
#include <em.h>
#else
#include "l_em.h"
#endif /* LINT */
#include <em_arith.h>
#include <em_reg.h>
#include <alloc.h>
#include <em_mes.h>

#include "util.h"
#include "sizes.h"
#include "align.h"
#include "stack.h"
#include "Lpars.h"
#include "def.h"

static struct localvar* FreeTmps;
#ifdef USE_TMP
static int loc_id;
#endif /* USE_TMP */

#ifdef PEEPHOLE
#undef REGCOUNT
#define REGCOUNT 1
#endif

extern char options[];

#ifdef ACK_TARGET_C1_TRACE
extern void c1_trace(char*);
extern void c1_trace_value(char*, long);
#define C1_TRACE(s) c1_trace(s)
#define C1_TRACE_VALUE(s, v) c1_trace_value((s), (long)(v))
#else
#define C1_TRACE(s) ((void)0)
#define C1_TRACE_VALUE(s, v) ((void)0)
#endif

void LocalInit(void)
{
	C1_TRACE("c1x: LocalInit enter\n");
#ifdef USE_TMP
	C_insertpart(loc_id = C_getid());
	C1_TRACE_VALUE("c1x: local part id ", loc_id);
#endif /* USE_TMP */
}

arith LocalSpace(arith sz, int al)
{
	struct stack_level* stl = local_level;

	stl->sl_max_block = -align(sz - stl->sl_max_block, al);
	return stl->sl_max_block;
}

#define TABSIZ 32
static struct localvar* regs[TABSIZ];

arith NewLocal(arith sz, int al, int regtype, int sc)
{
	struct localvar* tmp = FreeTmps;
	struct localvar* prev = 0;
	int index;

	C1_TRACE("c1x: NewLocal\n");
	C1_TRACE_VALUE("c1x:   size ", sz);
	C1_TRACE_VALUE("c1x:   align ", al);
	C1_TRACE_VALUE("c1x:   regtype ", regtype);
	C1_TRACE_VALUE("c1x:   storage ", sc);
	while (tmp)
	{
		if (tmp->t_align >= al && tmp->t_size >= sz && tmp->t_sc == sc && tmp->t_regtype == regtype)
		{
			if (prev)
			{
				prev->next = tmp->next;
			}
			else
				FreeTmps = tmp->next;
			break;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	if (!tmp)
	{
		tmp = new_localvar();
		tmp->t_offset = LocalSpace(sz, al);
		tmp->t_align = al;
		tmp->t_size = sz;
		tmp->t_sc = sc;
		tmp->t_regtype = regtype;
		tmp->t_count = REG_DEFAULT;
	}
	index = (int)(tmp->t_offset >> 2) & (TABSIZ - 1);
	C1_TRACE_VALUE("c1x:   offset ", tmp->t_offset);
	C1_TRACE_VALUE("c1x:   bucket ", index);
	tmp->next = regs[index];
	regs[index] = tmp;
	return tmp->t_offset;
}

void FreeLocal(arith off)
{
	int index = (int)(off >> 2) & (TABSIZ - 1);
	struct localvar* tmp = regs[index];
	struct localvar* prev = 0;

	C1_TRACE_VALUE("c1x: FreeLocal offset ", off);
	C1_TRACE_VALUE("c1x:   bucket ", index);
	while (tmp && tmp->t_offset != off)
	{
		prev = tmp;
		tmp = tmp->next;
	}
	if (tmp)
	{
		C1_TRACE("c1x:   found\n");
		if (prev)
			prev->next = tmp->next;
		else
			regs[index] = tmp->next;
		tmp->next = FreeTmps;
		FreeTmps = tmp;
	}
	else
		C1_TRACE("c1x:   not found\n");
}

void LocalFinish(void)
{
	struct localvar *tmp, *tmp1;
	int i;

#ifdef USE_TMP
	C_beginpart(loc_id);
#endif
	C1_TRACE("c1x: LocalFinish scan free list\n");
	tmp = FreeTmps;
	while (tmp)
	{
		C1_TRACE_VALUE("c1x: free offset ", tmp->t_offset);
		C1_TRACE_VALUE("c1x: free size ", tmp->t_size);
		C1_TRACE_VALUE("c1x: free regtype ", tmp->t_regtype);
		C1_TRACE_VALUE("c1x: free count ", tmp->t_count);
		tmp1 = tmp;
		if (tmp->t_sc == REGISTER)
			tmp->t_count += REG_BONUS;
		if (!options['n'] && tmp->t_regtype >= 0)
		{
			C1_TRACE("c1x: free C_ms_reg enter\n");
			C_ms_reg(tmp->t_offset, tmp->t_size, tmp->t_regtype, tmp->t_count);
			C1_TRACE("c1x: free C_ms_reg returned\n");
		}
		tmp = tmp->next;
		C1_TRACE("c1x: free node release\n");
		free_localvar(tmp1);
	}
	FreeTmps = 0;
	for (i = 0; i < TABSIZ; i++)
	{
		C1_TRACE_VALUE("c1x: bucket ", i);
		tmp = regs[i];
		while (tmp)
		{
			C1_TRACE_VALUE("c1x: reg offset ", tmp->t_offset);
			C1_TRACE_VALUE("c1x: reg size ", tmp->t_size);
			C1_TRACE_VALUE("c1x: reg type ", tmp->t_regtype);
			C1_TRACE_VALUE("c1x: reg count ", tmp->t_count);
			if (tmp->t_sc == REGISTER)
				tmp->t_count += REG_BONUS;
			tmp1 = tmp;
			if (!options['n'] && tmp->t_regtype >= 0)
			{
				C1_TRACE("c1x: reg C_ms_reg enter\n");
				C_ms_reg(tmp->t_offset, tmp->t_size, tmp->t_regtype, tmp->t_count);
				C1_TRACE("c1x: reg C_ms_reg returned\n");
			}
			tmp = tmp->next;
			C1_TRACE("c1x: reg node release\n");
			free_localvar(tmp1);
		}
		regs[i] = 0;
	}
#ifdef PEEPHOLE
	if (!options['n'])
	{
		C1_TRACE("c1x: empty ms_reg enter\n");
		C_mes_begin(ms_reg);
		C_mes_end();
		C1_TRACE("c1x: empty ms_reg returned\n");
	}
#endif
#ifdef USE_TMP
	C1_TRACE("c1x: local C_endpart enter\n");
	C_endpart(loc_id);
	C1_TRACE("c1x: local C_endpart returned\n");
#endif
	C1_TRACE("c1x: LocalFinish complete\n");
}

void RegisterAccount(arith offset, arith size, int regtype, int sc)
{
	struct localvar* p;
	int index;

	if (regtype < 0)
		return;

	C1_TRACE("c1x: RegisterAccount\n");
	C1_TRACE_VALUE("c1x:   offset ", offset);
	C1_TRACE_VALUE("c1x:   size ", size);
	C1_TRACE_VALUE("c1x:   regtype ", regtype);
	C1_TRACE_VALUE("c1x:   storage ", sc);
	p = new_localvar();
	index = (int)(offset >> 2) & (TABSIZ - 1);
	p->t_offset = offset;
	p->t_regtype = regtype;
	p->t_count = REG_DEFAULT;
	p->t_sc = sc;
	p->t_size = size;
	p->next = regs[index];
	regs[index] = p;
}

static struct localvar* find_reg(arith off)
{
	struct localvar* p = regs[(int)(off >> 2) & (TABSIZ - 1)];

	while (p && p->t_offset != off)
		p = p->next;
	return p;
}

void LoadLocal(arith off, arith sz)
{
	struct localvar* p = find_reg(off);

#ifdef USE_TMP
#ifdef REGCOUNT
	if (p)
		p->t_count++;
#endif
#endif
	if (p && p->t_size != sz)
		p->t_regtype = -1;
	if (sz == word_size)
		C_lol(off);
	else if (sz == dword_size)
		C_ldl(off);
	else
	{
		if (p)
			p->t_regtype = -1;
		C_lal(off);
		C_loi(sz);
	}
}

void StoreLocal(arith off, arith sz)
{
	struct localvar* p = find_reg(off);

#ifdef USE_TMP
#ifdef REGCOUNT
	if (p)
		p->t_count++;
#endif
#endif
	if (p && p->t_size != sz)
		p->t_regtype = -1;
	if (sz == word_size)
		C_stl(off);
	else if (sz == dword_size)
		C_sdl(off);
	else
	{
		if (p)
			p->t_regtype = -1;
		C_lal(off);
		C_sti(sz);
	}
}

#ifndef LINT
void AddrLocal(arith off)
{
	struct localvar* p = find_reg(off);

	if (p)
		p->t_regtype = -1;
	C_lal(off);
}
#endif /* LINT */
