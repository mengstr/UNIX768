/* LLgen generated code from source ../../cc/c1/expression.g */
#include "Lpars.h"
#define LL_LEXI LLlex
#define first_of_type_specifier(x) LLfirst((x), 0)
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
# line 8 "../../cc/c1/expression.g"

#include	<alloc.h>
#include	<stdlib.h>
#include	"parameters.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"LLlex.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"code.h"
#include    "error.h"
#include    "ch3.h"
#include    "ch3bin.h"
#include    "ch3mon.h"
#include    "proto.h"
#include	"sizes.h"

int InSizeof = 0;	/* inside a sizeof- expression */
int ResultKnown = 0;	/* result of the expression is already known */

#ifdef ACK_TARGET_C1_TRACE
extern void c1_trace(char*);
extern void c1_trace_value(char*, long);
#define C1_TRACE(s) c1_trace(s)
#define C1_TRACE_VALUE(s, v) c1_trace_value((s), (long)(v))
#else
#define C1_TRACE(s) ((void)0)
#define C1_TRACE_VALUE(s, v) ((void)0)
#endif

/* Since the grammar in the standard is not LL(n), it is modified so that
 * it accepts basically the same grammar. This means that there is no 1-1
 * mapping from the grammar in the standard to the grammar given here.
 * Such is life.
 */
void LL16_identifier(
# line 349 "../../cc/c1/expression.g"
struct idf **idfp) ;
void LL26_cast(
# line 562 "../../cc/c1/declar.g"
struct type **tpp) ;
static void LL35_primary(
# line 37 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL36_constant(
# line 335 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL37_string(
# line 54 "../../cc/c1/expression.g"
struct expr **expp) ;
void LL38_expression(
# line 278 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL39_postfix_expression(
# line 84 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL40_parameter_list(
# line 110 "../../cc/c1/expression.g"
struct expr **expp) ;
void LL41_assignment_expression(
# line 262 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL42_unary(
# line 126 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL43_unop(
# line 290 "../../cc/c1/expression.g"
int *oper) ;
static void LL44_size_of(
# line 149 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL45_binary_expression(
# line 206 "../../cc/c1/expression.g"
int maxrank ,struct expr **expp) ;
static void LL46_binop(
# line 323 "../../cc/c1/expression.g"
int *oper) ;
static void LL47_conditional_expression(
# line 232 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL48_asgnop(
# line 328 "../../cc/c1/expression.g"
int *oper) ;
static void LL49_multop(void);
static void LL50_addop(void);
static void LL51_shiftop(void);
static void LL52_relop(void);
static void LL53_eqop(void);
static void LL54_arithop(void);
static
void
LL35_primary(
# line 37 "../../cc/c1/expression.g"
struct expr **expp)  
{
	C1_TRACE("c1x: primary enter\n");
	C1_TRACE_VALUE("c1x: primary symbol ", LLcsymb);
switch(LLcsymb) {
default:
LL_SSCANDONE(IDENTIFIER);
# line 39 "../../cc/c1/expression.g"
{C1_TRACE("c1x: primary dot2expr enter\n"); dot2expr(expp); C1_TRACE("c1x: primary dot2expr returned\n");}
LLread();
break;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
LL36_constant(
# line 41 "../../cc/c1/expression.g"
expp);
LLread();
break;
case /*  STRING  */ 4 : ;
LL37_string(
# line 43 "../../cc/c1/expression.g"
expp);
break;
case /* '(' */ 89 : ;
LLtincr(90);
LL_SAFE('(');
LLread();
LL38_expression(
# line 45 "../../cc/c1/expression.g"
expp);
LLtdecr(90);
LL_SCANDONE(')');
# line 46 "../../cc/c1/expression.g"
{ (*expp)->ex_flags |= EX_PARENS; }
LLread();
break;
}
}
static
void
LL37_string(
# line 54 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 55 "../../cc/c1/expression.g"
	int i, len;
		char *str;
		int fund;
	
LLtincr(4);
LL_SAFE(STRING);
# line 61 "../../cc/c1/expression.g"
{	str = dot.tk_bts;
		len = dot.tk_len;
		fund = dot.tk_fund;
	}
LLread();
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  EOFILE  */ 0 : ;
case /*  PLUSAB  */ 9 : ;
case /*  MINAB  */ 10 : ;
case /*  TIMESAB  */ 11 : ;
case /*  DIVAB  */ 12 : ;
case /*  MODAB  */ 13 : ;
case /*  LEFTAB  */ 14 : ;
case /*  RIGHTAB  */ 15 : ;
case /*  ANDAB  */ 16 : ;
case /*  XORAB  */ 17 : ;
case /*  ORAB  */ 18 : ;
case /*  NOTEQUAL  */ 19 : ;
case /*  AND  */ 20 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  ARROW  */ 23 : ;
case /*  LEFT  */ 24 : ;
case /*  LESSEQ  */ 25 : ;
case /*  EQUAL  */ 26 : ;
case /*  GREATEREQ  */ 27 : ;
case /*  RIGHT  */ 28 : ;
case /*  OR  */ 29 : ;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
case /* ']' */ 92 : ;
case /* '}' */ 94 : ;
case /* ':' */ 95 : ;
case /* '*' */ 96 : ;
case /* '.' */ 97 : ;
case /* '?' */ 98 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '/' */ 104 : ;
case /* '%' */ 105 : ;
case /* '<' */ 106 : ;
case /* '>' */ 107 : ;
case /* '^' */ 108 : ;
case /* '|' */ 109 : ;
break;
default:{int LL_17=LLnext(260);
;if (!LL_17) {
break;
}
else if (LL_17 & 1) goto L_1;}
case /*  STRING  */ 4 : ;
LL_SAFE(STRING);
# line 67 "../../cc/c1/expression.g"
{	/* A pasted string keeps the type of the first
			 * string literal.
			 * The pasting of normal strings and wide
			 * character strings are stated as having an
			 * undefined behaviour.
			 */
			if (dot.tk_fund != fund)
				warning("illegal pasting of string literals");
			str = realloc(str, (unsigned) (--len + dot.tk_len));
			for (i = 0; i < dot.tk_len; i++)
				str[len++] = dot.tk_bts[i];
		}
LLread();
continue;
}
}
LLtdecr(4);
break;
}
# line 80 "../../cc/c1/expression.g"
{ string2expr(expp, str, len); }
}
static
void
LL39_postfix_expression(
# line 84 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 85 "../../cc/c1/expression.g"
 int oper; 
	  struct expr *e1 = 0;
	  struct idf *idf;
	
	C1_TRACE("c1x: postfix enter\n");
LLsincr(13);
LL35_primary(
# line 90 "../../cc/c1/expression.g"
expp);
C1_TRACE("c1x: postfix primary returned\n");
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  EOFILE  */ 0 : ;
case /*  PLUSAB  */ 9 : ;
case /*  MINAB  */ 10 : ;
case /*  TIMESAB  */ 11 : ;
case /*  DIVAB  */ 12 : ;
case /*  MODAB  */ 13 : ;
case /*  LEFTAB  */ 14 : ;
case /*  RIGHTAB  */ 15 : ;
case /*  ANDAB  */ 16 : ;
case /*  XORAB  */ 17 : ;
case /*  ORAB  */ 18 : ;
case /*  NOTEQUAL  */ 19 : ;
case /*  AND  */ 20 : ;
case /*  LEFT  */ 24 : ;
case /*  LESSEQ  */ 25 : ;
case /*  EQUAL  */ 26 : ;
case /*  GREATEREQ  */ 27 : ;
case /*  RIGHT  */ 28 : ;
case /*  OR  */ 29 : ;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
case /* ')' */ 90 : ;
case /* ']' */ 92 : ;
case /* '}' */ 94 : ;
case /* ':' */ 95 : ;
case /* '*' */ 96 : ;
case /* '?' */ 98 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '/' */ 104 : ;
case /* '%' */ 105 : ;
case /* '<' */ 106 : ;
case /* '>' */ 107 : ;
case /* '^' */ 108 : ;
case /* '|' */ 109 : ;
break;
default:{int LL_18=LLnext(-182);
;if (!LL_18) {
break;
}
else if (LL_18 & 1) goto L_1;}
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  ARROW  */ 23 : ;
case /* '(' */ 89 : ;
C1_TRACE("c1x: postfix call enter\n");
case /* '[' */ 91 : ;
case /* '.' */ 97 : ;
switch(LLcsymb) {
case /* '[' */ 91 : ;
LLtincr(92);
LL_SAFE('[');
LLread();
LL38_expression(
# line 92 "../../cc/c1/expression.g"
&e1);
C1_TRACE("c1x: postfix parameter list returned\n");
LLtdecr(92);
LL_SCANDONE(']');
# line 93 "../../cc/c1/expression.g"
{ ch3bin(expp, '[', e1); e1 = 0; }
break;
case /* '(' */ 89 : ;
LLsincr(4);
LLtincr(90);
LL_SAFE('(');
LLread();
goto L_5;
L_5 : {switch(LLcsymb) {
case /* ')' */ 90 : ;
LLsdecr(4);
break;
default:{int LL_19=LLnext(-56);
;if (!LL_19) {
LLsdecr(4);
break;
}
else if (LL_19 & 1) goto L_5;}
case /*  IDENTIFIER  */ 2 : ;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  SIZEOF  */ 49 : ;
case /* '(' */ 89 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
LLsdecr(4);
LL40_parameter_list(
# line 95 "../../cc/c1/expression.g"
&e1);
}
}
LLtdecr(90);
LL_SCANDONE(')');
# line 96 "../../cc/c1/expression.g"
{ C1_TRACE("c1x: postfix call ch3bin enter\n"); ch3bin(expp, '(', e1); C1_TRACE("c1x: postfix call ch3bin returned\n"); call_proto(expp); C1_TRACE("c1x: postfix call prototype returned\n"); e1 = 0; }
break;
case /*  ARROW  */ 23 : ;
case /* '.' */ 97 : ;
LLsincr(14);
# line 98 "../../cc/c1/expression.g"
{ oper = DOT; }
LLread();
LLsdecr(14);
LL16_identifier(
# line 99 "../../cc/c1/expression.g"
&idf);
# line 99 "../../cc/c1/expression.g"
{ ch3sel(expp, oper, idf); }
break;
default:
switch(LLcsymb) {
default:
LL_SAFE(PLUSPLUS);
# line 102 "../../cc/c1/expression.g"
{ oper = POSTINCR; }
break;
case /*  MINMIN  */ 22 : ;
LL_SAFE(MINMIN);
# line 104 "../../cc/c1/expression.g"
{ oper = POSTDECR; }
break;
}
# line 106 "../../cc/c1/expression.g"
{ ch3incr(expp, oper); }
break;
}
LLread();
continue;
}
}
LLsdecr(13);
break;
}
}
static
void
LL40_parameter_list(
# line 110 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 111 "../../cc/c1/expression.g"
struct expr *e1 = 0;
LLsincr(15);
LL41_assignment_expression(
# line 113 "../../cc/c1/expression.g"
expp);
# line 114 "../../cc/c1/expression.g"
{any2opnd(expp, PARCOMMA);}
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ')' */ 90 : ;
break;
default:{int LL_20=LLnext(-210);
;if (!LL_20) {
break;
}
else if (LL_20 & 1) goto L_1;}
case /*  IDENTIFIER  */ 2 : ;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /*  PLUSAB  */ 9 : ;
case /*  MINAB  */ 10 : ;
case /*  TIMESAB  */ 11 : ;
case /*  DIVAB  */ 12 : ;
case /*  MODAB  */ 13 : ;
case /*  LEFTAB  */ 14 : ;
case /*  RIGHTAB  */ 15 : ;
case /*  ANDAB  */ 16 : ;
case /*  XORAB  */ 17 : ;
case /*  ORAB  */ 18 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  SIZEOF  */ 49 : ;
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
case /* '(' */ 89 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
LL_SSCANDONE(',');
LLread();
LL41_assignment_expression(
# line 117 "../../cc/c1/expression.g"
&e1);
# line 118 "../../cc/c1/expression.g"
{any2opnd(&e1, PARCOMMA);}
# line 119 "../../cc/c1/expression.g"
{ch3bin(expp, PARCOMMA, e1);}
continue;
}
}
LLsdecr(15);
break;
}
}
static
void
LL42_unary(
# line 126 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 127 "../../cc/c1/expression.g"
struct type *tp; int oper;
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '(' */ 89 : ;
# line 129 "../../cc/c1/expression.g"
if (!(first_of_type_specifier(AHEAD) && AHEAD != IDENTIFIER)) goto L_1;
LL26_cast(
# line 130 "../../cc/c1/expression.g"
&tp);
LLread();
LL42_unary(
# line 130 "../../cc/c1/expression.g"
expp);
# line 131 "../../cc/c1/expression.g"
{	ch3cast(expp, CAST, tp);
		(*expp)->ex_flags |= EX_CAST;
		if (int_size != pointer_size)
			(*expp)->ex_flags &= ~EX_PTRDIFF;
	}
break;
goto L_1;
L_1 : ;
default:
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /* '(' */ 89 : ;
goto L_6;
L_6: ;
LLsdecr(16);
LL39_postfix_expression(
# line 137 "../../cc/c1/expression.g"
expp);
break;
default: if (LLskip()) goto L_2;
goto L_6;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
LL43_unop(
# line 139 "../../cc/c1/expression.g"
&oper);
LLread();
LL42_unary(
# line 139 "../../cc/c1/expression.g"
expp);
# line 140 "../../cc/c1/expression.g"
{ch3mon(oper, expp);}
break;
case /*  SIZEOF  */ 49 : ;
LLsdecr(16);
LL44_size_of(
# line 142 "../../cc/c1/expression.g"
expp);
break;
}
}
}
static
void
LL44_size_of(
# line 149 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 150 "../../cc/c1/expression.g"
struct type *tp;
LLsincr(16);
LL_SAFE(SIZEOF);
# line 152 "../../cc/c1/expression.g"
{ InSizeof++; }
LLread();
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '(' */ 89 : ;
# line 153 "../../cc/c1/expression.g"
if (!(first_of_type_specifier(AHEAD) && AHEAD != IDENTIFIER)) goto L_1;
LLsdecr(16);
LL26_cast(
# line 154 "../../cc/c1/expression.g"
&tp);
# line 155 "../../cc/c1/expression.g"
{
			*expp = intexpr(size_of_type(tp, "type"), UNSIGNED);
			(*expp)->ex_flags |= EX_SIZEOF;
		}
LLread();
break;
goto L_1;
L_1 : ;
case /*  IDENTIFIER  */ 2 : ;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  SIZEOF  */ 49 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
goto L_3;
L_3: ;
LL42_unary(
# line 160 "../../cc/c1/expression.g"
expp);
# line 161 "../../cc/c1/expression.g"
{ch3mon(SIZEOF, expp);}
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
# line 163 "../../cc/c1/expression.g"
{ InSizeof--; }
}
static
void
LL45_binary_expression(
# line 206 "../../cc/c1/expression.g"
int maxrank ,struct expr **expp)  
{
# line 207 "../../cc/c1/expression.g"
int oper, OldResultKnown; struct expr *e1;
LLsincr(16);
LLsincr(17);
LL42_unary(
# line 209 "../../cc/c1/expression.g"
expp);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  NOTEQUAL  */ 19 : ;
case /*  AND  */ 20 : ;
case /*  LEFT  */ 24 : ;
case /*  LESSEQ  */ 25 : ;
case /*  EQUAL  */ 26 : ;
case /*  GREATEREQ  */ 27 : ;
case /*  RIGHT  */ 28 : ;
case /*  OR  */ 29 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '/' */ 104 : ;
case /* '%' */ 105 : ;
case /* '<' */ 106 : ;
case /* '>' */ 107 : ;
case /* '^' */ 108 : ;
case /* '|' */ 109 : ;
# line 210 "../../cc/c1/expression.g"
if ((rank_of(DOT) <= maxrank )) goto L_2;
case /*  EOFILE  */ 0 : ;
case /*  PLUSAB  */ 9 : ;
case /*  MINAB  */ 10 : ;
case /*  TIMESAB  */ 11 : ;
case /*  DIVAB  */ 12 : ;
case /*  MODAB  */ 13 : ;
case /*  LEFTAB  */ 14 : ;
case /*  RIGHTAB  */ 15 : ;
case /*  ANDAB  */ 16 : ;
case /*  XORAB  */ 17 : ;
case /*  ORAB  */ 18 : ;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
case /* ')' */ 90 : ;
case /* ']' */ 92 : ;
case /* '}' */ 94 : ;
case /* ':' */ 95 : ;
case /* '?' */ 98 : ;
break;
default:{int LL_21=LLnext(-238);
;if (!LL_21) {
break;
}
else if (LL_21 & 1) goto L_1;}
goto L_2;
L_2 : ;
LLsincr(18);
LL46_binop(
# line 213 "../../cc/c1/expression.g"
&oper);
# line 214 "../../cc/c1/expression.g"
{ OldResultKnown = ResultKnown;
		  if (oper == OR || oper == AND) {
			  if (is_cp_cst(*expp) || is_fp_cst(*expp)) {
				  if (is_zero_cst(*expp)) {
					  if (oper == AND) ResultKnown++;
				  } else if (oper == OR) ResultKnown++;
			  }
		  }
		}
LLread();
LLsdecr(18);
LL45_binary_expression(
# line 223 "../../cc/c1/expression.g"
rank_of(oper)-1, &e1);
# line 224 "../../cc/c1/expression.g"
{
			ch3bin(expp, oper, e1);
			ResultKnown = OldResultKnown;
		}
continue;
}
}
LLsdecr(17);
break;
}
}
static
void
LL47_conditional_expression(
# line 232 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 233 "../../cc/c1/expression.g"
struct expr *e1 = 0, *e2 = 0; int OldResultKnown, ConstExpr=0;
LLtincr(98);
LL45_binary_expression(
# line 236 "../../cc/c1/expression.g"
rank_of('?') - 1, expp);
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  EOFILE  */ 0 : ;
case /*  PLUSAB  */ 9 : ;
case /*  MINAB  */ 10 : ;
case /*  TIMESAB  */ 11 : ;
case /*  DIVAB  */ 12 : ;
case /*  MODAB  */ 13 : ;
case /*  LEFTAB  */ 14 : ;
case /*  RIGHTAB  */ 15 : ;
case /*  ANDAB  */ 16 : ;
case /*  XORAB  */ 17 : ;
case /*  ORAB  */ 18 : ;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
case /* ')' */ 90 : ;
case /* ']' */ 92 : ;
case /* '}' */ 94 : ;
case /* ':' */ 95 : ;
LLtdecr(98);
break;
default:{int LL_22=LLnext(63);
;if (!LL_22) {
LLtdecr(98);
break;
}
else if (LL_22 & 1) goto L_1;}
case /* '?' */ 98 : ;
LLtdecr(98);
LLtincr(95);
LLsincr(19);
LL_SAFE('?');
# line 238 "../../cc/c1/expression.g"
{ OldResultKnown = ResultKnown;
		  if (is_cp_cst(*expp) || is_fp_cst(*expp)) {
			  ConstExpr++;
			  if (is_zero_cst(*expp)) ResultKnown++;
		  }
		}
LLread();
LL38_expression(
# line 244 "../../cc/c1/expression.g"
&e1);
LLtdecr(95);
LL_SCANDONE(':');
# line 246 "../../cc/c1/expression.g"
{ if (ConstExpr) {
			if (OldResultKnown == ResultKnown) ResultKnown++;
			else ResultKnown = OldResultKnown;
		  }
		}
LLread();
LLsdecr(19);
LL47_conditional_expression(
# line 251 "../../cc/c1/expression.g"
&e2);
# line 252 "../../cc/c1/expression.g"
{	
			ResultKnown = OldResultKnown;
			ch3bin(&e1, ':', e2);
			opnd2test(expp, '?');
			ch3bin(expp, '?', e1);
		}
}
}
}
void
LL41_assignment_expression(
# line 262 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 263 "../../cc/c1/expression.g"
 int oper;
	  struct expr *e1 = 0;
	
LLsincr(20);
LL47_conditional_expression(
# line 267 "../../cc/c1/expression.g"
expp);
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  PLUSAB  */ 9 : ;
case /*  MINAB  */ 10 : ;
case /*  TIMESAB  */ 11 : ;
case /*  DIVAB  */ 12 : ;
case /*  MODAB  */ 13 : ;
case /*  LEFTAB  */ 14 : ;
case /*  RIGHTAB  */ 15 : ;
case /*  ANDAB  */ 16 : ;
case /*  XORAB  */ 17 : ;
case /*  ORAB  */ 18 : ;
case /* '=' */ 88 : ;
LLsdecr(20);
LLsincr(21);
LL48_asgnop(
# line 269 "../../cc/c1/expression.g"
&oper);
LLread();
LLsdecr(21);
LL41_assignment_expression(
# line 270 "../../cc/c1/expression.g"
&e1);
# line 271 "../../cc/c1/expression.g"
{ch3asgn(expp, oper, e1);}
break;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* ')' */ 90 : ;
case /* ']' */ 92 : ;
case /* '}' */ 94 : ;
case /* ':' */ 95 : ;
goto L_3;
L_3: ;
LLsdecr(20);
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
}
void
LL38_expression(
# line 278 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 279 "../../cc/c1/expression.g"
struct expr *e1;
LLtincr(87);
LL41_assignment_expression(
# line 281 "../../cc/c1/expression.g"
expp);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
case /* ')' */ 90 : ;
case /* ']' */ 92 : ;
case /* ':' */ 95 : ;
break;
default:{int LL_23=LLnext(44);
;if (!LL_23) {
break;
}
else if (LL_23 & 1) goto L_1;}
case /* ',' */ 87 : ;
LL_SAFE(',');
LLread();
LL41_assignment_expression(
# line 283 "../../cc/c1/expression.g"
&e1);
# line 284 "../../cc/c1/expression.g"
{
			ch3bin(expp, ',', e1);
		}
continue;
}
}
LLtdecr(87);
break;
}
}
static
void
LL43_unop(
# line 290 "../../cc/c1/expression.g"
int *oper)  
{
# line 292 "../../cc/c1/expression.g"
{   if (DOT == '&') DOT = ADDRESSOF;
	    *oper = DOT;
	}
}
static
void
LL49_multop(
void
) {
}
static
void
LL50_addop(
void
) {
}
static
void
LL51_shiftop(
void
) {
}
static
void
LL52_relop(
void
) {
}
static
void
LL53_eqop(
void
) {
}
static
void
LL54_arithop(
void
) {
switch(LLcsymb) {
default:
LL49_multop();
break;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
LL50_addop();
break;
case /*  LEFT  */ 24 : ;
case /*  RIGHT  */ 28 : ;
LL51_shiftop();
break;
case /* '&' */ 99 : ;
LL_SAFE('&');
break;
case /* '^' */ 108 : ;
LL_SAFE('^');
break;
case /* '|' */ 109 : ;
LL_SAFE('|');
break;
}
}
static
void
LL46_binop(
# line 323 "../../cc/c1/expression.g"
int *oper)  
{
switch(LLcsymb) {
case /*  LEFT  */ 24 : ;
case /*  RIGHT  */ 28 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '/' */ 104 : ;
case /* '%' */ 105 : ;
case /* '^' */ 108 : ;
case /* '|' */ 109 : ;
LL54_arithop();
break;
default:
LL52_relop();
break;
case /*  NOTEQUAL  */ 19 : ;
case /*  EQUAL  */ 26 : ;
LL53_eqop();
break;
case /*  AND  */ 20 : ;
LL_SAFE(AND);
break;
case /*  OR  */ 29 : ;
LL_SAFE(OR);
break;
}
# line 325 "../../cc/c1/expression.g"
{*oper = DOT;}
}
static
void
LL48_asgnop(
# line 328 "../../cc/c1/expression.g"
int *oper)  
{
# line 331 "../../cc/c1/expression.g"
{ *oper = DOT; }
}
static
void
LL36_constant(
# line 335 "../../cc/c1/expression.g"
struct expr **expp)  
{
# line 340 "../../cc/c1/expression.g"
{dot2expr(expp);}
}
void
LL17_constant_expression(
# line 344 "../../cc/c1/expression.g"
struct expr **expp)  
{
LL47_conditional_expression(
# line 345 "../../cc/c1/expression.g"
expp);
# line 346 "../../cc/c1/expression.g"
{ chk_cst_expr(expp); }
}
void
LL16_identifier(
# line 349 "../../cc/c1/expression.g"
struct idf **idfp)  
{
LLsincr(14);
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
goto L_3;
L_3: ;
LLsdecr(14);
LL_SSCANDONE(IDENTIFIER);
break;
default: if (LLskip()) goto L_2;
goto L_3;
case /*  TYPE_IDENTIFIER  */ 3 : ;
LLsdecr(14);
LL_SAFE(TYPE_IDENTIFIER);
break;
}
# line 353 "../../cc/c1/expression.g"
{ *idfp = dot.tk_idf; }
}

