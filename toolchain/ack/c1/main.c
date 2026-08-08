/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* MAIN PROGRAM */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "parameters.h"
#include <ack_string.h>
#include <system.h>
#include "idf.h"
#include "input.h"
#include "level.h"
#include "arith.h"
#include "type.h"
#include "proto.h"
#include "declar.h"
#include "tokenname.h"
#include "Lpars.h"
#include "LLlex.h"
#include <alloc.h>
#include "specials.h"
#include "sizes.h"
#include "align.h"
#include "stack.h"
#include "macro.h"
#include "options.h"
#include "error.h"
#include "code.h"
#include "cstoper.h"
#include "tokenname.h"

#ifdef ACK_TARGET_C1_TRACE
extern i32 write(i32 fd, char* buf, i32 count);

void c1_trace(char* message)
{
	char* p;

	for (p = message; *p; p++)
		;
	(void)write(2, message, (i32)(p - message));
}

void c1_trace_value(char* label, long value)
{
	char buf[24];
	char* p = &buf[sizeof(buf)];
	unsigned long n;
	int negative = 0;

	*--p = '\n';
	if (value < 0)
	{
		negative = 1;
		n = (unsigned long)(-value);
	}
	else
		n = (unsigned long)value;
	do
	{
		*--p = (char)('0' + n % 10);
		n /= 10;
	} while (n != 0);
	if (negative)
		*--p = '-';
	c1_trace(label);
	(void)write(2, p, (i32)(&buf[sizeof(buf)] - p));
}
#define C1_TRACE(s) c1_trace(s)
#else
#define C1_TRACE(s) ((void)0)
#endif

extern struct tokenname tkidf[];

extern char options[128];

struct sp_id special_ids[] = { { "__setjmp", SP_SETJMP }, /* non-local goto's are registered */
	                           { 0, 0 } };

void compile(int argc, char* argv[]);
static void init(void);
static void init_specials(struct sp_id* si);
#ifdef DEBUG
void Info(void);
#endif

extern void C_program(void); /* program.c */

#ifndef NOCROSS
arith short_size = SZ_SHORT, word_size = SZ_WORD, dword_size = (2 * SZ_WORD), int_size = SZ_INT,
      long_size = SZ_LONG, lnglng_size = SZ_LNGLNG, float_size = SZ_FLOAT, double_size = SZ_DOUBLE,
      lngdbl_size = SZ_LNGDBL, pointer_size = SZ_POINTER;

int short_align = AL_SHORT, word_align = AL_WORD, int_align = AL_INT, long_align = AL_LONG,
    lnglng_align = AL_LNGLNG, float_align = AL_FLOAT, double_align = AL_DOUBLE,
    lngdbl_align = AL_LNGDBL, pointer_align = AL_POINTER, struct_align = AL_STRUCT,
    union_align = AL_UNION;
#endif /* NOCROSS */

char* prog_name;

int main(int argc, char* argv[])
{
	/* parse and interpret the command line options	*/
	C1_TRACE("c1x: main enter\n");
	prog_name = argv[0];

	/*	Note: source file "-" indicates that the source is supplied
	    as standard input.  This is only allowed if INP_READ_IN_ONE is
	    not defined!
	*/
#ifdef INP_READ_IN_ONE
	while (argc > 1 && *argv[1] == '-')
#else /* INP_READ_IN_ONE */
	while (argc > 1 && *argv[1] == '-' && argv[1][1] != '\0')
#endif /* INP_READ_IN_ONE */
	{
		char* par = &argv[1][1];

		do_option(par);
		argc--, argv++;
	}
#ifdef LINT
	lint_init();
#endif /* LINT */
	compile(argc - 1, &argv[1]);
	C1_TRACE("c1x: compile returned\n");

#ifdef DEBUG
	if (options['h'])
		hash_stat();
	if (options['m'])
		Info();
#endif /* DEBUG */

	exit(err_occurred ? 1 : 0);
	UNREACHABLE_CODE;
}

char* source = 0;

#ifdef GEN_NM_LIST
char* nmlist = 0;
#endif /* GEN_NM_LIST */

void compile(int argc, char* argv[])
{
	char* result;
#ifndef LINT
	char* destination = 0;
#endif /* LINT */

	C1_TRACE("c1x: compile enter\n");
	switch (argc)
	{
		case 1:
#ifndef LINT
#ifdef DEBUG
#endif
			fatal("%s: destination file not specified", prog_name);
#endif /* LINT */
			break;

#ifndef LINT
		case 2:
			destination = argv[1];
			break;
#ifdef GEN_NM_LIST
		case 3:
			nmlist = argv[2];
			destination = argv[1];
			break;
#endif /* GEN_NM_LIST */
#endif /* LINT */

		default:
#ifndef LINT
#ifdef GEN_NM_LIST
			fatal("use: %s source destination [namelist]", prog_name);
#else /* GEN_NM_LIST */
			fatal("use: %s source destination", prog_name);
#endif /* GEN_NM_LIST */
#else /* LINT */
			fatal("use: %s source", prog_name);
#endif /* LINT */
			break;
	}

	if (strcmp(argv[0], "-"))
		FileName = source = argv[0];
	else
	{
		source = 0;
		FileName = strdup("standard input");
	}

	if (!InsertFile(source, (char**)0, &result)) /* read the source file	*/
		fatal("%s: no source file %s\n", prog_name, FileName);
	C1_TRACE("c1x: input inserted\n");
	File_Inserted = 1;
	init();
	C1_TRACE("c1x: init complete\n");
	LineNumber = 0;
	nestlow = -1;

#ifndef LINT
	init_code(destination && strcmp(destination, "-") != 0 ? destination : 0);
	C1_TRACE("c1x: output opened\n");
#endif /* LINT */

	GetToken(&ahead);
	C1_TRACE("c1x: first token\n");
	{
		/* compile the source text			*/
		C1_TRACE("c1x: C_program enter\n");
		C_program();
		C1_TRACE("c1x: C_program returned\n");

#ifdef PREPEND_SCOPES
		C1_TRACE("c1x: prepend_scopes enter\n");
		prepend_scopes();
		C1_TRACE("c1x: prepend_scopes returned\n");
#endif /* PREPEND_SCOPES */

#ifndef LINT
		C1_TRACE("c1x: end_code enter\n");
		end_code();
		C1_TRACE("c1x: end_code returned\n");
#endif /* LINT */

#ifdef DEBUG
		if (options['u'])
		{
			unstack_level(); /* unstack L_GLOBAL */
		}
		if (options['f'] || options['t'])
			dumpidftab("end of main", options['f'] ? 7 : 0);
#endif /* DEBUG */
	}
}

static void init(void)
{
	init_cst(); /* initialize variables of "cstoper.c"		*/
	reserve(tkidf); /* mark the C reserved words as such	*/
	init_specials(special_ids); /* mark special ids as such	*/

	schar_type = standard_type(CHAR, 0, 1, (arith)1);
	uchar_type = standard_type(CHAR, UNSIGNED, 1, (arith)1);

	short_type = standard_type(SHORT, 0, short_align, short_size);
	ushort_type = standard_type(SHORT, UNSIGNED, short_align, short_size);

	/*	Treat type `word' as `int', having its own size and
	    alignment requirements.
	    This type is transparent to the user.
	*/
	word_type = standard_type(INT, 0, word_align, word_size);
	uword_type = standard_type(INT, UNSIGNED, word_align, word_size);

	int_type = standard_type(INT, 0, int_align, int_size);
	uint_type = standard_type(INT, UNSIGNED, int_align, int_size);

	long_type = standard_type(LONG, 0, long_align, long_size);
	ulong_type = standard_type(LONG, UNSIGNED, long_align, long_size);

	lnglng_type = standard_type(LNGLNG, 0, lnglng_align, lnglng_size);
	ulnglng_type = standard_type(LNGLNG, UNSIGNED, lnglng_align, lnglng_size);

	float_type = standard_type(FLOAT, 0, float_align, float_size);
	double_type = standard_type(DOUBLE, 0, double_align, double_size);
	lngdbl_type = standard_type(LNGDBL, 0, lngdbl_align, lngdbl_size);
	void_type = standard_type(VOID, 0, 1, (arith)-1);
	error_type = standard_type(ERRONEOUS, 0, 1, (arith)1);
	error_type->tp_up = error_type;

	/*	Pointer Arithmetic type: all arithmetics concerning
	    pointers is supposed to be performed in the
	    pointer arithmetic type which is equal to either
	    int_type or long_type, depending on the pointer_size
	*/
	if ((int)pointer_size == (int)int_size)
		pa_type = int_type;
	else if ((int)pointer_size == (int)long_size)
		pa_type = long_type;
	else
		fatal("pointer size incompatible with any integral size");

	if ((int)int_size != (int)word_size)
		fatal("int_size and word_size are not equal");
	if ((int)short_size > (int)int_size || (int)int_size > (int)long_size)
		fatal("sizes of short/int/long decreasing");
	if ((int)float_size > (int)double_size || (int)double_size > (int)lngdbl_size)
		fatal("sizes of float/double/long double decreasing");

	/* Build a type for function returning int (3.3.2.2) */
	funint_type = construct_type(FUNCTION, int_type, 0, (arith)0, NO_PROTO);
	string_type = construct_type(POINTER, schar_type, 0, (arith)0, NO_PROTO);

	/* Define the standard type identifiers. */
	add_def(str2idf("char", 0), TYPEDEF, schar_type, L_UNIVERSAL);
	add_def(str2idf("int", 0), TYPEDEF, int_type, L_UNIVERSAL);
	add_def(str2idf("float", 0), TYPEDEF, float_type, L_UNIVERSAL);
	add_def(str2idf("double", 0), TYPEDEF, double_type, L_UNIVERSAL);
	add_def(str2idf("void", 0), TYPEDEF, void_type, L_UNIVERSAL);
	stack_level();
}

static void init_specials(struct sp_id* si)
{
	while (si->si_identifier)
	{
		struct idf* idf = str2idf(si->si_identifier, 0);

		if (idf->id_special)
			fatal("maximum identifier length insufficient");
		idf->id_special = si->si_flag;
		si++;
	}
}

#ifdef DEBUG
void Info(void)
{
	extern int cnt_string_cst, cnt_formal, cnt_decl_unary, cnt_def, cnt_expr, cnt_field,
	    cnt_e_stack, cnt_localvar, cnt_proto, cnt_stack_level, cnt_stack_entry, cnt_stmt_block,
	    cnt_sdef, cnt_tag, cnt_switch_hdr, cnt_case_entry, cnt_type, cnt_brace,
	    cnt_lint_stack_entry, cnt_state, cnt_auto_def, cnt_expr_state, cnt_argument;
	printf(
	    "\
%6d string_cst\n%6d formal\n\
%6d decl_unary\n%6d def\n%6d expr\n%6d field\n\
%6d e_stack\n%6d localvar\n%6d proto\n\
%6d stack_level\n\
%6d stack_entry\n%6d stmt_block\n%6d sdef\n%6d tag\n\
%6d switch_hdr\n%6d case_entry\n%6d type\n%6d brace\n\
%6d lint_stack_entry\n%6d state\n%6d auto_def\n\
%6d expr_state\n%6d argument\n",
	    cnt_string_cst, cnt_formal, cnt_decl_unary, cnt_def, cnt_expr, cnt_field, cnt_e_stack,
	    cnt_localvar, cnt_proto, cnt_stack_level, cnt_stack_entry, cnt_stmt_block, cnt_sdef,
	    cnt_tag, cnt_switch_hdr, cnt_case_entry, cnt_type, cnt_brace, cnt_lint_stack_entry,
	    cnt_state, cnt_auto_def, cnt_expr_state, cnt_argument);
}
#endif /* DEBUG */

void C_failed(void) /* called by EM_code module */
{
	fatal("write failed");
}
