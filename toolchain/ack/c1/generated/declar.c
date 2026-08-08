/* LLgen generated code from source ../../cc/c1/declar.g */
#include "Lpars.h"
#define LL_LEXI LLlex
#define first_of_parameter_type_list(x) LLfirst((x), 0)
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
# line 8 "../../cc/c1/declar.g"

#include	"parameters.h"
#include	<stddef.h>
#include	<stdbool.h>
#include	<alloc.h>
#include	<flt_arith.h>
#include    "idf.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"label.h"
#include	"code.h"
#include	"type.h"
#include	"proto.h"
#include	"struct.h"
#include	"field.h"
#include	"decspecs.h"
#include    "declarator.h"
#include	"def.h"
#include	"declar.h"
#include	"label.h"
#include	"expr.h"
#include	"sizes.h"
#include	"level.h"
#include    "error.h"
#include    "stab.h"

#ifdef	LINT
#include	"l_lint.h"
#endif	/* LINT */

extern char	options[];
void LL1_decl_specifiers(
# line 90 "../../cc/c1/declar.g"
struct decspecs *ds) ;
static void LL2_init_declarator_list(
# line 192 "../../cc/c1/declar.g"
struct decspecs *ds) ;
static void LL3_single_decl_specifier(
# line 106 "../../cc/c1/declar.g"
struct decspecs *ds) ;
static void LL4_struct_or_union_specifier(
# line 443 "../../cc/c1/declar.g"
struct type **tpp) ;
static void LL5_enum_specifier(
# line 377 "../../cc/c1/declar.g"
struct type **tpp) ;
static void LL6_type_specifier(
# line 175 "../../cc/c1/declar.g"
struct type **tpp) ;
void LL7_init_declarator(
# line 197 "../../cc/c1/declar.g"
struct decspecs *ds) ;
void LL8_declarator(
# line 303 "../../cc/c1/declar.g"
struct declarator *dc) ;
void LL9_initializer(
# line 229 "../../cc/c1/declar.g"
struct idf *idf ,int sc) ;
void LL10_initial_value(
# line 79 "../../cc/c1/ival.g"
struct type **tpp ,struct expr **expp) ;
static void LL11_primary_declarator(
# line 333 "../../cc/c1/declar.g"
struct declarator *dc) ;
static void LL12_parameter_type_list(
# line 615 "../../cc/c1/declar.g"
struct proto **plp) ;
static void LL13_formal_list(
# line 355 "../../cc/c1/declar.g"
struct formal **fmp) ;
static void LL14_arrayer(
# line 339 "../../cc/c1/declar.g"
arith *sizep) ;
static void LL15_pointer(
# line 732 "../../cc/c1/declar.g"
int *qual) ;
void LL16_identifier(
# line 349 "../../cc/c1/expression.g"
struct idf **idfp) ;
void LL17_constant_expression(
# line 344 "../../cc/c1/expression.g"
struct expr **expp) ;
static void LL18_formal(
# line 360 "../../cc/c1/declar.g"
struct formal **fmp) ;
static void LL19_enumerator_pack(
# line 407 "../../cc/c1/declar.g"
struct type *tp ,arith *lp) ;
static void LL20_enumerator(
# line 424 "../../cc/c1/declar.g"
struct type *tp ,arith *lp) ;
static void LL21_struct_declaration_pack(
# line 492 "../../cc/c1/declar.g"
struct type *stp) ;
static void LL22_struct_declaration(
# line 509 "../../cc/c1/declar.g"
struct type *stp ,struct sdef ***sdefpp ,arith *szp) ;
static void LL23_struct_declarator_list(
# line 515 "../../cc/c1/declar.g"
struct type *tp ,struct type *stp
			,struct sdef ***sdefpp ,arith *szp) ;
static void LL24_struct_declarator(
# line 522 "../../cc/c1/declar.g"
struct type *tp ,struct type *stp
			,struct sdef ***sdefpp ,arith *szp) ;
static void LL25_bit_expression(
# line 544 "../../cc/c1/declar.g"
struct field **fd) ;
static void LL27_abstract_declarator(
# line 577 "../../cc/c1/declar.g"
struct declarator *dc) ;
static void LL28_primary_abstract_declarator(
# line 606 "../../cc/c1/declar.g"
struct declarator *dc) ;
static void LL29_parameter_decl_list(
# line 641 "../../cc/c1/declar.g"
struct proto **plp) ;
static void LL30_parameter_decl(
# line 650 "../../cc/c1/declar.g"
struct proto **plp) ;
static void LL31_parameter_declarator(
# line 690 "../../cc/c1/declar.g"
struct declarator *dc) ;
static void LL32_primary_parameter_declarator(
# line 720 "../../cc/c1/declar.g"
struct declarator *dc) ;
static void LL33_type_qualifier_list(
# line 755 "../../cc/c1/declar.g"
int *qual) ;
void
LL0_declaration(
void
) {
# line 43 "../../cc/c1/declar.g"
struct decspecs Ds;
LLsincr(1);
LLtincr(86);
# line 45 "../../cc/c1/declar.g"
{Ds = null_decspecs;}
LL1_decl_specifiers(
# line 46 "../../cc/c1/declar.g"
&Ds);
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
LLsdecr(1);
break;
default:{int LL_1=LLnext(-14);
;if (!LL_1) {
LLsdecr(1);
break;
}
else if (LL_1 & 1) goto L_1;}
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* '(' */ 89 : ;
case /* '*' */ 96 : ;
LLsdecr(1);
LL2_init_declarator_list(
# line 47 "../../cc/c1/declar.g"
&Ds);
}
}
LLtdecr(86);
LL_SCANDONE(';');
}
void
LL1_decl_specifiers(
# line 90 "../../cc/c1/declar.g"
struct decspecs *ds)  
{
LLsincr(0);
LLsincr(0);
LL3_single_decl_specifier(
# line 95 "../../cc/c1/declar.g"
ds);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
# line 96 "../../cc/c1/declar.g"
if ((  (DOT==TYPE_IDENTIFIER
			&& ds->ds_size == 0
			&& ds->ds_unsigned == 0
			&& ds->ds_type == (struct type *)0)
		    || AHEAD == IDENTIFIER)) goto L_2;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
case /* ':' */ 95 : ;
case /* '*' */ 96 : ;
break;
default:{int LL_2=LLnext(0);
;if (!LL_2) {
break;
}
else if (LL_2 & 1) goto L_1;}
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
LLsincr(0);
LL3_single_decl_specifier(
# line 101 "../../cc/c1/declar.g"
ds);
continue;
}
}
LLsdecr(0);
break;
}
# line 103 "../../cc/c1/declar.g"
{do_decspecs(ds);}
}
static
void
LL3_single_decl_specifier(
# line 106 "../../cc/c1/declar.g"
struct decspecs *ds)  
{
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  AUTO  */ 31 : ;
case /*  EXTERN  */ 40 : ;
case /*  REGISTER  */ 45 : ;
case /*  STATIC  */ 50 : ;
case /*  TYPEDEF  */ 53 : ;
LLsdecr(0);
# line 109 "../../cc/c1/declar.g"
{	if (ds->ds_sc_given)
			error("repeated storage class specifier");
		ds->ds_sc_given = 1;
		ds->ds_sc = DOT;
	}
LLread();
break;
case /*  VOLATILE  */ 56 : ;
LLsdecr(0);
LL_SAFE(VOLATILE);
# line 116 "../../cc/c1/declar.g"
{	if (ds->ds_typequal & TQ_VOLATILE)
			error("repeated type qualifier");
		ds->ds_typequal |= TQ_VOLATILE;
	}
LLread();
break;
case /*  CONST  */ 34 : ;
LLsdecr(0);
LL_SAFE(CONST);
# line 122 "../../cc/c1/declar.g"
{	if (ds->ds_typequal & TQ_CONST)
			error("repeated type qualifier");
		ds->ds_typequal |= TQ_CONST;
	}
LLread();
break;
case /*  LONG  */ 44 : ;
case /*  SHORT  */ 47 : ;
LLsdecr(0);
# line 128 "../../cc/c1/declar.g"
{	if (ds->ds_size == LONG && DOT == LONG)
			ds->ds_size = LNGLNG;
		else {
			if (ds->ds_size)
				error("repeated size specifier");
			ds->ds_size = DOT;
		}
	}
LLread();
break;
case /*  SIGNED  */ 48 : ;
case /*  UNSIGNED  */ 55 : ;
LLsdecr(0);
# line 138 "../../cc/c1/declar.g"
{	if (ds->ds_unsigned != 0)
			error("repeated sign specifier");
		ds->ds_unsigned = DOT;
	}
LLread();
break;
case /*  VOID  */ 58 : ;
case /*  CHAR  */ 59 : ;
case /*  INT  */ 60 : ;
case /*  FLOAT  */ 61 : ;
case /*  DOUBLE  */ 62 : ;
LLsdecr(0);
# line 144 "../../cc/c1/declar.g"
{
		idf2type(dot.tk_idf, &ds->ds_type);
		ds->ds_typedef = 0;
	}
LLread();
break;
case /*  TYPE_IDENTIFIER  */ 3 : ;
goto L_3;
L_3: ;
LLsdecr(0);
LL_SSCANDONE(TYPE_IDENTIFIER);
# line 150 "../../cc/c1/declar.g"
{
		idf2type(dot.tk_idf, &ds->ds_type);
		ds->ds_typedef = 1;
	}
LLread();
break;
default: if (LLskip()) goto L_2;
goto L_3;
case /*  IDENTIFIER  */ 2 : ;
LLsdecr(0);
LL_SAFE(IDENTIFIER);
# line 157 "../../cc/c1/declar.g"
{
		error("%s is not a type identifier", dot.tk_idf->id_text);
		ds->ds_type = error_type;
		if (dot.tk_idf->id_def) {
			dot.tk_idf->id_def->df_type = error_type;
			dot.tk_idf->id_def->df_sc = TYPEDEF;
		}
	}
LLread();
break;
case /*  LLILLEGAL  */ 1 : ;
LLsdecr(0);
LL_SAFE(LLILLEGAL);
LL_NOSCANDONE(IDENTIFIER);
LLread();
break;
case /*  STRUCT  */ 51 : ;
case /*  UNION  */ 54 : ;
LLsdecr(0);
LL4_struct_or_union_specifier(
# line 169 "../../cc/c1/declar.g"
&ds->ds_type);
break;
case /*  ENUM  */ 39 : ;
LLsdecr(0);
LL5_enum_specifier(
# line 171 "../../cc/c1/declar.g"
&ds->ds_type);
break;
}
}
static
void
LL6_type_specifier(
# line 175 "../../cc/c1/declar.g"
struct type **tpp)  
{
# line 179 "../../cc/c1/declar.g"
struct decspecs Ds; Ds = null_decspecs;
LL1_decl_specifiers(
# line 181 "../../cc/c1/declar.g"
&Ds);
# line 182 "../../cc/c1/declar.g"
{
		if (Ds.ds_sc_given)
			error("storage class ignored");
		if (Ds.ds_sc == REGISTER)
			error("register ignored");
	}
# line 188 "../../cc/c1/declar.g"
{*tpp = Ds.ds_type;}
}
static
void
LL2_init_declarator_list(
# line 192 "../../cc/c1/declar.g"
struct decspecs *ds)  
{
LLtincr(87);
LL7_init_declarator(
# line 193 "../../cc/c1/declar.g"
ds);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
break;
default:{int LL_3=LLnext(44);
;if (!LL_3) {
break;
}
else if (LL_3 & 1) goto L_1;}
case /* ',' */ 87 : ;
LL_SAFE(',');
LLread();
LL7_init_declarator(
# line 194 "../../cc/c1/declar.g"
ds);
continue;
}
}
LLtdecr(87);
break;
}
}
void
LL7_init_declarator(
# line 197 "../../cc/c1/declar.g"
struct decspecs *ds)  
{
# line 198 "../../cc/c1/declar.g"

		struct declarator Dc;
	
LLsincr(2);
LLtincr(88);
# line 202 "../../cc/c1/declar.g"
{
		Dc = null_declarator;
	}
LL8_declarator(
# line 206 "../../cc/c1/declar.g"
&Dc);
# line 207 "../../cc/c1/declar.g"
{
		reject_params(&Dc);
		declare_idf(ds, &Dc, level);
#ifdef	LINT
		lint_declare_idf(Dc.dc_idf, ds->ds_sc);
#endif	/* LINT */
	}
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '=' */ 88 : ;
LLtdecr(88);
LL9_initializer(
# line 215 "../../cc/c1/declar.g"
Dc.dc_idf, ds->ds_sc);
break;
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
goto L_3;
L_3: ;
LLtdecr(88);
# line 217 "../../cc/c1/declar.g"
{ code_declaration(Dc.dc_idf, (struct expr *) 0, level, ds->ds_sc); }
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
# line 220 "../../cc/c1/declar.g"
{
#ifdef	LINT
		add_auto(Dc.dc_idf);
#endif	/* LINT */
		remove_declarator(&Dc);
	}
}
void
LL9_initializer(
# line 229 "../../cc/c1/declar.g"
struct idf *idf ,int sc)  
{
# line 230 "../../cc/c1/declar.g"

		struct expr *expr = (struct expr *) 0;
		int fund = idf->id_def->df_type->tp_fund;
		int autoagg = (level >= L_LOCAL
				&& sc != STATIC
				&& ( fund == STRUCT
				    || fund == UNION
				    || fund == ARRAY));
		int globalflag = level == L_GLOBAL
				|| (level >= L_LOCAL && sc == STATIC);
	
# line 242 "../../cc/c1/declar.g"
{	if (idf->id_def->df_type->tp_fund == FUNCTION)	{
			error("illegal initialization of function");
			idf->id_def->df_type->tp_fund = ERRONEOUS;
		}
		if (level == L_FORMAL2)
			error("illegal initialization of formal parameter");
	}
LL_SAFE('=');
# line 250 "../../cc/c1/declar.g"
{
		if (AHEAD != '{' && AHEAD != STRING ) autoagg = 0;
#ifdef	LINT
		lint_statement();
#endif	/* LINT */
		if (globalflag) {
			struct expr ex;
			code_declaration(idf, &ex, level, sc);
		}
		else if (autoagg)
			loc_init((struct expr *) 0, idf);
	}
LL10_initial_value(
# line 262 "../../cc/c1/declar.g"
(globalflag || autoagg) ?
				&(idf->id_def->df_type)
				: (struct type **)0,
			&expr);
# line 266 "../../cc/c1/declar.g"
{	if (! globalflag) {
			if (idf->id_def->df_type->tp_fund == FUNCTION)	{
				free_expression(expr);
				expr = 0;
			}
#ifdef	DEBUG
			print_expr("initializer-expression", expr);
#endif	/* DEBUG */
#ifdef	LINT
			change_state(idf, SET);
#endif	/* LINT */
#ifdef	DBSYMTAB
			if (options['g'] && level >= L_LOCAL && expr) {
				db_line(expr->ex_file, (unsigned) expr->ex_line);
			}
#endif	/* DBSYMTAB */
			if (autoagg)
				loc_init((struct expr *) 0, idf);
			else	code_declaration(idf, expr, level, sc);
		}
#ifdef	DBSYMTAB
		if (options['g'] && globalflag) {
			stb_string(idf->id_def, sc, idf->id_text);
		}
#endif	/* DBSYMTAB */
		idf_initialized(idf);
	}
}
void
LL8_declarator(
# line 303 "../../cc/c1/declar.g"
struct declarator *dc)  
{
# line 304 "../../cc/c1/declar.g"
	struct formal *fm = NO_PARAMS;
		struct proto *pl = NO_PROTO;
		arith count;
		int qual;
	
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* '(' */ 89 : ;
goto L_3;
L_3: ;
LLsdecr(2);
LLsincr(3);
LL11_primary_declarator(
# line 310 "../../cc/c1/declar.g"
dc);
LLread();
for (;;) {
goto L_4;
L_4 : {switch(LLcsymb) {
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
case /* ',' */ 87 : ;
case /* '=' */ 88 : ;
case /* ')' */ 90 : ;
case /* '{' */ 93 : ;
case /* ':' */ 95 : ;
break;
default:{int LL_4=LLnext(-42);
;if (!LL_4) {
break;
}
else if (LL_4 & 1) goto L_4;}
case /* '(' */ 89 : ;
case /* '[' */ 91 : ;
switch(LLcsymb) {
case /* '(' */ 89 : ;
LLsincr(0);
LLtincr(90);
LL_SAFE('(');
LLread();
goto L_9; /* so that the label is used for certain */
L_9: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
# line 313 "../../cc/c1/declar.g"
if (!(DOT != IDENTIFIER)) goto L_8;
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
LLsdecr(0);
LL12_parameter_type_list(
# line 314 "../../cc/c1/declar.g"
&pl);
break;
goto L_8;
L_8 : ;
default:
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
LLsdecr(0);
LL13_formal_list(
# line 316 "../../cc/c1/declar.g"
&fm);
break;
case /* ')' */ 90 : ;
goto L_13;
L_13: ;
LLsdecr(0);
break;
default: if (LLskip()) goto L_9;
goto L_13;
}
}
LLtdecr(90);
LL_SCANDONE(')');
# line 321 "../../cc/c1/declar.g"
{	add_decl_unary(dc, FUNCTION, 0, (arith)0, fm, pl);
			fm = NO_PARAMS;
		}
break;
default:
LL14_arrayer(
# line 325 "../../cc/c1/declar.g"
&count);
# line 326 "../../cc/c1/declar.g"
{add_decl_unary(dc, ARRAY, 0, count, NO_PARAMS, NO_PROTO);}
break;
}
LLread();
continue;
}
}
LLsdecr(3);
break;
}
break;
default: if (LLskip()) goto L_2;
goto L_3;
case /* '*' */ 96 : ;
LL15_pointer(
# line 329 "../../cc/c1/declar.g"
&qual);
LL8_declarator(
# line 329 "../../cc/c1/declar.g"
dc);
# line 330 "../../cc/c1/declar.g"
{add_decl_unary(dc, POINTER, qual, (arith)0, NO_PARAMS, NO_PROTO);}
break;
}
}
static
void
LL11_primary_declarator(
# line 333 "../../cc/c1/declar.g"
struct declarator *dc)  
{
switch(LLcsymb) {
default:
LL16_identifier(
# line 334 "../../cc/c1/declar.g"
&dc->dc_idf);
break;
case /* '(' */ 89 : ;
LLsincr(2);
LLtincr(90);
LL_SAFE('(');
LLread();
LL8_declarator(
# line 336 "../../cc/c1/declar.g"
dc);
LLtdecr(90);
LL_SCANDONE(')');
break;
}
}
static
void
LL14_arrayer(
# line 339 "../../cc/c1/declar.g"
arith *sizep)  
{
# line 340 "../../cc/c1/declar.g"
 struct expr *expr; 
LLsincr(4);
LLtincr(92);
LL_SAFE('[');
# line 343 "../../cc/c1/declar.g"
{ *sizep = (arith)-1; }
LLread();
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ']' */ 92 : ;
LLsdecr(4);
break;
default:{int LL_5=LLnext(-56);
;if (!LL_5) {
LLsdecr(4);
break;
}
else if (LL_5 & 1) goto L_1;}
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
LL17_constant_expression(
# line 345 "../../cc/c1/declar.g"
&expr);
# line 346 "../../cc/c1/declar.g"
{
				check_array_subscript(expr);
				*sizep = (arith)expr->VL_VALUE;
				free_expression(expr);
			}
}
}
LLtdecr(92);
LL_SCANDONE(']');
}
static
void
LL13_formal_list(
# line 355 "../../cc/c1/declar.g"
struct formal **fmp)  
{
LLsincr(5);
LL18_formal(
# line 357 "../../cc/c1/declar.g"
fmp);
LLread();
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ')' */ 90 : ;
break;
default:{int LL_6=LLnext(-70);
;if (!LL_6) {
break;
}
else if (LL_6 & 1) goto L_1;}
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* ',' */ 87 : ;
LL_SSCANDONE(',');
LLread();
LL18_formal(
# line 357 "../../cc/c1/declar.g"
fmp);
LLread();
continue;
}
}
LLsdecr(5);
break;
}
}
static
void
LL18_formal(
# line 360 "../../cc/c1/declar.g"
struct formal **fmp)  
{
# line 361 "../../cc/c1/declar.g"
struct idf *idf;	
LL16_identifier(
# line 363 "../../cc/c1/declar.g"
&idf);
# line 364 "../../cc/c1/declar.g"
{
		struct formal *new = new_formal();
		
		new->fm_idf = idf;
		new->next = *fmp;
		*fmp = new;
		if (idf->id_def && idf->id_def->df_sc == TYPEDEF) {
			error("typedef name %s may not be redeclared as a parameter", idf->id_text);
		}
	}
}
static
void
LL5_enum_specifier(
# line 377 "../../cc/c1/declar.g"
struct type **tpp)  
{
# line 378 "../../cc/c1/declar.g"

		struct idf *idf;
		arith l = (arith)0;
	
LLsincr(6);
# line 383 "../../cc/c1/declar.g"
{if (*tpp) error("multiple types in declaration");}
LL_SAFE(ENUM);
LLread();
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '{' */ 93 : ;
LLsdecr(6);
# line 386 "../../cc/c1/declar.g"
{declare_struct(ENUM, (struct idf *) 0, tpp);}
LL19_enumerator_pack(
# line 387 "../../cc/c1/declar.g"
*tpp, &l);
LLread();
break;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
goto L_3;
L_3: ;
LLsdecr(6);
LLtincr(93);
LL16_identifier(
# line 389 "../../cc/c1/declar.g"
&idf);
LLread();
goto L_5; /* so that the label is used for certain */
L_5: ;
switch(LLcsymb) {
case /* '{' */ 93 : ;
LLtdecr(93);
# line 391 "../../cc/c1/declar.g"
{declare_struct(ENUM, idf, tpp);}
LL19_enumerator_pack(
# line 392 "../../cc/c1/declar.g"
*tpp, &l);
# line 393 "../../cc/c1/declar.g"
{
#ifdef DBSYMTAB
				if (options['g']) {
					stb_tag(idf->id_tag, idf->id_text);
				}
#endif /*DBSYMTAB */
			}
LLread();
break;
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
case /* ',' */ 87 : ;
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
case /* ':' */ 95 : ;
case /* '*' */ 96 : ;
goto L_6;
L_6: ;
LLtdecr(93);
# line 401 "../../cc/c1/declar.g"
{apply_struct(ENUM, idf, tpp);}
break;
default: if (LLskip()) goto L_5;
goto L_6;
}
break;
default: if (LLskip()) goto L_2;
goto L_3;
}
}
static
void
LL19_enumerator_pack(
# line 407 "../../cc/c1/declar.g"
struct type *tp ,arith *lp)  
{
LLtincr(87);
LLtincr(87);
LLtincr(94);
LL_SAFE('{');
LL20_enumerator(
# line 409 "../../cc/c1/declar.g"
tp, lp);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ',' */ 87 : ;
# line 410 "../../cc/c1/declar.g"
if ((AHEAD != '}')) goto L_2;
case /* '}' */ 94 : ;
break;
default:{int LL_7=LLnext(44);
;if (!LL_7) {
break;
}
else if (LL_7 & 1) goto L_1;}
goto L_2;
L_2 : ;
LL_SAFE(',');
LL20_enumerator(
# line 412 "../../cc/c1/declar.g"
tp, lp);
continue;
}
}
LLtdecr(87);
break;
}
LLtdecr(87);
goto L_3;
L_3 : {switch(LLcsymb) {
default:
break;
case /* ',' */ 87 : ;
LL_SAFE(',');
# line 415 "../../cc/c1/declar.g"
{warning("unexpected trailing comma in enumerator pack");}
LLread();
}
}
LLtdecr(94);
LL_SCANDONE('}');
# line 418 "../../cc/c1/declar.g"
{tp->tp_size = int_size;}
}
static
void
LL20_enumerator(
# line 424 "../../cc/c1/declar.g"
struct type *tp ,arith *lp)  
{
# line 425 "../../cc/c1/declar.g"

		struct idf *idf;
		struct expr *expr;
	
LLtincr(88);
LLread();
LL16_identifier(
# line 430 "../../cc/c1/declar.g"
&idf);
LLread();
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ',' */ 87 : ;
case /* '}' */ 94 : ;
LLtdecr(88);
break;
default:{int LL_8=LLnext(61);
;if (!LL_8) {
LLtdecr(88);
break;
}
else if (LL_8 & 1) goto L_1;}
case /* '=' */ 88 : ;
LLtdecr(88);
LL_SAFE('=');
LLread();
LL17_constant_expression(
# line 433 "../../cc/c1/declar.g"
&expr);
# line 434 "../../cc/c1/declar.g"
{
			*lp = (arith)expr->VL_VALUE;
			free_expression(expr);
		}
}
}
# line 439 "../../cc/c1/declar.g"
{declare_enum(tp, idf, (*lp)++);}
}
static
void
LL4_struct_or_union_specifier(
# line 443 "../../cc/c1/declar.g"
struct type **tpp)  
{
# line 444 "../../cc/c1/declar.g"

		int fund;
		struct idf *idfX;
		struct idf *idf;
	
LLsincr(6);
# line 450 "../../cc/c1/declar.g"
{if (*tpp) error("multiple types in declaration");}
# line 452 "../../cc/c1/declar.g"
{fund = DOT;}
LLread();
goto L_5; /* so that the label is used for certain */
L_5: ;
switch(LLcsymb) {
case /* '{' */ 93 : ;
LLsdecr(6);
# line 454 "../../cc/c1/declar.g"
{
			declare_struct(fund, (struct idf *)0, tpp);
		}
LL21_struct_declaration_pack(
# line 457 "../../cc/c1/declar.g"
*tpp);
LLread();
break;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
goto L_6;
L_6: ;
LLsdecr(6);
LLtincr(93);
LL16_identifier(
# line 459 "../../cc/c1/declar.g"
&idfX);
# line 459 "../../cc/c1/declar.g"
{ idf = idfX; }
LLread();
goto L_8; /* so that the label is used for certain */
L_8: ;
switch(LLcsymb) {
case /* '{' */ 93 : ;
LLtdecr(93);
# line 461 "../../cc/c1/declar.g"
{
				declare_struct(fund, idf, tpp);
				(idf->id_tag->tg_busy)++;
			}
LL21_struct_declaration_pack(
# line 465 "../../cc/c1/declar.g"
*tpp);
# line 466 "../../cc/c1/declar.g"
{
				(idf->id_tag->tg_busy)--;
#ifdef DBSYMTAB
				if (options['g']) {
					stb_tag(idf->id_tag, idf->id_text);
				}
#endif /*DBSYMTAB */
			}
LLread();
break;
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
case /* ',' */ 87 : ;
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
case /* ':' */ 95 : ;
case /* '*' */ 96 : ;
goto L_9;
L_9: ;
LLtdecr(93);
# line 475 "../../cc/c1/declar.g"
{
			  /* a ';' means an empty declaration (probably)
			   * this means that we have to declare a new
			   * structure. (yegh)
			   */
			  if (DOT == ';' &&
			      ( !idf->id_tag ||
				 idf->id_tag->tg_level != level ||
				 idf->id_tag->tg_type->tp_size < 0
			     )) declare_struct(fund, idf, tpp);
			  else apply_struct(fund, idf, tpp);
			}
break;
default: if (LLskip()) goto L_8;
goto L_9;
}
break;
default: if (LLskip()) goto L_5;
goto L_6;
}
}
static
void
LL21_struct_declaration_pack(
# line 492 "../../cc/c1/declar.g"
struct type *stp)  
{
# line 493 "../../cc/c1/declar.g"

		struct sdef **sdefp = &stp->tp_sdef;
		arith size = (arith)0;
	
LLsincr(7);
LLtincr(94);
LL_SAFE('{');
LLread();
LLsdecr(7);
LLsincr(0);
for (;;) {
LL22_struct_declaration(
# line 502 "../../cc/c1/declar.g"
stp, &sdefp, &size);
LLread();
goto L_1;
L_1 : {switch(LLcsymb) {
case /* '}' */ 94 : ;
break;
default:{int LL_9=LLnext(0);
;if (!LL_9) {
break;
}
else if (LL_9 & 1) goto L_1;}
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
continue;
}
}
LLsdecr(0);
break;
}
LLtdecr(94);
LL_SSCANDONE('}');
# line 504 "../../cc/c1/declar.g"
{stp->tp_size = align(size, stp->tp_align);
	 completed(stp);
	}
}
static
void
LL22_struct_declaration(
# line 509 "../../cc/c1/declar.g"
struct type *stp ,struct sdef ***sdefpp ,arith *szp)  
{
# line 510 "../../cc/c1/declar.g"
struct type *tp;
LLsincr(8);
LLtincr(86);
LL6_type_specifier(
# line 512 "../../cc/c1/declar.g"
&tp);
LLsdecr(8);
LL23_struct_declarator_list(
# line 512 "../../cc/c1/declar.g"
tp, stp, sdefpp, szp);
LLtdecr(86);
LL_SCANDONE(';');
}
static
void
LL23_struct_declarator_list(
# line 515 "../../cc/c1/declar.g"
struct type *tp ,struct type *stp
			,struct sdef ***sdefpp ,arith *szp)  
{
LLtincr(87);
LL24_struct_declarator(
# line 518 "../../cc/c1/declar.g"
tp, stp, sdefpp, szp);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
break;
default:{int LL_10=LLnext(44);
;if (!LL_10) {
break;
}
else if (LL_10 & 1) goto L_1;}
case /* ',' */ 87 : ;
LL_SAFE(',');
LLread();
LL24_struct_declarator(
# line 519 "../../cc/c1/declar.g"
tp, stp, sdefpp, szp);
continue;
}
}
LLtdecr(87);
break;
}
}
static
void
LL24_struct_declarator(
# line 522 "../../cc/c1/declar.g"
struct type *tp ,struct type *stp
			,struct sdef ***sdefpp ,arith *szp)  
{
# line 524 "../../cc/c1/declar.g"

		struct declarator Dc;
		struct field *fd = 0;
	
LLsincr(9);
# line 529 "../../cc/c1/declar.g"
{
		Dc = null_declarator;
	}
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* '(' */ 89 : ;
case /* '*' */ 96 : ;
goto L_3;
L_3: ;
LLsdecr(9);
LLsincr(2);
LLtincr(95);
LL8_declarator(
# line 533 "../../cc/c1/declar.g"
&Dc);
# line 534 "../../cc/c1/declar.g"
{reject_params(&Dc);}
goto L_4;
L_4 : {switch(LLcsymb) {
case /* ';' */ 86 : ;
case /* ',' */ 87 : ;
LLtdecr(95);
break;
default:{int LL_11=LLnext(58);
;if (!LL_11) {
LLtdecr(95);
break;
}
else if (LL_11 & 1) goto L_4;}
case /* ':' */ 95 : ;
LLtdecr(95);
LL25_bit_expression(
# line 535 "../../cc/c1/declar.g"
&fd);
}
}
break;
default: if (LLskip()) goto L_2;
goto L_3;
case /* ':' */ 95 : ;
LLsdecr(9);
# line 537 "../../cc/c1/declar.g"
{Dc.dc_idf = gen_idf();}
LL25_bit_expression(
# line 538 "../../cc/c1/declar.g"
&fd);
break;
}
# line 540 "../../cc/c1/declar.g"
{add_sel(stp, declare_type(tp, &Dc), Dc.dc_idf, sdefpp, szp, fd);}
# line 541 "../../cc/c1/declar.g"
{remove_declarator(&Dc);}
}
static
void
LL25_bit_expression(
# line 544 "../../cc/c1/declar.g"
struct field **fd)  
{
# line 545 "../../cc/c1/declar.g"
 struct expr *expr; 
# line 547 "../../cc/c1/declar.g"
{
		*fd = new_field();
	}
LL_SAFE(':');
LLread();
LL17_constant_expression(
# line 551 "../../cc/c1/declar.g"
&expr);
# line 552 "../../cc/c1/declar.g"
{
		(*fd)->fd_width = (arith)expr->VL_VALUE;
		free_expression(expr);
#ifdef NOBITFIELD
		error("bitfields are not implemented");
#endif /* NOBITFIELD */
	}
}
void
LL26_cast(
# line 562 "../../cc/c1/declar.g"
struct type **tpp)  
{
# line 563 "../../cc/c1/declar.g"
struct declarator Dc;
LLsincr(10);
LLtincr(90);
# line 565 "../../cc/c1/declar.g"
{Dc = null_declarator;}
LL_SAFE('(');
LLread();
LL6_type_specifier(
# line 567 "../../cc/c1/declar.g"
tpp);
LL27_abstract_declarator(
# line 568 "../../cc/c1/declar.g"
&Dc);
LLtdecr(90);
LL_SCANDONE(')');
# line 570 "../../cc/c1/declar.g"
{*tpp = declare_type(*tpp, &Dc);}
# line 571 "../../cc/c1/declar.g"
{remove_declarator(&Dc);}
}
static
void
LL27_abstract_declarator(
# line 577 "../../cc/c1/declar.g"
struct declarator *dc)  
{
# line 578 "../../cc/c1/declar.g"
	struct proto *pl = NO_PROTO;
		arith count;
		int qual;
	
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
goto L_3;
L_3: ;
LLsdecr(10);
LLsincr(3);
LL28_primary_abstract_declarator(
# line 583 "../../cc/c1/declar.g"
dc);
for (;;) {
goto L_4;
L_4 : {switch(LLcsymb) {
case /* ')' */ 90 : ;
break;
default:{int LL_12=LLnext(-42);
;if (!LL_12) {
break;
}
else if (LL_12 & 1) goto L_4;}
case /* '(' */ 89 : ;
case /* '[' */ 91 : ;
switch(LLcsymb) {
default:
LLsincr(0);
LLtincr(90);
LL_SAFE('(');
LLread();
goto L_9; /* so that the label is used for certain */
L_9: ;
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
LLsdecr(0);
LL12_parameter_type_list(
# line 587 "../../cc/c1/declar.g"
&pl);
break;
case /* ')' */ 90 : ;
goto L_10;
L_10: ;
LLsdecr(0);
break;
default: if (LLskip()) goto L_9;
goto L_10;
}
LLtdecr(90);
LL_SCANDONE(')');
# line 592 "../../cc/c1/declar.g"
{add_decl_unary(dc, FUNCTION, 0, (arith)0, NO_PARAMS, pl);
		 if (pl) remove_proto_idfs(pl);
		}
break;
case /* '[' */ 91 : ;
LL14_arrayer(
# line 596 "../../cc/c1/declar.g"
&count);
# line 597 "../../cc/c1/declar.g"
{add_decl_unary(dc, ARRAY, 0, count, NO_PARAMS, NO_PROTO);}
break;
}
LLread();
continue;
}
}
LLsdecr(3);
break;
}
break;
default: if (LLskip()) goto L_2;
goto L_3;
case /* '*' */ 96 : ;
LL15_pointer(
# line 600 "../../cc/c1/declar.g"
&qual);
LL27_abstract_declarator(
# line 600 "../../cc/c1/declar.g"
dc);
# line 601 "../../cc/c1/declar.g"
{add_decl_unary(dc, POINTER, qual, (arith)0, NO_PARAMS, NO_PROTO);}
break;
}
}
static
void
LL28_primary_abstract_declarator(
# line 606 "../../cc/c1/declar.g"
struct declarator *dc)  
{
switch(LLcsymb) {
case /* '(' */ 89 : ;
# line 608 "../../cc/c1/declar.g"
if (!(AHEAD == ')' || first_of_parameter_type_list(AHEAD))) goto L_1;
default:
break;
goto L_1;
L_1 : ;
LLsincr(10);
LLtincr(90);
LL_SAFE('(');
LLread();
LL27_abstract_declarator(
# line 611 "../../cc/c1/declar.g"
dc);
LLtdecr(90);
LL_SCANDONE(')');
LLread();
break;
}
}
static
void
LL12_parameter_type_list(
# line 615 "../../cc/c1/declar.g"
struct proto **plp)  
{
# line 616 "../../cc/c1/declar.g"
	int save_level; 
LLtincr(87);
# line 618 "../../cc/c1/declar.g"
{	if (level > L_PROTO) {
			save_level = level;
			level = L_PROTO;
		} else level--;
	}
LL29_parameter_decl_list(
# line 623 "../../cc/c1/declar.g"
plp);
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ')' */ 90 : ;
LLtdecr(87);
break;
default:{int LL_13=LLnext(44);
;if (!LL_13) {
LLtdecr(87);
break;
}
else if (LL_13 & 1) goto L_1;}
case /* ',' */ 87 : ;
LLtdecr(87);
LL_SAFE(',');
LL_NOSCANDONE(ELLIPSIS);
# line 626 "../../cc/c1/declar.g"
{	struct proto *new = new_proto();

			new->next = *plp;
			new->pl_flag = PL_ELLIPSIS;
			*plp = new;
		}
LLread();
}
}
# line 634 "../../cc/c1/declar.g"
{	check_for_void(*plp);
		if (level == L_PROTO)
			level = save_level;
		else level++;
	}
}
static
void
LL29_parameter_decl_list(
# line 641 "../../cc/c1/declar.g"
struct proto **plp)  
{
LLsincr(11);
LL30_parameter_decl(
# line 643 "../../cc/c1/declar.g"
plp);
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /* ',' */ 87 : ;
# line 644 "../../cc/c1/declar.g"
if ((AHEAD != ELLIPSIS)) goto L_2;
case /* ')' */ 90 : ;
break;
default:{int LL_14=LLnext(-154);
;if (!LL_14) {
break;
}
else if (LL_14 & 1) goto L_1;}
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
case /* '(' */ 89 : ;
case /* '[' */ 91 : ;
case /* '*' */ 96 : ;
goto L_2;
L_2 : ;
LL_SSCANDONE(',');
LLread();
LL30_parameter_decl(
# line 646 "../../cc/c1/declar.g"
plp);
continue;
}
}
LLsdecr(11);
break;
}
}
static
void
LL30_parameter_decl(
# line 650 "../../cc/c1/declar.g"
struct proto **plp)  
{
# line 651 "../../cc/c1/declar.g"
	struct proto *new = new_proto();
		struct declarator Dc;
		struct decspecs Ds;
	
LLsincr(2);
# line 656 "../../cc/c1/declar.g"
{	Dc = null_declarator;
		Ds = null_decspecs;
	}
LL1_decl_specifiers(
# line 659 "../../cc/c1/declar.g"
&Ds);
LL31_parameter_declarator(
# line 660 "../../cc/c1/declar.g"
&Dc);
# line 661 "../../cc/c1/declar.g"
{	add_proto(new, &Ds, &Dc, level);
		new->next = *plp;
		*plp = new;
		remove_declarator(&Dc);
	}
}
static
void
LL31_parameter_declarator(
# line 690 "../../cc/c1/declar.g"
struct declarator *dc)  
{
# line 691 "../../cc/c1/declar.g"
	struct formal *fm = NO_PARAMS;
		struct proto *pl = NO_PROTO;
		arith count;
		int qual;
	
goto L_2; /* so that the label is used for certain */
L_2: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* ',' */ 87 : ;
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
goto L_3;
L_3: ;
LLsdecr(2);
LLsincr(3);
LL32_primary_parameter_declarator(
# line 697 "../../cc/c1/declar.g"
dc);
for (;;) {
goto L_4;
L_4 : {switch(LLcsymb) {
case /* ',' */ 87 : ;
case /* ')' */ 90 : ;
break;
default:{int LL_15=LLnext(-42);
;if (!LL_15) {
break;
}
else if (LL_15 & 1) goto L_4;}
case /* '(' */ 89 : ;
case /* '[' */ 91 : ;
switch(LLcsymb) {
case /* '(' */ 89 : ;
LLsincr(0);
LLtincr(90);
LL_SAFE('(');
LLread();
goto L_9; /* so that the label is used for certain */
L_9: ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
# line 700 "../../cc/c1/declar.g"
if (!(DOT != IDENTIFIER)) goto L_8;
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
LLsdecr(0);
LL12_parameter_type_list(
# line 701 "../../cc/c1/declar.g"
&pl);
break;
goto L_8;
L_8 : ;
default:
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
LLsdecr(0);
LL13_formal_list(
# line 703 "../../cc/c1/declar.g"
&fm);
break;
case /* ')' */ 90 : ;
goto L_13;
L_13: ;
LLsdecr(0);
break;
default: if (LLskip()) goto L_9;
goto L_13;
}
}
LLtdecr(90);
LL_SCANDONE(')');
# line 708 "../../cc/c1/declar.g"
{   add_decl_unary(dc, FUNCTION, 0, (arith)0, fm, pl);
		    reject_params(dc);
		}
break;
default:
LL14_arrayer(
# line 712 "../../cc/c1/declar.g"
&count);
# line 713 "../../cc/c1/declar.g"
{add_decl_unary(dc, ARRAY, 0, count, NO_PARAMS, NO_PROTO);}
break;
}
LLread();
continue;
}
}
LLsdecr(3);
break;
}
break;
default: if (LLskip()) goto L_2;
goto L_3;
case /* '*' */ 96 : ;
LL15_pointer(
# line 716 "../../cc/c1/declar.g"
&qual);
LL31_parameter_declarator(
# line 716 "../../cc/c1/declar.g"
dc);
# line 717 "../../cc/c1/declar.g"
{add_decl_unary(dc, POINTER, qual, (arith)0, NO_PARAMS, NO_PROTO);}
break;
}
}
static
void
LL32_primary_parameter_declarator(
# line 720 "../../cc/c1/declar.g"
struct declarator *dc)  
{
switch(LLcsymb) {
case /* '(' */ 89 : ;
# line 722 "../../cc/c1/declar.g"
if (!(AHEAD == ')' || first_of_parameter_type_list(AHEAD)
				    && (AHEAD != IDENTIFIER))) goto L_1;
default:
break;
goto L_1;
L_1 : ;
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
LL16_identifier(
# line 726 "../../cc/c1/declar.g"
&dc->dc_idf);
LLread();
break;
case /* '(' */ 89 : ;
LLsincr(2);
LLtincr(90);
LL_SAFE('(');
LLread();
LL31_parameter_declarator(
# line 728 "../../cc/c1/declar.g"
dc);
LLtdecr(90);
LL_SCANDONE(')');
LLread();
break;
}
}
}
static
void
LL15_pointer(
# line 732 "../../cc/c1/declar.g"
int *qual)  
{
LL_SAFE('*');
LL33_type_qualifier_list(
# line 734 "../../cc/c1/declar.g"
qual);
}
static
void
LL33_type_qualifier_list(
# line 755 "../../cc/c1/declar.g"
int *qual)  
{
LLsincr(12);
# line 757 "../../cc/c1/declar.g"
{ *qual = 0; }
LLread();
for (;;) {
goto L_1;
L_1 : {switch(LLcsymb) {
case /*  IDENTIFIER  */ 2 : ;
case /*  TYPE_IDENTIFIER  */ 3 : ;
case /* ',' */ 87 : ;
case /* '(' */ 89 : ;
case /* ')' */ 90 : ;
case /* '[' */ 91 : ;
case /* '*' */ 96 : ;
break;
default:{int LL_16=LLnext(-168);
;if (!LL_16) {
break;
}
else if (LL_16 & 1) goto L_1;}
case /*  CONST  */ 34 : ;
case /*  VOLATILE  */ 56 : ;
switch(LLcsymb) {
default:
LL_SAFE(VOLATILE);
# line 760 "../../cc/c1/declar.g"
{	if (*qual & TQ_VOLATILE)
				error("repeated type qualifier");
			*qual |= TQ_VOLATILE;
		}
break;
case /*  CONST  */ 34 : ;
LL_SAFE(CONST);
# line 766 "../../cc/c1/declar.g"
{	if (*qual & TQ_CONST)
				error("repeated type qualifier");
			*qual |= TQ_CONST;
		}
break;
}
LLread();
continue;
}
}
LLsdecr(12);
break;
}
}


