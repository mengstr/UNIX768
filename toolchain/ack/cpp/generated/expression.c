/* LLgen generated code from source lang/cem/cpp.ansi/expression.g */
#include "Lpars.h"
#define LL_LEXI LLlex
#define LLNOFIRSTS
#define LL_LEXI LLlex
/* $Id$ */
#ifdef LL_DEBUG
#include <assert.h>
#define LL_assert(x)	assert(x)
#else
#define LL_assert(x)	/* nothing */
#endif

extern int LLsymb;

#define LL_SAFE(x)	/* Nothing */
#define LL_SSCANDONE(x)	if (LLsymb != x) LLsafeerror(x)
#define LL_SCANDONE(x)	if (LLsymb != x) LLerror(x)
#define LL_NOSCANDONE(x) LLscan(x)
#ifdef LL_FASTER
#define LLscan(x)	if ((LLsymb = LL_LEXI()) != x) LLerror(x)
#endif

extern unsigned int LLscnt[];
extern unsigned int LLtcnt[];
extern int LLcsymb;

#if LL_NON_CORR
extern int LLstartsymb;
#endif

#define LLsdecr(d)	{LL_assert(LLscnt[d] > 0); LLscnt[d]--;}
#define LLtdecr(d)	{LL_assert(LLtcnt[d] > 0); LLtcnt[d]--;}
#define LLsincr(d)	LLscnt[d]++
#define LLtincr(d)	LLtcnt[d]++

extern int LL_LEXI(void);
extern void LLread(void);
extern int LLskip(void);
extern int LLnext(int);
extern void LLerror(int);
extern void LLsafeerror(int);
extern void LLnewlevel(unsigned int *);
extern void LLoldlevel(unsigned int *);
#ifndef LL_FASTER
extern void LLscan(int);
#endif
#ifndef LLNOFIRSTS
extern int LLfirst(int, int);
#endif
#if LL_NON_CORR
extern void LLnc_recover(void);
#endif
# line 10 "lang/cem/cpp.ansi/expression.g"

#include	"arith.h"
#include	"LLlex.h"
#include	"ch3mon.h"
#include	"ch3bin.h"
#include	"expr.h"

extern arith ifval;
static void LL1_constant_expression(
# line 137 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL2_primary(
# line 27 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL3_constant(
# line 130 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL4_expression(
# line 80 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL5_unary(
# line 34 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL6_unop(
# line 94 "lang/cem/cpp.ansi/expression.g"
int *oper) ;
static void LL7_binary_expression(
# line 44 "lang/cem/cpp.ansi/expression.g"
int maxrank ,arith *pval ,int *is_uns) ;
static void LL8_binop(
# line 125 "lang/cem/cpp.ansi/expression.g"
int *oper) ;
static void LL9_conditional_expression(
# line 58 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL10_assignment_expression(
# line 74 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns) ;
static void LL11_multop(void);
static void LL12_addop(void);
static void LL13_shiftop(void);
static void LL14_relop(void);
static void LL15_eqop(void);
static void LL16_arithop(void);
void
LL0_if_expression(
void
) {
# line 21 "lang/cem/cpp.ansi/expression.g"
 int is_unsigned = 0; 
LL1_constant_expression(
# line 23 "lang/cem/cpp.ansi/expression.g"
&ifval, &is_unsigned);
}
static
void
LL2_primary(
# line 27 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
switch(LLcsymb) {
default:
LL3_constant(
# line 29 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
break;
case /* '(' */ 29 : ;
LLtincr(30);
LL_SAFE('(');
LL4_expression(
# line 31 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
LLtdecr(30);
LL_SCANDONE(')');
break;
}
}
static
void
LL5_unary(
# line 34 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
# line 35 "lang/cem/cpp.ansi/expression.g"
int oper;
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '-' */ 34 : ;
case /* '!' */ 35 : ;
case /* '~' */ 36 : ;
LL6_unop(
# line 37 "lang/cem/cpp.ansi/expression.g"
&oper);
LLread();
LL5_unary(
# line 38 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
# line 39 "lang/cem/cpp.ansi/expression.g"
{ ch3mon(oper, pval, is_uns); }
break;
case /*  INTEGER  */ 5 : ;
case /* '(' */ 29 : ;
goto L_3;
L_3: ;
LLsdecr(0);
LL2_primary(
# line 41 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
LLread();
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
}
static
void
LL7_binary_expression(
# line 44 "lang/cem/cpp.ansi/expression.g"
int maxrank ,arith *pval ,int *is_uns)  
{
# line 45 "lang/cem/cpp.ansi/expression.g"
int oper; arith val1; int u;
LLsincr(0);
LLsincr(1);
LL5_unary(
# line 47 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  NOTEQUAL  */ 16 : ;
case /*  AND  */ 17 : ;
case /*  LEFT  */ 21 : ;
case /*  LESSEQ  */ 22 : ;
case /*  EQUAL  */ 23 : ;
case /*  GREATEREQ  */ 24 : ;
case /*  RIGHT  */ 25 : ;
case /*  OR  */ 26 : ;
case /* '-' */ 34 : ;
case /* '*' */ 37 : ;
case /* '/' */ 38 : ;
case /* '%' */ 39 : ;
case /* '+' */ 40 : ;
case /* '<' */ 41 : ;
case /* '>' */ 42 : ;
case /* '&' */ 43 : ;
case /* '^' */ 44 : ;
case /* '|' */ 45 : ;
# line 48 "lang/cem/cpp.ansi/expression.g"
if ((rank_of(DOT) <= maxrank)) goto L_2;
case /*  EOFILE  */ 0 : ;
case /* ')' */ 30 : ;
case /* '?' */ 31 : ;
case /* ':' */ 32 : ;
case /* ',' */ 33 : ;
break;
default:{int LL_1=LLnext(-6);
;if (!LL_1) {
break;
}
else if (LL_1 & 1) goto L_1;}
goto L_2;
L_2 : ;
LLsincr(2);
LL8_binop(
# line 49 "lang/cem/cpp.ansi/expression.g"
&oper);
LLread();
LLsdecr(2);
LL7_binary_expression(
# line 50 "lang/cem/cpp.ansi/expression.g"
rank_of(oper)-1, &val1, &u);
# line 51 "lang/cem/cpp.ansi/expression.g"
{
			ch3bin(pval, is_uns, oper, val1, u);
		}
continue;
}
}
LLsdecr(1);
break;
}
}
static
void
LL9_conditional_expression(
# line 58 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
# line 59 "lang/cem/cpp.ansi/expression.g"
arith val1 = 0, val2 = 0; int u;
LLtincr(31);
LL7_binary_expression(
# line 62 "lang/cem/cpp.ansi/expression.g"
rank_of('?') - 1, pval, is_uns);
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  EOFILE  */ 0 : ;
case /* ')' */ 30 : ;
case /* ':' */ 32 : ;
case /* ',' */ 33 : ;
LLtdecr(31);
break;
default:{int LL_2=LLnext(63);
;if (!LL_2) {
LLtdecr(31);
break;
}
else if (LL_2 & 1) goto L_1;}
case /* '?' */ 31 : ;
LLtdecr(31);
LLtincr(32);
LLsincr(3);
LL_SAFE('?');
LL4_expression(
# line 64 "lang/cem/cpp.ansi/expression.g"
&val1, is_uns);
LLtdecr(32);
LL_SCANDONE(':');
LLread();
LLsdecr(3);
LL10_assignment_expression(
# line 66 "lang/cem/cpp.ansi/expression.g"
&val2, &u);
# line 67 "lang/cem/cpp.ansi/expression.g"
{ if (*pval) *pval = val1;
		  else { *pval = val2; *is_uns = u; }
		}
}
}
}
static
void
LL10_assignment_expression(
# line 74 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
LL9_conditional_expression(
# line 76 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
}
static
void
LL4_expression(
# line 80 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
# line 81 "lang/cem/cpp.ansi/expression.g"
arith val1;
	 int is_uns1;
	
LLtincr(33);
LLread();
LL10_assignment_expression(
# line 85 "lang/cem/cpp.ansi/expression.g"
pval,is_uns);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ')' */ 30 : ;
case /* ':' */ 32 : ;
break;
default:{int LL_3=LLnext(44);
;if (!LL_3) {
break;
}
else if (LL_3 & 1) goto L_1;}
case /* ',' */ 33 : ;
LL_SAFE(',');
LLread();
LL10_assignment_expression(
# line 87 "lang/cem/cpp.ansi/expression.g"
&val1, &is_uns1);
# line 88 "lang/cem/cpp.ansi/expression.g"
{
			ch3bin(pval, is_uns, ',', val1, is_uns1);
		}
continue;
}
}
LLtdecr(33);
break;
}
}
static
void
LL6_unop(
# line 94 "lang/cem/cpp.ansi/expression.g"
int *oper)  
{
# line 96 "lang/cem/cpp.ansi/expression.g"
{*oper = DOT;}
}
static
void
LL11_multop(
void
) {
}
static
void
LL12_addop(
void
) {
}
static
void
LL13_shiftop(
void
) {
}
static
void
LL14_relop(
void
) {
}
static
void
LL15_eqop(
void
) {
}
static
void
LL16_arithop(
void
) {
switch(LLcsymb) {
default:
LL11_multop();
break;
case /* '-' */ 34 : ;
case /* '+' */ 40 : ;
LL12_addop();
break;
case /*  LEFT  */ 21 : ;
case /*  RIGHT  */ 25 : ;
LL13_shiftop();
break;
case /* '&' */ 43 : ;
LL_SAFE('&');
break;
case /* '^' */ 44 : ;
LL_SAFE('^');
break;
case /* '|' */ 45 : ;
LL_SAFE('|');
break;
}
}
static
void
LL8_binop(
# line 125 "lang/cem/cpp.ansi/expression.g"
int *oper)  
{
switch(LLcsymb) {
case /*  LEFT  */ 21 : ;
case /*  RIGHT  */ 25 : ;
case /* '-' */ 34 : ;
case /* '*' */ 37 : ;
case /* '/' */ 38 : ;
case /* '%' */ 39 : ;
case /* '+' */ 40 : ;
case /* '&' */ 43 : ;
case /* '^' */ 44 : ;
case /* '|' */ 45 : ;
LL16_arithop();
break;
default:
LL14_relop();
break;
case /*  NOTEQUAL  */ 16 : ;
case /*  EQUAL  */ 23 : ;
LL15_eqop();
break;
case /*  AND  */ 17 : ;
LL_SAFE(AND);
break;
case /*  OR  */ 26 : ;
LL_SAFE(OR);
break;
}
# line 127 "lang/cem/cpp.ansi/expression.g"
{*oper = DOT;}
}
static
void
LL3_constant(
# line 130 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
LL_SSCANDONE(INTEGER);
# line 132 "lang/cem/cpp.ansi/expression.g"
{*pval = dot.tk_val;
	 *is_uns = dot.tk_unsigned;
	}
}
static
void
LL1_constant_expression(
# line 137 "lang/cem/cpp.ansi/expression.g"
arith *pval ,int *is_uns)  
{
LL10_assignment_expression(
# line 138 "lang/cem/cpp.ansi/expression.g"
pval, is_uns);
}


