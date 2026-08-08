/* LLgen generated code from source ../../cc/c1/statement.g */
#include "Lpars.h"
#define LL_LEXI LLlex
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
# line 8 "../../cc/c1/statement.g"

#include	"parameters.h"
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#include	"l_lint.h"
#endif	/* LINT */

#include	<flt_arith.h>
#include    "idf.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"type.h"
#include	"label.h"
#include	"expr.h"
#include	"code.h"
#include	"stack.h"
#include	"def.h"
#include    "switch.h"
#ifdef DBSYMTAB
#include	<stb.h>
#endif /* DBSYMTAB */

extern int level;
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
void LL0_declaration(void);
void LL16_identifier(
# line 349 "../../cc/c1/expression.g"
struct idf **idfp) ;
void LL17_constant_expression(
# line 344 "../../cc/c1/expression.g"
struct expr **expp) ;
void LL38_expression(
# line 278 "../../cc/c1/expression.g"
struct expr **expp) ;
void LL62_compound_statement(void);
static void LL63_statement(void);
static void LL64_expression_statement(void);
static void LL65_label(void);
static void LL66_if_statement(void);
static void LL67_while_statement(void);
static void LL68_do_statement(void);
static void LL69_for_statement(void);
static void LL70_switch_statement(void);
static void LL71_case_statement(void);
static void LL72_default_statement(void);
static void LL73_return_statement(void);
static void LL74_jump(void);
static
void
LL63_statement(
void
) {
# line 43 "../../cc/c1/statement.g"

	C1_TRACE("c1x: statement enter\n");
	C1_TRACE_VALUE("c1x: statement symbol ", LLcsymb);
#ifdef	LINT
		lint_statement();
#endif	/* LINT */
	
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
# line 49 "../../cc/c1/statement.g"
if (!(AHEAD != ':')) goto L_1;
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
LLsdecr(27);
LL64_expression_statement();
LLread();
break;
goto L_1;
L_1 : ;
default:
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
LLtincr(95);
LL65_label();
LLtdecr(95);
LL_NOSCANDONE(':');
LLread();
LL63_statement();
break;
case /* '{' */ 93 : ;
LLsdecr(27);
LL62_compound_statement();
LLread();
break;
case /*  IF  */ 43 : ;
LLsdecr(27);
LL66_if_statement();
break;
case /*  WHILE  */ 57 : ;
LLsdecr(27);
LL67_while_statement();
break;
case /*  DO  */ 37 : ;
LLsdecr(27);
LL68_do_statement();
LLread();
break;
case /*  FOR  */ 41 : ;
LLsdecr(27);
LL69_for_statement();
break;
case /*  SWITCH  */ 52 : ;
LLsdecr(27);
LL70_switch_statement();
break;
case /*  CASE  */ 33 : ;
LLsdecr(27);
LL71_case_statement();
break;
case /*  DEFAULT  */ 36 : ;
LLsdecr(27);
LL72_default_statement();
break;
case /*  BREAK  */ 32 : ;
LLsdecr(27);
LL_SAFE(BREAK);
# line 71 "../../cc/c1/statement.g"
{
		code_break();
#ifdef	LINT
		lint_break_stmt();
#endif	/* LINT */
	}
LL_NOSCANDONE(';');
LLread();
break;
case /*  CONTINUE  */ 35 : ;
LLsdecr(27);
LL_SAFE(CONTINUE);
# line 80 "../../cc/c1/statement.g"
{
		code_continue();
#ifdef	LINT
		lint_continue_stmt();
#endif	/* LINT */
	}
LL_NOSCANDONE(';');
LLread();
break;
case /*  RETURN  */ 46 : ;
LLsdecr(27);
LL73_return_statement();
LLread();
break;
case /*  GOTO  */ 42 : ;
LLsdecr(27);
LL74_jump();
LLread();
break;
case /* ';' */ 86 : ;
goto L_6;
L_6: ;
LLsdecr(27);
LL_SSCANDONE(';');
LLread();
break;
default: if (LLskip()) goto L_2;
goto L_6;
}
}
}
static
void
LL64_expression_statement(
void
) {
# line 97 "../../cc/c1/statement.g"
	struct expr *expr;
	
LLtincr(86);
LL38_expression(
# line 100 "../../cc/c1/statement.g"
&expr);
LLtdecr(86);
LL_SCANDONE(';');
# line 102 "../../cc/c1/statement.g"
{
#ifdef	DEBUG
			print_expr("expression_statement", expr);
#endif	/* DEBUG */
			code_expr(expr, RVAL, FALSE, NO_LABEL, NO_LABEL);
			free_expression(expr);
		}
}
static
void
LL65_label(
void
) {
# line 113 "../../cc/c1/statement.g"
 struct idf *idf; 
LL16_identifier(
# line 115 "../../cc/c1/statement.g"
&idf);
# line 116 "../../cc/c1/statement.g"
{
		/*	This allows the following absurd case:

				typedef int grz;
				main()	{
					grz: printf("A labelled statement\n");
				}
		*/
#ifdef	LINT
		lint_label();
#endif	/* LINT */
		define_label(idf);
		C_df_ilb((label)idf->id_label->df_address);
	}
}
static
void
LL66_if_statement(
void
) {
# line 134 "../../cc/c1/statement.g"

		struct expr *expr;
		label l_true = text_label();
		label l_false = text_label();
		label l_end = text_label();
	
	C1_TRACE("c1x: if enter\n");
LLsincr(15);
LLtincr(90);
LLsincr(27);
LLtincr(38);
LL_SAFE(IF);
LL_NOSCANDONE('(');
LLread();
LLsdecr(15);
LL38_expression(
# line 143 "../../cc/c1/statement.g"
&expr);
# line 144 "../../cc/c1/statement.g"
{
			C1_TRACE("c1x: if expression parsed\n");
			opnd2test(&expr, IF);
			C1_TRACE("c1x: if opnd2test returned\n");
			if (is_cp_cst(expr))	{
				/*	The comparison has been optimized
					to a 0 or 1.
				*/
				if (expr->VL_VALUE == 0)	{
					C_bra(l_false);
				}
				/* else fall through */
#ifdef	LINT
				start_if_part(1);
#endif	/* LINT */
			}
			else	{
				C1_TRACE("c1x: if code_expr enter\n");
				code_expr(expr, RVAL, TRUE, l_true, l_false);
				C1_TRACE("c1x: if code_expr returned\n");
				C_df_ilb(l_true);
#ifdef	LINT
				start_if_part(0);
#endif	/* LINT */
			}
			free_expression(expr);
			C1_TRACE("c1x: if expression freed\n");
		}
LLtdecr(90);
LL_SCANDONE(')');
LLread();
C1_TRACE("c1x: if body enter\n");
LL63_statement();
C1_TRACE("c1x: if body returned\n");
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  ELSE  */ 38 : ;
LLtdecr(38);
LLsincr(27);
LL_SAFE(ELSE);
# line 171 "../../cc/c1/statement.g"
{
#ifdef	LINT
				start_else_part();
#endif	/* LINT */
				C_bra(l_end);
				C_df_ilb(l_false);
			}
LLread();
LL63_statement();
# line 179 "../../cc/c1/statement.g"
{	C_df_ilb(l_end);
#ifdef	LINT
				end_if_else_stmt();
#endif	/* LINT */
			}
break;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  BREAK  */ 32 : ;
case /*  CASE  */ 33 : ;
case /*  CONTINUE  */ 35 : ;
case /*  DEFAULT  */ 36 : ;
case /*  DO  */ 37 : ;
case /*  FOR  */ 41 : ;
case /*  GOTO  */ 42 : ;
case /*  IF  */ 43 : ;
case /*  RETURN  */ 46 : ;
case /*  SIZEOF  */ 49 : ;
case /*  SWITCH  */ 52 : ;
case /*  WHILE  */ 57 : ;
case /* ';' */ 86 : ;
case /* '(' */ 89 : ;
case /* '{' */ 93 : ;
case /* '}' */ 94 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
goto L_3;
L_3: ;
LLtdecr(38);
# line 186 "../../cc/c1/statement.g"
{	C_df_ilb(l_false);
#ifdef	LINT
				end_if_stmt();
#endif	/* LINT */
			}
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
}
static
void
LL67_while_statement(
void
) {
# line 196 "../../cc/c1/statement.g"

		struct expr *expr;
		label l_break = text_label();
		label l_continue = text_label();
		label l_body = text_label();
	
	C1_TRACE("c1x: while enter\n");
LLsincr(15);
LLtincr(90);
LLsincr(27);
LL_SAFE(WHILE);
# line 204 "../../cc/c1/statement.g"
{
			stack_stmt(l_break, l_continue);
			C_df_ilb(l_continue);
		}
LL_NOSCANDONE('(');
LLread();
LLsdecr(15);
LL38_expression(
# line 209 "../../cc/c1/statement.g"
&expr);
# line 210 "../../cc/c1/statement.g"
{
			C1_TRACE("c1x: while expression parsed\n");
			opnd2test(&expr, WHILE);
			C1_TRACE("c1x: while opnd2test returned\n");
			if (is_cp_cst(expr))	{
				if (expr->VL_VALUE == 0)	{
					C_bra(l_break);
				}
			}
			else	{
				C1_TRACE("c1x: while code_expr enter\n");
				code_expr(expr, RVAL, TRUE, l_body, l_break);
				C1_TRACE("c1x: while code_expr returned\n");
				C_df_ilb(l_body);
			}
#ifdef	LINT
			start_while_stmt(expr);
#endif	/* LINT */
		}
LLtdecr(90);
LL_SCANDONE(')');
LLread();
C1_TRACE("c1x: while body enter\n");
LL63_statement();
C1_TRACE("c1x: while body returned\n");
# line 227 "../../cc/c1/statement.g"
{
			C_bra(l_continue);
			C_df_ilb(l_break);
			unstack_stmt();
			free_expression(expr);
#ifdef	LINT
			end_loop_body();
			end_loop_stmt();
#endif	/* LINT */
		}
}
static
void
LL68_do_statement(
void
) {
# line 241 "../../cc/c1/statement.g"
	struct expr *expr;
		label l_break = text_label();
		label l_continue = text_label();
		label l_body = text_label();
	
LLsincr(27);
LLtincr(57);
LLtincr(89);
LLsincr(15);
LLtincr(90);
LLtincr(86);
LL_SAFE(DO);
# line 248 "../../cc/c1/statement.g"
{	C_df_ilb(l_body);
			stack_stmt(l_break, l_continue);
#ifdef	LINT
			start_do_stmt();
#endif	/* LINT */
		}
LLread();
LL63_statement();
LLtdecr(57);
LL_SCANDONE(WHILE);
LLtdecr(89);
LL_NOSCANDONE('(');
# line 257 "../../cc/c1/statement.g"
{
#ifdef	LINT
			end_loop_body();
#endif	/* LINT */
			C_df_ilb(l_continue);
		}
LLread();
LLsdecr(15);
LL38_expression(
# line 263 "../../cc/c1/statement.g"
&expr);
# line 264 "../../cc/c1/statement.g"
{
			opnd2test(&expr, WHILE);
			if (is_cp_cst(expr))	{
				if (expr->VL_VALUE == 1)	{
					C_bra(l_body);
				}
#ifdef	LINT
				end_do_stmt(1, expr->VL_VALUE != 0);
#endif	/* LINT */
			}
			else	{
				code_expr(expr, RVAL, TRUE, l_body, l_break);
#ifdef	LINT
				end_do_stmt(0, 0);
#endif	/* LINT */
			}
			C_df_ilb(l_break);
		}
LLtdecr(90);
LL_SCANDONE(')');
LLtdecr(86);
LL_NOSCANDONE(';');
# line 284 "../../cc/c1/statement.g"
{
			unstack_stmt();
			free_expression(expr);
		}
}
static
void
LL69_for_statement(
void
) {
# line 292 "../../cc/c1/statement.g"
	struct expr *e_init = 0, *e_test = 0, *e_incr = 0;
		label l_break = text_label();
		label l_continue = text_label();
		label l_body = text_label();
		label l_test = text_label();
	
LLsincr(4);
LLtincr(86);
LLsincr(4);
LLtincr(86);
LLsincr(4);
LLtincr(90);
LLsincr(27);
LL_SAFE(FOR);
# line 300 "../../cc/c1/statement.g"
{	stack_stmt(l_break, l_continue);
		}
LL_NOSCANDONE('(');
LLread();
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
LLsdecr(4);
break;
default:{int LL_28=LLnext(-56);
;if (!LL_28) {
LLsdecr(4);
break;
}
else if (LL_28 & 1) goto L_1;}
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
LL38_expression(
# line 304 "../../cc/c1/statement.g"
&e_init);
# line 305 "../../cc/c1/statement.g"
{	code_expr(e_init, RVAL, FALSE, NO_LABEL, NO_LABEL);
		}
}
}
LLtdecr(86);
LL_SCANDONE(';');
# line 309 "../../cc/c1/statement.g"
{	C_df_ilb(l_test);
		}
LLread();
goto L_2;
L_2 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
LLsdecr(4);
break;
default:{int LL_29=LLnext(-56);
;if (!LL_29) {
LLsdecr(4);
break;
}
else if (LL_29 & 1) goto L_2;}
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
LL38_expression(
# line 312 "../../cc/c1/statement.g"
&e_test);
# line 313 "../../cc/c1/statement.g"
{
			opnd2test(&e_test, FOR);
			if (is_cp_cst(e_test))	{
				if (e_test->VL_VALUE == 0)	{
					C_bra(l_break);
				}
			}
			else	{
				code_expr(e_test, RVAL, TRUE, l_body, l_break);
				C_df_ilb(l_body);
			}
		}
}
}
LLtdecr(86);
LL_SCANDONE(';');
LLread();
goto L_3;
L_3 : {switch(LLcsymb) {
case /* ')' */ 90 : ;
LLsdecr(4);
break;
default:{int LL_30=LLnext(-56);
;if (!LL_30) {
LLsdecr(4);
break;
}
else if (LL_30 & 1) goto L_3;}
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
LL38_expression(
# line 327 "../../cc/c1/statement.g"
&e_incr);
}
}
LLtdecr(90);
LL_SCANDONE(')');
# line 329 "../../cc/c1/statement.g"
{
#ifdef	LINT
			start_for_stmt(e_test);
#endif	/* LINT */
		}
LLread();
LL63_statement();
# line 335 "../../cc/c1/statement.g"
{
#ifdef	LINT
			end_loop_body();
#endif	/* LINT */
			C_df_ilb(l_continue);
			if (e_incr)
				code_expr(e_incr, RVAL, FALSE,
							NO_LABEL, NO_LABEL);
			C_bra(l_test);
			C_df_ilb(l_break);
			unstack_stmt();
			free_expression(e_init);
			free_expression(e_test);
			free_expression(e_incr);
#ifdef	LINT
			end_loop_stmt();
#endif	/* LINT */
		}
}
static
void
LL70_switch_statement(
void
) {
# line 357 "../../cc/c1/statement.g"

		struct expr *expr;
	
LLsincr(15);
LLtincr(90);
LLsincr(27);
LL_SAFE(SWITCH);
LL_NOSCANDONE('(');
LLread();
LLsdecr(15);
LL38_expression(
# line 363 "../../cc/c1/statement.g"
&expr);
# line 364 "../../cc/c1/statement.g"
{
			code_startswitch(&expr);
#ifdef	LINT
			start_switch_part(is_cp_cst(expr));
#endif	/* LINT */
		}
LLtdecr(90);
LL_SCANDONE(')');
LLread();
LL63_statement();
# line 372 "../../cc/c1/statement.g"
{
#ifdef	LINT
			end_switch_stmt();
#endif	/* LINT */
			code_endswitch();
			free_expression(expr);
		}
}
static
void
LL71_case_statement(
void
) {
# line 383 "../../cc/c1/statement.g"

		struct expr *expr;
	
LLtincr(95);
LLsincr(27);
LL_SAFE(CASE);
LLread();
LL17_constant_expression(
# line 388 "../../cc/c1/statement.g"
&expr);
# line 389 "../../cc/c1/statement.g"
{
#ifdef	LINT
			lint_case_stmt(0);
#endif	/* LINT */
			code_case(expr);
			free_expression(expr);
		}
LLtdecr(95);
LL_SCANDONE(':');
LLread();
LL63_statement();
}
static
void
LL72_default_statement(
void
) {
LLsincr(27);
LL_SAFE(DEFAULT);
# line 404 "../../cc/c1/statement.g"
{
#ifdef	LINT
			lint_case_stmt(1);
#endif	/* LINT */
			code_default();
		}
LL_NOSCANDONE(':');
LLread();
LL63_statement();
}
static
void
LL73_return_statement(
void
) {
# line 416 "../../cc/c1/statement.g"
	struct expr *expr = 0;
	
LLsincr(4);
LLtincr(86);
LL_SAFE(RETURN);
LLread();
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
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
LL38_expression(
# line 421 "../../cc/c1/statement.g"
&expr);
# line 422 "../../cc/c1/statement.g"
{
#ifdef	LINT
			lint_ret_conv(expr);
#endif	/* LINT */

			do_return_expr(expr);
			free_expression(expr);
#ifdef	LINT
			lint_return_stmt(VALRETURNED);
#endif	/* LINT */
		}
break;
case /* ';' */ 86 : ;
goto L_3;
L_3: ;
LLsdecr(4);
# line 435 "../../cc/c1/statement.g"
{
			do_return();
#ifdef	LINT
			lint_return_stmt(NOVALRETURNED);
#endif	/* LINT */
		}
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
LLtdecr(86);
LL_SCANDONE(';');
}
static
void
LL74_jump(
void
) {
# line 447 "../../cc/c1/statement.g"
	struct idf *idf;
	
LLtincr(86);
LL_SAFE(GOTO);
LLread();
LL16_identifier(
# line 451 "../../cc/c1/statement.g"
&idf);
LLtdecr(86);
LL_NOSCANDONE(';');
# line 453 "../../cc/c1/statement.g"
{
			apply_label(idf);
			C_bra((label)idf->id_label->df_address);
#ifdef	LINT
			lint_jump_stmt(idf);
#endif	/* LINT */
		}
}
void
LL62_compound_statement(
void
) {
# line 464 "../../cc/c1/statement.g"

#ifdef DBSYMTAB
	static int	brc_level = 1;
	int		decl_seen = brc_level == 1;
#endif /* DBSYMTAB */
	
LLsincr(0);
LLsincr(27);
LLtincr(94);
LL_SSCANDONE('{');
# line 472 "../../cc/c1/statement.g"
{
			stack_level();
		}
LLread();
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
# line 475 "../../cc/c1/statement.g"
if (((DOT != IDENTIFIER && AHEAD != ':') ||
		 (DOT == IDENTIFIER && AHEAD == IDENTIFIER))) goto L_2;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  BREAK  */ 32 : ;
case /*  CASE  */ 33 : ;
case /*  CONTINUE  */ 35 : ;
case /*  DEFAULT  */ 36 : ;
case /*  DO  */ 37 : ;
case /*  FOR  */ 41 : ;
case /*  GOTO  */ 42 : ;
case /*  IF  */ 43 : ;
case /*  RETURN  */ 46 : ;
case /*  SIZEOF  */ 49 : ;
case /*  SWITCH  */ 52 : ;
case /*  WHILE  */ 57 : ;
case /* ';' */ 86 : ;
case /* '(' */ 89 : ;
case /* '{' */ 93 : ;
case /* '}' */ 94 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
break;
default:{int LL_31=LLnext(0);
;if (!LL_31) {
break;
}
else if (LL_31 & 1) goto L_1;}
case /*  LLILLEGAL  */ 1 : ;
case /*  AUTO  */ 31 : ;
case /*  CONST  */ 34 : ;
case /*  ENUM  */ 39 : ;
case /*  EXTERN  */ 40 : ;
case /*  LONG  */ 44 : ;
case /*  REGISTER  */ 45 : ;
case /*  SHORT  */ 47 : ;
case /*  SIGNED  */ 48 : ;
case /*  STATIC  */ 50 : ;
case /*  STRUCT  */ 51 : ;
case /*  TYPEDEF  */ 53 : ;
case /*  UNION  */ 54 : ;
case /*  UNSIGNED  */ 55 : ;
case /*  VOLATILE  */ 56 : ;
case /*  VOID  */ 58 : ;
case /*  CHAR  */ 59 : ;
case /*  INT  */ 60 : ;
case /*  FLOAT  */ 61 : ;
case /*  DOUBLE  */ 62 : ;
goto L_2;
L_2 : ;
LL0_declaration();
# line 479 "../../cc/c1/statement.g"
{
#ifdef DBSYMTAB
			decl_seen++;
#endif /* DBSYMTAB */
		}
LLread();
continue;
}
}
LLsdecr(0);
break;
}
# line 485 "../../cc/c1/statement.g"
{
#ifdef DBSYMTAB
			++brc_level;
			if (options['g'] && decl_seen) {
				C_ms_std((char *) 0, N_LBRAC, brc_level);
			}
#endif /* DBSYMTAB */
		}
for (;;) {
goto L_3;
L_3 : {switch(LLcsymb) {
case /* '}' */ 94 : ;
break;
default:{int LL_32=LLnext(-378);
;if (!LL_32) {
break;
}
else if (LL_32 & 1) goto L_3;}
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /*  STRING  */ 4 : ;
case /*  INTEGER  */ 7 : ;
case /*  FLOATING  */ 8 : ;
case /*  PLUSPLUS  */ 21 : ;
case /*  MINMIN  */ 22 : ;
case /*  BREAK  */ 32 : ;
case /*  CASE  */ 33 : ;
case /*  CONTINUE  */ 35 : ;
case /*  DEFAULT  */ 36 : ;
case /*  DO  */ 37 : ;
case /*  FOR  */ 41 : ;
case /*  GOTO  */ 42 : ;
case /*  IF  */ 43 : ;
case /*  RETURN  */ 46 : ;
case /*  SIZEOF  */ 49 : ;
case /*  SWITCH  */ 52 : ;
case /*  WHILE  */ 57 : ;
case /* ';' */ 86 : ;
case /* '(' */ 89 : ;
case /* '{' */ 93 : ;
case /* '*' */ 96 : ;
case /* '&' */ 99 : ;
case /* '-' */ 100 : ;
case /* '+' */ 101 : ;
case /* '!' */ 102 : ;
case /* '~' */ 103 : ;
LLsincr(27);
LL63_statement();
continue;
}
}
LLsdecr(27);
break;
}
LLtdecr(94);
LL_SSCANDONE('}');
# line 497 "../../cc/c1/statement.g"
{
			unstack_level();
#ifdef DBSYMTAB
			if (options['g'] && decl_seen) {
				C_ms_std((char *) 0, N_RBRAC, brc_level);
			}
			brc_level--;
#endif /* DBSYMTAB */
		}
}

