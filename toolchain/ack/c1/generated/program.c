/* LLgen generated code from source ../../cc/c1/program.g */
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
# line 47 "../../cc/c1/program.g"

#include	<stddef.h>
#include 	<stdbool.h>
#include	"parameters.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"LLlex.h"
#include	"label.h"
#include	"type.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"code.h"
#include	"expr.h"
#include	"def.h"
#include    "idf.h"
#include    "declarator.h"
#include    "stack.h"
#include    "proto.h"
#include    "error.h"
#ifdef	LINT
#include	"l_lint.h"
#endif	/* LINT */

void LL0_declaration(void);
void LL1_decl_specifiers(
# line 90 "../../cc/c1/declar.g"
struct decspecs *ds) ;
void LL7_init_declarator(
# line 197 "../../cc/c1/declar.g"
struct decspecs *ds) ;
void LL8_declarator(
# line 303 "../../cc/c1/declar.g"
struct declarator *dc) ;
void LL9_initializer(
# line 229 "../../cc/c1/declar.g"
struct idf *idf ,int sc) ;
void LL17_constant_expression(
# line 344 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL59_external_definition(void);
static void LL60_function(
# line 189 "../../cc/c1/program.g"
struct decspecs *ds ,struct declarator *dc) ;
static void LL61_non_function(
# line 167 "../../cc/c1/program.g"
struct decspecs *ds ,struct declarator *dc) ;
void LL62_compound_statement(void);
void
LL58_control_if_expression(
void
) {
# line 73 "../../cc/c1/program.g"

		struct expr *exprX;
	
LL17_constant_expression(
# line 77 "../../cc/c1/program.g"
&exprX);
# line 78 "../../cc/c1/program.g"
{
		}
}
void
LL57_program(
void
) {
LLsincr(23);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  EOFILE  */ 0 : ;
break;
default:{int LL_25=LLnext(-322);
;if (!LL_25) {
break;
}
else if (LL_25 & 1) goto L_1;}
case /*  LLILLEGAL  */ 1 : ;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
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
case /* ';' */ 86 : ;
case /* '(' */ 89 : ;
case /* '*' */ 96 : ;
LL59_external_definition();
LLread();
continue;
}
}
LLsdecr(23);
break;
}
# line 85 "../../cc/c1/program.g"
{ unstack_world(); }
}
static
void
LL59_external_definition(
void
) {
# line 126 "../../cc/c1/program.g"
	struct decspecs Ds;
		struct	declarator Dc;
	
LLsincr(24);
# line 130 "../../cc/c1/program.g"
{	Ds = null_decspecs;
		Dc = null_declarator;
	}
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
# line 133 "../../cc/c1/program.g"
if (!(DOT != IDENTIFIER || AHEAD == IDENTIFIER)) goto L_1;
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
LL1_decl_specifiers(
# line 134 "../../cc/c1/program.g"
&Ds);
break;
goto L_1;
L_1 : ;
default:
# line 136 "../../cc/c1/program.g"
{do_decspecs(&Ds);}
break;
}
goto L_5; /* so that the label is used for certain */
L_5: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* '(' */ 89 : ;
case /* '*' */ 96 : ;
LLsdecr(24);
LLsincr(2);
LLsincr(25);
LL8_declarator(
# line 139 "../../cc/c1/program.g"
&Dc);
# line 140 "../../cc/c1/program.g"
{
			declare_idf(&Ds, &Dc, level);
#ifdef	LINT
			lint_ext_def(Dc.dc_idf, Ds.ds_sc);
#endif	/* LINT */
		}
goto L_8; /* so that the label is used for certain */
L_8: ;
switch(LLcsymb) {
case /*  LLILLEGAL  */ 1 : ;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
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
case /* '{' */ 93 : ;
LLsdecr(25);
LL60_function(
# line 147 "../../cc/c1/program.g"
&Ds, &Dc);
break;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
goto L_9;
L_9: ;
LLsdecr(25);
# line 149 "../../cc/c1/program.g"
{	if (! Ds.ds_sc_given && ! Ds.ds_typequal &&
				    Ds.ds_notypegiven) {
					strict("declaration specifiers missing");
				}
			}
LL61_non_function(
# line 154 "../../cc/c1/program.g"
&Ds, &Dc);
break;
default: if (LLskip()) goto L_8;
goto L_9;
}
break;
case /* ';' */ 86 : ;
goto L_6;
L_6: ;
LLsdecr(24);
# line 157 "../../cc/c1/program.g"
{	if (! Ds.ds_sc_given && ! Ds.ds_typequal &&
			    Ds.ds_notypegiven) {
				strict("declaration missing");
			}
		}
LL_SSCANDONE(';');
break;
default: if (LLskip()) goto L_5;
goto L_6;
}
# line 164 "../../cc/c1/program.g"
{remove_declarator(&Dc); flush_strings(); }
}
static
void
LL61_non_function(
# line 167 "../../cc/c1/program.g"
struct decspecs *ds ,struct declarator *dc)  
{
LLtincr(87);
LLtincr(86);
# line 169 "../../cc/c1/program.g"
{	reject_params(dc);
	}
switch(LLcsymb) {
case /* '=' */ 88 : ;
LL9_initializer(
# line 172 "../../cc/c1/program.g"
dc->dc_idf, ds->ds_sc);
break;
default:
# line 174 "../../cc/c1/program.g"
{ code_declaration(dc->dc_idf, (struct expr *) 0, level, ds->ds_sc); }
break;
}
# line 176 "../../cc/c1/program.g"
{
#ifdef	LINT
		lint_non_function_decl(ds, dc);
#endif	/* LINT */
	}
for (;;) {
goto L_4;
L_4 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
break;
default:{int LL_26=LLnext(44);
;if (!LL_26) {
break;
}
else if (LL_26 & 1) goto L_4;}
case /* ',' */ 87 : ;
LL_SAFE(',');
LLread();
LL7_init_declarator(
# line 183 "../../cc/c1/program.g"
ds);
continue;
}
}
LLtdecr(87);
break;
}
LLtdecr(86);
LL_SSCANDONE(';');
}
static
void
LL60_function(
# line 189 "../../cc/c1/program.g"
struct decspecs *ds ,struct declarator *dc)  
{
# line 190 "../../cc/c1/program.g"

		arith fbytes;
		struct idf *idf = dc->dc_idf;
	
LLsincr(0);
LLsincr(26);
# line 195 "../../cc/c1/program.g"
{
#ifdef	LINT
		lint_start_function();
#endif	/* LINT */
		idf_initialized(idf);
		stack_level();		/* L_FORMAL1 declarations */
		declare_params(dc);
		begin_proc(ds, idf);	/* sets global function info */
		stack_level();		/* L_FORMAL2 declarations */
		declare_protos(dc);
	}
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* '{' */ 93 : ;
break;
default:{int LL_27=LLnext(0);
;if (!LL_27) {
break;
}
else if (LL_27 & 1) goto L_1;}
case /*  LLILLEGAL  */ 1 : ;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
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
LL0_declaration();
LLread();
continue;
}
}
LLsdecr(0);
break;
}
# line 207 "../../cc/c1/program.g"
{
		check_formals(idf, dc);		/* check style-mixtures */
		declare_formals(idf, &fbytes);
#ifdef	LINT
		lint_formals();
#endif	/* LINT */
	}
LLsdecr(26);
LL62_compound_statement();
# line 215 "../../cc/c1/program.g"
{
		end_proc(fbytes);
#ifdef	LINT
		lint_implicit_return();
#endif	/* LINT */
		unstack_level();	/* L_FORMAL2 declarations */
#ifdef	LINT
		lint_end_formals();
#endif	/* LINT */
		unstack_level();	/* L_FORMAL1 declarations */
#ifdef	LINT
		lint_end_function();
#endif	/* LINT */
	}
}


