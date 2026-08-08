/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRING = 258,
     IDENT = 259,
     FBSYM = 260,
     CODE1 = 261,
     CODE2 = 262,
     CODE4 = 263,
     NUMBER0 = 264,
     NUMBER1 = 265,
     NUMBER2 = 266,
     NUMBER3 = 267,
     NUMBER4 = 268,
     NUMBER8 = 269,
     NUMBERF = 270,
     DOT = 271,
     EXTERN = 272,
     DATA = 273,
     DATA8 = 274,
     DATAF = 275,
     ASCII = 276,
     SECTION = 277,
     COMMON = 278,
     BASE = 279,
     SYMB = 280,
     SYMD = 281,
     ALIGN = 282,
     ASSERT = 283,
     SPACE = 284,
     SEEK = 285,
     LINE = 286,
     FILe = 287,
     LIST = 288,
     OP_OO = 289,
     OP_AA = 290,
     OP_NE = 291,
     OP_EQ = 292,
     OP_GE = 293,
     OP_LE = 294,
     OP_RR = 295,
     OP_LL = 296,
     SIZE = 297,
     DREG = 298,
     AREG = 299,
     PC = 300,
     CREG = 301,
     SPEC = 302,
     ABCD = 303,
     ADDX = 304,
     ADD = 305,
     AND = 306,
     BITOP = 307,
     SHIFT = 308,
     SZ_EA = 309,
     OP_EA = 310,
     OP_NOOP = 311,
     OP_EA_D = 312,
     LEA = 313,
     DBR = 314,
     BR = 315,
     CMP = 316,
     MOVE = 317,
     MOVEP = 318,
     MOVEM = 319,
     MOVEC = 320,
     MOVES = 321,
     SWAP = 322,
     LINK = 323,
     UNLK = 324,
     TRAP = 325,
     STOP = 326,
     EXG = 327,
     OP_EXT = 328,
     RTD = 329,
     MODEL = 330,
     FPCR = 331,
     FPREG = 332,
     FMOVE = 333,
     FMOVECR = 334,
     FMOVEM = 335,
     FDYADIC = 336,
     FMONADIC = 337,
     FSINCOS = 338,
     FBCC = 339,
     FDBCC = 340,
     FNOP = 341,
     FSCC = 342,
     FTST = 343,
     FSAVRES = 344,
     FTRAPCC = 345,
     FSIZE = 346,
     CP = 347
   };
#endif
/* Tokens.  */
#define STRING 258
#define IDENT 259
#define FBSYM 260
#define CODE1 261
#define CODE2 262
#define CODE4 263
#define NUMBER0 264
#define NUMBER1 265
#define NUMBER2 266
#define NUMBER3 267
#define NUMBER4 268
#define NUMBER8 269
#define NUMBERF 270
#define DOT 271
#define EXTERN 272
#define DATA 273
#define DATA8 274
#define DATAF 275
#define ASCII 276
#define SECTION 277
#define COMMON 278
#define BASE 279
#define SYMB 280
#define SYMD 281
#define ALIGN 282
#define ASSERT 283
#define SPACE 284
#define SEEK 285
#define LINE 286
#define FILe 287
#define LIST 288
#define OP_OO 289
#define OP_AA 290
#define OP_NE 291
#define OP_EQ 292
#define OP_GE 293
#define OP_LE 294
#define OP_RR 295
#define OP_LL 296
#define SIZE 297
#define DREG 298
#define AREG 299
#define PC 300
#define CREG 301
#define SPEC 302
#define ABCD 303
#define ADDX 304
#define ADD 305
#define AND 306
#define BITOP 307
#define SHIFT 308
#define SZ_EA 309
#define OP_EA 310
#define OP_NOOP 311
#define OP_EA_D 312
#define LEA 313
#define DBR 314
#define BR 315
#define CMP 316
#define MOVE 317
#define MOVEP 318
#define MOVEM 319
#define MOVEC 320
#define MOVES 321
#define SWAP 322
#define LINK 323
#define UNLK 324
#define TRAP 325
#define STOP 326
#define EXG 327
#define OP_EXT 328
#define RTD 329
#define MODEL 330
#define FPCR 331
#define FPREG 332
#define FMOVE 333
#define FMOVECR 334
#define FMOVEM 335
#define FDYADIC 336
#define FMONADIC 337
#define FSINCOS 338
#define FBCC 339
#define FDBCC 340
#define FNOP 341
#define FSCC 342
#define FTST 343
#define FSAVRES 344
#define FTRAPCC 345
#define FSIZE 346
#define CP 347




/* Copy the first part of user declarations.  */
#line 13 "/tmp/cc-as-comm2.i"













 #include 	<ctype.h>
 #include 	<signal.h>
 #include 	<stdint.h>
 #include 	<stdio.h>
 #include 	<stdlib.h>
 #include 	<string.h>



































































































 #include 	"out.h"









 #include  <assert.h>
















































struct expr_t {
	short	typ;
	 int64_t 	val;
};

typedef	struct expr_t	expr_t;

struct item_t {
	struct item_t *
		i_next;
	short	i_type;
	




	 int64_t 	i_valu;
	char	*i_name;
};

struct common_t {
	struct common_t *
		c_next;
	struct item_t *c_it;

	 int64_t 	c_size;

};

typedef struct common_t	common_t;

typedef	struct item_t	item_t;

struct sect_t {
	short	s_flag;
	 long 	s_base;
	 long 	s_size;
	 long 	s_comm;
	 long 	s_zero;
	 long 	s_lign;
	long	s_foff;
	item_t	*s_item;

	 long 	s_gain;

};

typedef	struct sect_t	sect_t;





























































extern short	pass   ;

extern short	peekc;
extern short	unresolved;
extern long	lineno;
extern short	hllino;
extern short	nerrors;
extern short	sflag   ;

extern char	*progname;
extern char	*modulename;
extern common_t	*commons;


extern short	uflag;




extern short	dflag;




extern  int64_t 	relonami;



extern short	bflag;


extern char	*aoutpath   ;

extern FILE	*input;
extern FILE	*tempfile;

extern char	*stringbuf;
extern int	stringlen;

extern sect_t	sect[ 64 ];




extern sect_t	*DOTSCT;
extern  long 	DOTVAL;
extern short	DOTTYP;

extern unsigned short	nname;

extern item_t	*hashtab[ (2* 307 ) ];
extern short	hashindex;

extern item_t	*fb_ptr[4* 10 ];



extern int	nbits;
extern int	bitindex;	





extern short	listmode;
extern short	listtemp;
extern short	listflag;
extern short	listcolm;
extern short	listeoln   ;

extern FILE	*listfile;



extern item_t		keytab[];
extern struct outhead	outhead;


extern int	curr_token;



int	 yyparse(void);

void	 stop(void);
void	 newmodule(char *);

int	 yylex(void);
void	 putval(int);
int	 getval(int);
int	 nextchar(void);

int	 hash(const char *);
item_t	*item_search(const char *);
void	 item_insert(item_t *, int);
item_t	*item_alloc(int);
item_t	*fb_alloc(int);
item_t	*fb_shift(int);

void	 newequate(item_t *, int);
void	 newident(item_t *, int);
void	 newlabel(item_t *);
void	 newsect(item_t *);
void	 newbase( int64_t );
void	 newcomm(item_t *,  int64_t );
void	 switchsect(int);
void	 align( int64_t );

void	 newrelo(int, int);

long	 new_string(const char *);
void	 newsymb(const char *, int, int,  int64_t );

 int64_t 	 load(const item_t *);
int	 store(item_t *,  int64_t );
char	*remember(char *);
int	 combine(int, int, int);

int	 printx(int,  int64_t );
void	 listline(int);


int	 small(int, int);

void	 emit1(int);
void	 emit2(int);
void	 emit4(long);
void	 emitx( int64_t , int);
void	 emit8(int64_t);
void	 emitstr(int);
void	 emitf(int size, int negative);
void	 yyerror(const char *);
void	 nosect(void);
void	 fatal(const char *, ...);
void	 serror(const char *, ...);
void	 warning(const char *, ...);
void	 nofit(void);








































extern int	co_id;

extern int	mrg_1,mrg_2;
extern expr_t	exp_1,exp_2;

extern  int64_t 	rel_1,rel_2;

extern int 	model;
extern int	curr_instr;












extern long	eamode[];



void ea_1(int sz, int bits);
void ea_2(int sz, int bits);
void indexmode(int hibyte);
void checksize(int sz, int bits);
void test68010(void);
void badoperand(void);
void shift_op(int opc, int sz);
void bitop(int opc);
void add(int opc, int sz);
void and(int opc, int sz);
int to_dreg(int opc, int sz, int bits);
int from_dreg(int opc, int sz, int bits);
void cmp(int sz);
void move(int sz);
void move_special(int sz);
int reverse(int regs, int max);
void movem(int dr, int sz, int regs);
void movep(int sz);
void branch(int opc, expr_t exp);
void ea5x73(int rg, int sz);
void ea707172(int sz);
void ea6x(int rg, int ir, int sz);
void ea72(void);
void ea73(int ri, int sz);
void Xnofit(void);
void fbranch(int opc, expr_t exp);
void ch_sz_dreg(int size, int mode);
void check_fsize(int sz, int size);

static item_t	*last_it, *o_it;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 535 "/tmp/cc-as-comm2.i"
{
	 short 	y_word;
	 int64_t 	y_valu;
	expr_t	y_expr;
	item_t	*y_item;

}
/* Line 193 of yacc.c.  */
#line 808 "cc/as/generated/comm2.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 821 "cc/as/generated/comm2.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   901

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  116
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  182
/* YYNRULES -- Number of states.  */
#define YYNSTATES  389

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   347

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     106,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   107,     2,    51,    38,     2,
     112,   113,    49,    47,   109,    48,     2,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   104,   105,
      41,   108,    42,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   110,     2,   111,    37,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   114,    36,   115,    52,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    39,    40,    43,    44,    45,    46,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     8,    12,    15,    18,    21,    25,
      29,    35,    39,    40,    44,    46,    49,    54,    57,    60,
      61,    69,    76,    79,    82,    85,    88,    91,    94,    97,
     100,   103,   106,   108,   112,   114,   118,   120,   124,   126,
     129,   131,   135,   137,   139,   141,   143,   147,   151,   155,
     159,   163,   167,   171,   175,   179,   183,   187,   191,   195,
     199,   203,   207,   211,   215,   219,   222,   225,   228,   230,
     232,   234,   236,   237,   239,   240,   243,   244,   247,   252,
     263,   267,   271,   275,   278,   283,   286,   292,   297,   300,
     302,   306,   310,   314,   320,   326,   330,   335,   340,   345,
     349,   352,   355,   360,   363,   367,   370,   372,   374,   376,
     379,   381,   383,   385,   388,   390,   394,   396,   400,   402,
     404,   406,   408,   410,   414,   419,   424,   427,   433,   441,
     446,   454,   457,   459,   461,   462,   464,   465,   467,   468,
     473,   474,   478,   479,   482,   488,   494,   500,   506,   512,
     522,   531,   538,   544,   550,   556,   562,   568,   574,   580,
     586,   590,   598,   606,   609,   614,   616,   619,   623,   627,
     630,   632,   636,   638,   640,   642,   646,   648,   652,   654,
     658,   659,   661
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     117,     0,    -1,    -1,   117,     4,   104,    -1,   117,    14,
     104,    -1,   117,     6,    -1,   117,     7,    -1,   117,     8,
      -1,   117,   118,   105,    -1,   117,   118,   106,    -1,   117,
     107,    14,     3,   106,    -1,   117,     1,   106,    -1,    -1,
       4,   108,   125,    -1,    33,    -1,    22,     4,    -1,    23,
       4,   109,   127,    -1,    24,   127,    -1,    28,   125,    -1,
      -1,    25,     3,   109,   125,   119,   129,   129,    -1,    26,
       3,   109,   127,   109,   127,    -1,    31,   128,    -1,    32,
       3,    -1,    17,   120,    -1,    27,   128,    -1,    29,   127,
      -1,    30,   127,    -1,    18,   121,    -1,    19,   122,    -1,
      20,   124,    -1,    21,     3,    -1,     4,    -1,   120,   109,
       4,    -1,   125,    -1,   121,   109,   125,    -1,   127,    -1,
     122,   109,   127,    -1,    15,    -1,    48,    15,    -1,   123,
      -1,   124,   109,   123,    -1,     1,    -1,    14,    -1,   126,
      -1,     3,    -1,   110,   125,   111,    -1,   125,    34,   125,
      -1,   125,    35,   125,    -1,   125,    36,   125,    -1,   125,
      37,   125,    -1,   125,    38,   125,    -1,   125,    40,   125,
      -1,   125,    39,   125,    -1,   125,    41,   125,    -1,   125,
      42,   125,    -1,   125,    44,   125,    -1,   125,    43,   125,
      -1,   125,    45,   125,    -1,   125,    46,   125,    -1,   125,
      47,   125,    -1,   125,    48,   125,    -1,   125,    49,   125,
      -1,   125,    50,   125,    -1,   125,    51,   125,    -1,    47,
     125,    -1,    48,   125,    -1,    52,   125,    -1,    16,    -1,
       4,    -1,     5,    -1,   125,    -1,    -1,   127,    -1,    -1,
     109,   127,    -1,    -1,   130,   131,    -1,   132,    54,   109,
      54,    -1,   132,    48,   112,    55,   113,   109,    48,   112,
      55,   113,    -1,    61,   141,   143,    -1,    62,   142,   143,
      -1,    64,   141,   143,    -1,    71,   125,    -1,    70,    54,
     109,   125,    -1,    63,   143,    -1,    68,   141,   137,   109,
      54,    -1,    69,   137,   109,    55,    -1,   134,   137,    -1,
      67,    -1,    72,   141,   143,    -1,    73,   142,   143,    -1,
      74,   141,   143,    -1,    75,   141,   135,   109,   138,    -1,
      75,   141,   138,   109,   135,    -1,    77,   141,   143,    -1,
      76,   133,   109,   140,    -1,    76,   140,   109,   133,    -1,
      83,   140,   109,   140,    -1,    84,   141,    54,    -1,    78,
      54,    -1,    82,   139,    -1,    79,    55,   109,   139,    -1,
      80,    55,    -1,    81,   107,   127,    -1,    85,   139,    -1,
      86,    -1,   145,    -1,    59,    -1,    60,   141,    -1,    57,
      -1,    58,    -1,    66,    -1,    65,   141,    -1,   136,    -1,
     135,    50,   136,    -1,   140,    -1,   140,    48,   140,    -1,
      54,    -1,    55,    -1,    58,    -1,   138,    -1,   139,    -1,
     112,    55,   113,    -1,   112,    55,   113,    47,    -1,    48,
     112,    55,   113,    -1,   125,   142,    -1,   125,   112,   140,
     142,   113,    -1,   125,   112,    55,   109,   140,   141,   113,
      -1,   125,   112,    56,   113,    -1,   125,   112,    56,   109,
     140,   141,   113,    -1,   107,   125,    -1,    54,    -1,    55,
      -1,    -1,    53,    -1,    -1,    53,    -1,    -1,   137,   109,
     144,   137,    -1,    -1,   103,   146,   148,    -1,    -1,   147,
     148,    -1,    89,   153,   137,   109,    87,    -1,    89,   153,
      87,   109,   137,    -1,    89,   153,    88,   109,    88,    -1,
      89,   153,   137,   109,    88,    -1,    89,   153,    88,   109,
     137,    -1,    89,   153,    88,   109,   137,   114,   107,   127,
     115,    -1,    89,   153,    88,   109,   137,   114,    54,   115,
      -1,    90,   153,   107,   127,   109,    88,    -1,    91,   102,
     149,   109,   138,    -1,    91,   102,   138,   109,   149,    -1,
      91,    53,   152,   109,   137,    -1,    91,    53,   137,   109,
     152,    -1,    92,   153,   137,   109,    88,    -1,    92,   153,
      88,   109,    88,    -1,    93,   153,   137,   109,    88,    -1,
      93,   153,    88,   109,    88,    -1,    93,   153,    88,    -1,
      94,   153,   137,   109,    88,   104,    88,    -1,    94,   153,
      88,   109,    88,   104,    88,    -1,    95,   125,    -1,    96,
      54,   109,   125,    -1,    97,    -1,    98,   137,    -1,    99,
     153,   137,    -1,    99,   153,    88,    -1,   100,   137,    -1,
     101,    -1,   101,    53,   139,    -1,    54,    -1,   150,    -1,
     151,    -1,   150,    50,   151,    -1,    88,    -1,    88,    48,
      88,    -1,    87,    -1,   152,    50,    87,    -1,    -1,    53,
      -1,   102,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   673,   673,   675,   677,   684,   686,   688,   690,   691,
     693,   698,   705,   707,   717,   724,   726,   728,   730,   734,
     734,   774,   788,   802,   813,   814,   816,   822,   832,   833,
     834,   835,   839,   841,   845,   853,   864,   866,   871,   875,
     882,   883,   886,   890,   894,   899,   905,   907,   911,   915,
     919,   923,   927,   931,   935,   939,   943,   947,   951,   955,
     959,   963,   967,   971,   980,   989,   993,   997,  1001,  1006,
    1007,  1009,  1017,  1018,  1023,  1024,  1042,  1042,  1047,  1049,
    1051,  1053,  1055,  1057,  1059,  1068,  1070,  1077,  1081,  1085,
    1087,  1089,  1091,  1093,  1095,  1097,  1110,  1114,  1118,  1132,
    1134,  1136,  1138,  1140,  1142,  1144,  1149,  1151,  1153,  1154,
    1157,  1158,  1160,  1161,  1164,  1165,  1168,  1170,  1177,  1179,
    1181,  1183,  1184,  1187,  1189,  1191,  1193,  1197,  1201,  1205,
    1209,  1214,  1219,  1220,  1224,  1225,  1228,  1229,  1232,  1231,
    1238,  1237,  1240,  1240,  1243,  1251,  1259,  1263,  1269,  1277,
    1284,  1290,  1296,  1307,  1315,  1326,  1337,  1343,  1348,  1354,
    1359,  1364,  1369,  1374,  1376,  1386,  1390,  1395,  1401,  1406,
    1412,  1416,  1423,  1425,  1427,  1428,  1431,  1433,  1440,  1442,
    1446,  1447,  1454
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "IDENT", "FBSYM", "CODE1",
  "CODE2", "CODE4", "NUMBER0", "NUMBER1", "NUMBER2", "NUMBER3", "NUMBER4",
  "NUMBER8", "NUMBERF", "DOT", "EXTERN", "DATA", "DATA8", "DATAF", "ASCII",
  "SECTION", "COMMON", "BASE", "SYMB", "SYMD", "ALIGN", "ASSERT", "SPACE",
  "SEEK", "LINE", "FILe", "LIST", "OP_OO", "OP_AA", "'|'", "'^'", "'&'",
  "OP_NE", "OP_EQ", "'<'", "'>'", "OP_GE", "OP_LE", "OP_RR", "OP_LL",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'~'", "SIZE", "DREG", "AREG", "PC",
  "CREG", "SPEC", "ABCD", "ADDX", "ADD", "AND", "BITOP", "SHIFT", "SZ_EA",
  "OP_EA", "OP_NOOP", "OP_EA_D", "LEA", "DBR", "BR", "CMP", "MOVE",
  "MOVEP", "MOVEM", "MOVEC", "MOVES", "SWAP", "LINK", "UNLK", "TRAP",
  "STOP", "EXG", "OP_EXT", "RTD", "MODEL", "FPCR", "FPREG", "FMOVE",
  "FMOVECR", "FMOVEM", "FDYADIC", "FMONADIC", "FSINCOS", "FBCC", "FDBCC",
  "FNOP", "FSCC", "FTST", "FSAVRES", "FTRAPCC", "FSIZE", "CP", "':'",
  "';'", "'\\n'", "'#'", "'='", "','", "'['", "']'", "'('", "')'", "'{'",
  "'}'", "$accept", "program", "operation", "@1", "externlist", "datalist",
  "data8list", "numberf", "dataflist", "expr", "id_fb", "absexp",
  "optabs1", "optabs2", "@2", "_operation", "bcdx", "creg", "op_ea",
  "regs", "rrange", "ea", "notimmreg", "imm", "reg", "sizedef", "sizenon",
  "ea_ea", "@3", "fp_op", "@4", "@5", "fp_op1", "fregs", "frlist",
  "frrange", "fcregs", "fsize", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   124,    94,    38,   291,
     292,    60,    62,   293,   294,   295,   296,    43,    45,    42,
      47,    37,   126,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,    58,    59,    10,    35,    61,    44,
      91,    93,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   116,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   118,   118,   118,   118,   118,   118,   118,   119,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   120,   120,   121,   121,   122,   122,   123,   123,
     124,   124,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   126,
     126,   127,   128,   128,   129,   129,   130,   118,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   132,   132,
     133,   133,   134,   134,   135,   135,   136,   136,   137,   137,
     137,   137,   137,   138,   138,   138,   138,   138,   138,   138,
     138,   139,   140,   140,   141,   141,   142,   142,   144,   143,
     146,   145,   147,   145,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   149,   149,   150,   150,   151,   151,   152,   152,
     153,   153,   153
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     3,     2,     2,     2,     3,     3,
       5,     3,     0,     3,     1,     2,     4,     2,     2,     0,
       7,     6,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     3,     1,     3,     1,     3,     1,     2,
       1,     3,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     1,     1,
       1,     1,     0,     1,     0,     2,     0,     2,     4,    10,
       3,     3,     3,     2,     4,     2,     5,     4,     2,     1,
       3,     3,     3,     5,     5,     3,     4,     4,     4,     3,
       2,     2,     4,     2,     3,     2,     1,     1,     1,     2,
       1,     1,     1,     2,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     3,     4,     4,     2,     5,     7,     4,
       7,     2,     1,     1,     0,     1,     0,     1,     0,     4,
       0,     3,     0,     2,     5,     5,     5,     5,     5,     9,
       8,     6,     5,     5,     5,     5,     5,     5,     5,     5,
       3,     7,     7,     2,     4,     1,     2,     3,     3,     2,
       1,     3,     1,     1,     1,     3,     1,     3,     1,     3,
       0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     5,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,   142,    11,
       3,     0,     4,    32,    24,    42,    45,    69,    70,    43,
      68,     0,     0,     0,     0,    28,    34,    44,    29,    71,
      36,    38,     0,    40,    30,    31,    15,     0,    17,     0,
       0,    73,    25,    18,    26,    27,    22,    23,     0,     8,
       9,   108,   134,   134,   136,     0,   134,   134,   112,    89,
     134,     0,     0,     0,   134,   136,   134,   134,     0,   134,
       0,     0,     0,     0,     0,     0,   134,     0,   106,   140,
      77,     0,     0,   107,     0,    13,     0,    65,    66,    67,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    39,     0,     0,     0,     0,     0,   135,   109,     0,
     137,     0,     0,   118,   119,   120,     0,     0,   136,     0,
     121,   122,    85,     0,   113,     0,     0,     0,    83,     0,
       0,     0,     0,   132,   133,   110,   111,     0,     0,     0,
     100,     0,   103,     0,   101,     0,     0,   105,     0,     0,
       0,    88,   180,   180,     0,   180,   180,   180,     0,     0,
     165,     0,   180,     0,   170,   143,    33,    46,    35,    47,
      48,    49,    50,    51,    53,    52,    54,    55,    57,    56,
      58,    59,    60,    61,    62,    63,    64,    37,    41,    16,
      19,     0,    10,    80,    81,     0,   131,     0,     0,   126,
     138,    82,     0,     0,     0,    90,    91,    92,     0,   114,
       0,   116,     0,     0,    95,     0,   104,     0,    99,   141,
       0,     0,   181,   182,     0,     0,     0,     0,     0,     0,
       0,   163,     0,   166,     0,   169,     0,    74,     0,     0,
     123,   133,     0,   136,     0,     0,    87,    84,     0,     0,
       0,     0,    96,    97,   102,    98,     0,    78,     0,     0,
       0,     0,   178,     0,     0,   172,   176,     0,     0,   173,
     174,     0,     0,   160,     0,     0,     0,     0,   168,   167,
     171,     0,    74,    21,   125,   124,     0,     0,   129,     0,
     139,    86,   115,    93,    94,   117,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   164,    75,    20,   134,   134,   127,
       0,   145,   146,   148,   144,   147,     0,   155,   179,   154,
     177,   153,   152,   175,   157,   156,   159,   158,     0,     0,
       0,     0,     0,     0,   151,     0,     0,   128,   130,     0,
       0,     0,   162,   161,     0,   150,     0,    79,   149
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    27,   267,    34,    45,    48,    53,    54,   148,
      47,    61,    62,   312,    28,   100,   101,   167,   102,   238,
     239,   149,   150,   151,   241,   138,   141,   152,   274,   103,
     178,   104,   195,   298,   299,   300,   294,   254
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -158
static const yytype_int16 yypact[] =
{
    -158,   687,  -158,   -87,   -68,  -158,  -158,  -158,   -76,    43,
     634,   634,     7,    47,    62,    87,   634,    96,   118,   574,
     634,   634,   634,   574,   124,  -158,   117,   -53,   736,  -158,
    -158,   634,  -158,  -158,    24,  -158,  -158,  -158,  -158,  -158,
    -158,   634,   634,   634,   634,    26,   806,  -158,    29,   806,
    -158,  -158,   126,  -158,    39,  -158,  -158,    51,  -158,    53,
      58,  -158,  -158,   806,  -158,  -158,  -158,  -158,   148,  -158,
    -158,  -158,   116,   116,   120,   489,   116,   116,  -158,  -158,
     116,   489,   122,   634,   116,   120,   116,   116,    14,   116,
     128,   119,   125,    85,    90,    75,   116,    90,  -158,  -158,
    -158,   -25,   489,  -158,   -15,   806,   194,  -158,  -158,  -158,
     617,   634,   634,   634,   634,   634,   634,   634,   634,   634,
     634,   634,   634,   634,   634,   634,   634,   634,   634,   634,
     634,  -158,     7,   634,   634,   634,    95,  -158,  -158,   489,
    -158,   489,   513,  -158,  -158,  -158,   634,   147,   291,    94,
    -158,  -158,  -158,   489,  -158,   489,    97,    98,   806,   489,
     489,   489,    40,  -158,  -158,  -158,  -158,    99,   101,   489,
    -158,   102,  -158,   634,  -158,   103,   150,  -158,   -15,    93,
     104,  -158,   -45,   -45,   -35,   -45,   -45,   -45,   634,   162,
    -158,   489,   -45,   489,   164,  -158,  -158,  -158,   806,   823,
     787,   692,   837,   850,   561,   561,   108,   108,   108,   108,
     137,   137,    10,    10,  -158,  -158,  -158,  -158,  -158,  -158,
     806,   109,  -158,  -158,  -158,   165,   806,   110,    42,  -158,
    -158,  -158,   121,   171,   634,  -158,  -158,  -158,   -47,  -158,
     123,   179,    75,    88,  -158,    90,  -158,    75,  -158,  -158,
     173,   175,  -158,  -158,   167,   127,   236,   505,   266,   303,
     376,   806,   129,  -158,   392,  -158,    90,   133,   634,   130,
     184,   135,   -20,   120,   489,   191,  -158,   806,    75,   566,
      75,    75,  -158,  -158,  -158,  -158,   134,  -158,   139,   140,
     142,   634,  -158,   144,   -46,  -158,   185,   151,   152,   196,
    -158,   155,   156,   157,   166,   169,   176,   634,  -158,  -158,
    -158,   634,   133,  -158,  -158,  -158,    75,    75,  -158,   143,
    -158,  -158,  -158,  -158,   207,  -158,   177,   489,   413,    77,
     178,   172,   186,   489,   188,   -24,   566,   201,   204,   205,
     208,   210,   211,   212,   806,  -158,  -158,   116,   116,  -158,
     254,  -158,  -158,   189,  -158,  -158,   217,   261,  -158,  -158,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,   218,   245,
     199,   203,   240,   -37,  -158,   265,   268,  -158,  -158,   305,
     247,   634,  -158,  -158,   250,  -158,   249,  -158,  -158
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -158,  -158,  -158,  -158,  -158,  -158,  -158,   237,  -158,   -10,
    -158,     4,   345,    60,  -158,  -158,  -158,   131,  -158,   106,
      92,   -65,  -157,   -70,   -81,   -38,   -83,   206,  -158,  -158,
    -158,  -158,   209,    48,  -158,    45,    57,   -43
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -77
static const yytype_int16 yytable[] =
{
      46,    49,   160,   278,   332,   240,    49,   168,   252,    49,
      63,    49,    49,    49,   175,    50,   156,   380,   256,    29,
      58,   105,    51,   179,   174,    64,    65,   177,    32,   180,
     295,   107,   108,   109,   110,   139,    30,   181,   153,   154,
      31,    35,   155,    36,    37,    38,   159,    33,   161,   162,
      55,   169,    69,    70,    39,    52,    40,   253,   176,   127,
     128,   129,   279,   333,   296,   229,    56,   257,   163,   164,
     381,   165,   166,   158,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,    41,   142,   317,
     232,    57,    43,   318,   163,   164,   163,   271,   272,    59,
     297,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
      49,    60,   323,    49,   220,    49,   263,    67,   265,   163,
     164,    68,   108,   106,   217,   111,   226,   219,   130,   221,
     255,   131,   258,   259,   260,   165,   166,   273,   132,   264,
      44,   136,   147,   123,   124,   125,   126,   127,   128,   129,
     133,   282,   134,    49,   354,   355,   285,   135,    35,   137,
      36,    37,    38,   140,   171,   284,   157,   246,   261,   362,
     172,    39,   170,    40,   125,   126,   127,   128,   129,   290,
     319,   293,   173,   302,   304,   306,   310,   146,   196,   309,
     325,   222,   227,   230,   248,   250,   233,   234,   242,   320,
     243,   245,   247,   251,    41,   142,   262,   266,   268,    43,
     269,   143,   144,   270,   277,   145,   276,   281,   286,   287,
     275,   315,   280,   334,   291,   347,   348,    35,   307,    36,
      37,    38,   311,   314,   316,   321,   337,   326,   327,   328,
      39,   329,    40,   331,   288,   289,   349,   278,    49,   292,
     335,   336,   351,   353,   338,   339,   340,    35,   359,    36,
      37,    38,   313,   358,   146,   341,   360,    44,   342,   147,
      39,    49,    40,    41,   142,   343,   350,   356,    43,   296,
     143,   144,   364,   365,   145,   330,   366,   344,   367,   368,
     369,    49,   372,   373,    35,   374,    36,    37,    38,   370,
     371,   332,   377,    41,   142,   345,   378,    39,    43,    40,
     143,   144,   375,   292,   145,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   146,   140,   223,    44,   224,   147,   376,
      41,   142,   379,   382,   301,    43,   383,   143,   144,   231,
     384,   145,   385,   387,   388,   235,   236,   237,    66,   218,
     322,    49,   346,   146,   283,   244,    44,    35,   147,    36,
      37,    38,   363,   361,     0,   386,   324,   249,   357,     0,
      39,   303,    40,    35,     0,    36,    37,    38,     0,     0,
       0,     0,     0,   228,     0,     0,    39,     0,    40,     0,
     146,     0,     0,    44,    35,   147,    36,    37,    38,     0,
       0,     0,     0,    41,   142,     0,     0,    39,    43,    40,
     143,   144,     0,     0,   145,     0,     0,     0,     0,    41,
     142,     0,     0,     0,    43,     0,   143,   144,     0,     0,
     145,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      41,   142,     0,     0,   305,    43,     0,   143,   144,     0,
       0,   145,     0,     0,     0,     0,     0,     0,     0,     0,
     308,     0,     0,   146,     0,     0,    44,     0,   147,     0,
      35,     0,    36,    37,    38,     0,     0,     0,     0,   146,
       0,   352,    44,    39,   147,    40,    35,     0,    36,    37,
      38,     0,     0,     0,    35,     0,    36,    37,    38,    39,
     146,    40,     0,    44,     0,   147,     0,    39,     0,    40,
       0,     0,     0,     0,     0,     0,    41,   142,     0,     0,
       0,    43,     0,   143,   144,     0,     0,   145,     0,     0,
       0,     0,    41,   142,     0,     0,     0,    43,     0,   295,
      41,    42,     0,     0,     0,    43,     0,    35,     0,    36,
      37,    38,     0,     0,     0,    35,     0,    36,    37,    38,
      39,     0,    40,     0,     0,     0,     0,     0,    39,     0,
      40,     0,     0,   296,     0,     0,   146,     0,     0,    44,
       0,   147,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,    41,   142,    44,     0,   147,    43,     0,
       0,    41,    42,    44,     0,   225,    43,     0,     0,     0,
       0,     0,     0,     0,     0,    35,     0,    36,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,    39,     0,
      40,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,     0,
       0,     0,     0,     0,     0,     0,    44,     0,   147,   -72,
     -72,    41,    42,     0,    44,     0,    43,     2,     3,     0,
       0,     4,     0,     5,     6,     7,     0,     0,     0,     0,
       0,     8,     0,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,     0,     0,     0,     0,     0,     0,     0,   197,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    44,     0,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,     0,     0,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,     0,
     -76,     0,   -12,   -12,    26,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,    99,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129
};

static const yytype_int16 yycheck[] =
{
      10,    11,    85,    50,    50,   162,    16,    88,    53,    19,
      20,    21,    22,    23,    95,    11,    81,    54,    53,   106,
      16,    31,    15,    48,    94,    21,    22,    97,   104,    54,
      54,    41,    42,    43,    44,    73,   104,   102,    76,    77,
     108,     1,    80,     3,     4,     5,    84,     4,    86,    87,
       3,    89,   105,   106,    14,    48,    16,   102,    96,    49,
      50,    51,   109,   109,    88,   148,     4,   102,    54,    55,
     107,    57,    58,    83,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,    47,    48,   109,
     155,     4,    52,   113,    54,    55,    54,    55,    56,     3,
     257,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,     3,   279,   133,   134,   135,   191,     3,   193,    54,
      55,    14,   142,   109,   130,   109,   146,   133,   109,   135,
     183,    15,   185,   186,   187,    57,    58,   228,   109,   192,
     110,     3,   112,    45,    46,    47,    48,    49,    50,    51,
     109,   242,   109,   173,    87,    88,   247,   109,     1,    53,
       3,     4,     5,    53,    55,   245,    54,   173,   188,   336,
      55,    14,    54,    16,    47,    48,    49,    50,    51,   254,
     273,   256,   107,   258,   259,   260,   266,   107,     4,   264,
     281,   106,    55,   109,    54,   112,   109,   109,   109,   274,
     109,   109,   109,   109,    47,    48,    54,    53,   109,    52,
      55,    54,    55,   113,   234,    58,    55,    48,    55,    54,
     109,    47,   109,    48,   107,   316,   317,     1,   109,     3,
       4,     5,   109,   113,   109,    54,    50,   113,   109,   109,
      14,   109,    16,   109,    87,    88,   113,    50,   268,    87,
     109,   109,   327,   328,   109,   109,   109,     1,   333,     3,
       4,     5,   268,    87,   107,   109,    88,   110,   109,   112,
      14,   291,    16,    47,    48,   109,   109,   109,    52,    88,
      54,    55,    88,    88,    58,   291,    88,   307,    88,    88,
      88,   311,    48,   114,     1,    88,     3,     4,     5,   347,
     348,    50,   113,    47,    48,   311,   113,    14,    52,    16,
      54,    55,   104,    87,    58,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,   107,    53,   139,   110,   141,   112,   104,
      47,    48,   112,    88,    88,    52,    88,    54,    55,   153,
      55,    58,   115,   113,   115,   159,   160,   161,    23,   132,
     278,   381,   312,   107,   243,   169,   110,     1,   112,     3,
       4,     5,   337,   335,    -1,   381,   280,   178,   331,    -1,
      14,    88,    16,     1,    -1,     3,     4,     5,    -1,    -1,
      -1,    -1,    -1,   112,    -1,    -1,    14,    -1,    16,    -1,
     107,    -1,    -1,   110,     1,   112,     3,     4,     5,    -1,
      -1,    -1,    -1,    47,    48,    -1,    -1,    14,    52,    16,
      54,    55,    -1,    -1,    58,    -1,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    52,    -1,    54,    55,    -1,    -1,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,    88,    52,    -1,    54,    55,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    -1,    -1,   107,    -1,    -1,   110,    -1,   112,    -1,
       1,    -1,     3,     4,     5,    -1,    -1,    -1,    -1,   107,
      -1,    88,   110,    14,   112,    16,     1,    -1,     3,     4,
       5,    -1,    -1,    -1,     1,    -1,     3,     4,     5,    14,
     107,    16,    -1,   110,    -1,   112,    -1,    14,    -1,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    52,    -1,    54,    55,    -1,    -1,    58,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    -1,    52,    -1,    54,
      47,    48,    -1,    -1,    -1,    52,    -1,     1,    -1,     3,
       4,     5,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
      14,    -1,    16,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      16,    -1,    -1,    88,    -1,    -1,   107,    -1,    -1,   110,
      -1,   112,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    47,    48,   110,    -1,   112,    52,    -1,
      -1,    47,    48,   110,    -1,   112,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      16,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,   105,
     106,    47,    48,    -1,   110,    -1,    52,     0,     1,    -1,
      -1,     4,    -1,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    14,    -1,    -1,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,   110,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    -1,    -1,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,    -1,
     103,    -1,   105,   106,   107,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,   103,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   117,     0,     1,     4,     6,     7,     8,    14,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   107,   118,   130,   106,
     104,   108,   104,     4,   120,     1,     3,     4,     5,    14,
      16,    47,    48,    52,   110,   121,   125,   126,   122,   125,
     127,    15,    48,   123,   124,     3,     4,     4,   127,     3,
       3,   127,   128,   125,   127,   127,   128,     3,    14,   105,
     106,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,   103,
     131,   132,   134,   145,   147,   125,   109,   125,   125,   125,
     125,   109,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
     109,    15,   109,   109,   109,   109,     3,    53,   141,   141,
      53,   142,    48,    54,    55,    58,   107,   112,   125,   137,
     138,   139,   143,   141,   141,   141,   137,    54,   125,   141,
     142,   141,   141,    54,    55,    57,    58,   133,   140,   141,
      54,    55,    55,   107,   139,   140,   141,   139,   146,    48,
      54,   137,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   148,     4,   111,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   127,   123,   127,
     125,   127,   106,   143,   143,   112,   125,    55,   112,   142,
     109,   143,   137,   109,   109,   143,   143,   143,   135,   136,
     138,   140,   109,   109,   143,   109,   127,   109,    54,   148,
     112,   109,    53,   102,   153,   153,    53,   102,   153,   153,
     153,   125,    54,   137,   153,   137,    53,   119,   109,    55,
     113,    55,    56,   140,   144,   109,    55,   125,    50,   109,
     109,    48,   140,   133,   139,   140,    55,    54,    87,    88,
     137,   107,    87,   137,   152,    54,    88,   138,   149,   150,
     151,    88,   137,    88,   137,    88,   137,   109,    88,   137,
     139,   109,   129,   127,   113,    47,   109,   109,   113,   142,
     137,    54,   136,   138,   135,   140,   113,   109,   109,   109,
     127,   109,    50,   109,    48,   109,   109,    50,   109,   109,
     109,   109,   109,   109,   125,   127,   129,   140,   140,   113,
     109,   137,    88,   137,    87,    88,   109,   152,    87,   137,
      88,   149,   138,   151,    88,    88,    88,    88,    88,    88,
     141,   141,    48,   114,    88,   104,   104,   113,   113,   112,
      54,   107,    88,    88,    55,   115,   127,   113,   115
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 676 "/tmp/cc-as-comm2.i"
    {	newident((yyvsp[(2) - (3)].y_item), DOTTYP); newlabel((yyvsp[(2) - (3)].y_item));;}
    break;

  case 4:
#line 678 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(2) - (3)].y_valu) < 0 || (yyvsp[(2) - (3)].y_valu) > 9) {
					serror("bad f/b label");
					(yyvsp[(2) - (3)].y_valu) = 0;
				}
				newlabel(fb_shift((int)(yyvsp[(2) - (3)].y_valu)));
			;}
    break;

  case 5:
#line 685 "/tmp/cc-as-comm2.i"
    {	emit1((int)(yyvsp[(2) - (2)].y_valu));  if (listflag) listline(0 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;;}
    break;

  case 6:
#line 687 "/tmp/cc-as-comm2.i"
    {	emit2((int)(yyvsp[(2) - (2)].y_valu));  if (listflag) listline(0 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;;}
    break;

  case 7:
#line 689 "/tmp/cc-as-comm2.i"
    {	emit4((long)(yyvsp[(2) - (2)].y_valu));  if (listflag) listline(0 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;;}
    break;

  case 9:
#line 692 "/tmp/cc-as-comm2.i"
    {	lineno++;  if (listflag) listline(1 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;  assert(relonami == 0) ;;}
    break;

  case 10:
#line 694 "/tmp/cc-as-comm2.i"
    {	lineno = (yyvsp[(3) - (5)].y_valu);
				if (modulename) strncpy(modulename, stringbuf,  200 -1);
				 if (listflag) listline(1 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;  assert(relonami == 0) ;
			;}
    break;

  case 11:
#line 699 "/tmp/cc-as-comm2.i"
    {	serror("syntax error"); yyerrok;
				lineno++;  if (listflag) listline(1 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;  assert(relonami == 0) ;
			;}
    break;

  case 13:
#line 708 "/tmp/cc-as-comm2.i"
    {

				if (listflag & 1)
					listcolm += printx( 8 , (yyvsp[(3) - (3)].y_expr).val);

				newequate((yyvsp[(1) - (3)].y_item), (yyvsp[(3) - (3)].y_expr).typ);
				store((yyvsp[(1) - (3)].y_item), (yyvsp[(3) - (3)].y_expr).val);
			;}
    break;

  case 14:
#line 718 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(1) - (1)].y_word))
					listtemp = listmode;
				else if ((dflag & 01000) == 0)
					listtemp = 0;
			;}
    break;

  case 15:
#line 725 "/tmp/cc-as-comm2.i"
    {	newsect((yyvsp[(2) - (2)].y_item));;}
    break;

  case 16:
#line 727 "/tmp/cc-as-comm2.i"
    {	newcomm((yyvsp[(2) - (4)].y_item), (yyvsp[(4) - (4)].y_valu));;}
    break;

  case 17:
#line 729 "/tmp/cc-as-comm2.i"
    {	if (pass ==  0 ) newbase((yyvsp[(2) - (2)].y_valu));;}
    break;

  case 18:
#line 731 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(2) - (2)].y_expr).val == 0 && pass ==  2 )
					warning("assertion failed");
			;}
    break;

  case 19:
#line 734 "/tmp/cc-as-comm2.i"
    { o_it = last_it; ;}
    break;

  case 20:
#line 736 "/tmp/cc-as-comm2.i"
    {	if ((sflag &  010 ) &&  (pass !=  0 ) ) {

					if (
						pass ==  2
						&&
						((yyvsp[(4) - (7)].y_expr).typ & S_TYP) == S_UND
					   ) {
						serror("expression undefined");
						relonami = -1;
					}
					if (
						 (pass !=  0 )
						&&
						((yyvsp[(4) - (7)].y_expr).typ & S_COM)
					   ) {
						




						(yyvsp[(4) - (7)].y_expr).typ = S_CRS;
						(yyvsp[(4) - (7)].y_expr).val = new_string(o_it->i_name);
						relonami = 0;
					}


					newsymb(
						*stringbuf ? stringbuf : (char *) 0,
						(short)(
							((yyvsp[(4) - (7)].y_expr).typ & (S_EXT|S_TYP))
							|
							((unsigned short)(yyvsp[(6) - (7)].y_valu)<<8)
						),
						(short)(yyvsp[(7) - (7)].y_valu),
						(yyvsp[(4) - (7)].y_expr).val
					);
				}
			;}
    break;

  case 21:
#line 775 "/tmp/cc-as-comm2.i"
    {	if ((sflag &  010 ) &&  (pass !=  0 ) ) {
					newsymb(
						*stringbuf ? stringbuf : (char *) 0,
						(short)(
							(DOTTYP & (S_EXT|S_TYP))
							|
							((unsigned short)(yyvsp[(4) - (6)].y_valu)<<8)
						),
						(short)(yyvsp[(6) - (6)].y_valu),
						( int64_t )DOTVAL
					);
				}
			;}
    break;

  case 22:
#line 789 "/tmp/cc-as-comm2.i"
    {	if ((sflag &  020 ) &&  (pass !=  0 ) ) {
					if ((yyvsp[(2) - (2)].y_valu))
						hllino = (short)(yyvsp[(2) - (2)].y_valu);
					else
						hllino++;
					newsymb(
						(char *)0,
						(DOTTYP | S_LIN),
						hllino,
						( int64_t )DOTVAL
					);
				}
			;}
    break;

  case 23:
#line 803 "/tmp/cc-as-comm2.i"
    {	if ((sflag &  020 ) &&  (pass !=  0 ) ) {
					hllino = 0;
					newsymb(
						stringbuf,
						(DOTTYP | S_FIL),
						0,
						( int64_t )DOTVAL
					);
				}
			;}
    break;

  case 25:
#line 815 "/tmp/cc-as-comm2.i"
    {	align((yyvsp[(2) - (2)].y_valu));;}
    break;

  case 26:
#line 817 "/tmp/cc-as-comm2.i"
    {	if (DOTSCT == NULL)
					nosect();
				DOTVAL += (yyvsp[(2) - (2)].y_valu);
				DOTSCT->s_zero += (yyvsp[(2) - (2)].y_valu);
			;}
    break;

  case 27:
#line 823 "/tmp/cc-as-comm2.i"
    {	if (DOTSCT == NULL)
					nosect();
				if ((yyvsp[(2) - (2)].y_valu) < DOTVAL)
					serror("cannot move location counter backwards");
				if (pass ==  0 )
					DOTSCT->s_flag |=  2 ;
				DOTSCT->s_zero += (yyvsp[(2) - (2)].y_valu) - DOTVAL;
				DOTVAL = (yyvsp[(2) - (2)].y_valu);
			;}
    break;

  case 31:
#line 836 "/tmp/cc-as-comm2.i"
    {	emitstr((yyvsp[(1) - (2)].y_word));;}
    break;

  case 32:
#line 840 "/tmp/cc-as-comm2.i"
    {	(yyvsp[(1) - (1)].y_item)->i_type |= S_EXT;;}
    break;

  case 33:
#line 842 "/tmp/cc-as-comm2.i"
    {	(yyvsp[(3) - (3)].y_item)->i_type |= S_EXT;;}
    break;

  case 34:
#line 846 "/tmp/cc-as-comm2.i"
    {

				if ( 1  != 0 &&  (pass !=  0 ) )
					newrelo((yyvsp[(1) - (1)].y_expr).typ, (int)(yyvsp[(0) - (1)].y_word)| (RELBR|RELWR) );

				emitx((yyvsp[(1) - (1)].y_expr).val, (int)(yyvsp[(0) - (1)].y_word));
			;}
    break;

  case 35:
#line 854 "/tmp/cc-as-comm2.i"
    {

				if ( 1  != 0 &&  (pass !=  0 ) )
					newrelo((yyvsp[(3) - (3)].y_expr).typ, (int)(yyvsp[(0) - (3)].y_word)| (RELBR|RELWR) );

				emitx((yyvsp[(3) - (3)].y_expr).val, (int)(yyvsp[(0) - (3)].y_word));
			;}
    break;

  case 36:
#line 865 "/tmp/cc-as-comm2.i"
    {	emit8((yyvsp[(1) - (1)].y_valu));;}
    break;

  case 37:
#line 867 "/tmp/cc-as-comm2.i"
    {	emit8((yyvsp[(3) - (3)].y_valu));;}
    break;

  case 38:
#line 872 "/tmp/cc-as-comm2.i"
    {
				emitf((int)(yyvsp[(-1) - (1)].y_word), 0);
			;}
    break;

  case 39:
#line 876 "/tmp/cc-as-comm2.i"
    {
				emitf((int)(yyvsp[(-1) - (2)].y_word), 1);
			;}
    break;

  case 42:
#line 887 "/tmp/cc-as-comm2.i"
    {	serror("expr syntax err");
				(yyval.y_expr).val = 0; (yyval.y_expr).typ = S_UND;
			;}
    break;

  case 43:
#line 891 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = (yyvsp[(1) - (1)].y_valu);
				(yyval.y_expr).typ = S_ABS;
			;}
    break;

  case 44:
#line 895 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = load((yyvsp[(1) - (1)].y_item));
				last_it = (yyvsp[(1) - (1)].y_item);
				(yyval.y_expr).typ = (yyvsp[(1) - (1)].y_item)->i_type & ~S_EXT;
			;}
    break;

  case 45:
#line 900 "/tmp/cc-as-comm2.i"
    {	if (stringlen != 1)
					serror("too many chars");
				(yyval.y_expr).val = stringbuf[0];
				(yyval.y_expr).typ = S_ABS;
			;}
    break;

  case 46:
#line 906 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr) = (yyvsp[(2) - (3)].y_expr);;}
    break;

  case 47:
#line 908 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val || (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 48:
#line 912 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val && (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 49:
#line 916 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val | (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 50:
#line 920 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val ^ (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 51:
#line 924 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val & (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 52:
#line 928 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val == (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '>');
			;}
    break;

  case 53:
#line 932 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val != (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '>');
			;}
    break;

  case 54:
#line 936 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val < (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '>');
			;}
    break;

  case 55:
#line 940 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val > (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '>');
			;}
    break;

  case 56:
#line 944 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val <= (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '>');
			;}
    break;

  case 57:
#line 948 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val >= (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '>');
			;}
    break;

  case 58:
#line 952 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val >> (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 59:
#line 956 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val << (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 60:
#line 960 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val + (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '+');
			;}
    break;

  case 61:
#line 964 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val - (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, '-');
			;}
    break;

  case 62:
#line 968 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val * (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 63:
#line 972 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(3) - (3)].y_expr).val == 0) {
					if (pass ==  2 )
						serror("divide by zero");
					(yyval.y_expr).val = 0;
				} else
					(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val / (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 64:
#line 981 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(3) - (3)].y_expr).val == 0) {
					if (pass ==  2 )
						serror("divide by zero");
					(yyval.y_expr).val = 0;
				} else
					(yyval.y_expr).val = ((yyvsp[(1) - (3)].y_expr).val % (yyvsp[(3) - (3)].y_expr).val);
				(yyval.y_expr).typ = combine((yyvsp[(1) - (3)].y_expr).typ, (yyvsp[(3) - (3)].y_expr).typ, 0);
			;}
    break;

  case 65:
#line 990 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = (yyvsp[(2) - (2)].y_expr).val;
				(yyval.y_expr).typ = combine(S_ABS, (yyvsp[(2) - (2)].y_expr).typ, 0);
			;}
    break;

  case 66:
#line 994 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = -(yyvsp[(2) - (2)].y_expr).val;
				(yyval.y_expr).typ = combine(S_ABS, (yyvsp[(2) - (2)].y_expr).typ, 0);
			;}
    break;

  case 67:
#line 998 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = ~(yyvsp[(2) - (2)].y_expr).val;
				(yyval.y_expr).typ = combine(S_ABS, (yyvsp[(2) - (2)].y_expr).typ, 0);
			;}
    break;

  case 68:
#line 1002 "/tmp/cc-as-comm2.i"
    {	(yyval.y_expr).val = DOTVAL;
				(yyval.y_expr).typ = DOTTYP| 0x0400 ;
			;}
    break;

  case 71:
#line 1010 "/tmp/cc-as-comm2.i"
    {	if (((yyvsp[(1) - (1)].y_expr).typ & ~S_EXT) != S_ABS)
					serror("must be absolute");
				(yyval.y_valu) = (yyvsp[(1) - (1)].y_expr).val;
			;}
    break;

  case 72:
#line 1017 "/tmp/cc-as-comm2.i"
    {	(yyval.y_valu) = 0;;}
    break;

  case 73:
#line 1019 "/tmp/cc-as-comm2.i"
    {	(yyval.y_valu) = (yyvsp[(1) - (1)].y_valu);;}
    break;

  case 74:
#line 1023 "/tmp/cc-as-comm2.i"
    {	(yyval.y_valu) = 0;;}
    break;

  case 75:
#line 1025 "/tmp/cc-as-comm2.i"
    {	(yyval.y_valu) = (yyvsp[(2) - (2)].y_valu);;}
    break;

  case 76:
#line 1042 "/tmp/cc-as-comm2.i"
    { curr_instr = curr_token; ;}
    break;

  case 78:
#line 1048 "/tmp/cc-as-comm2.i"
    {	emit2((yyvsp[(1) - (4)].y_word) | (yyvsp[(2) - (4)].y_word) | (yyvsp[(4) - (4)].y_word)<<9);;}
    break;

  case 79:
#line 1050 "/tmp/cc-as-comm2.i"
    {	emit2((yyvsp[(1) - (10)].y_word) | (yyvsp[(4) - (10)].y_word) | (yyvsp[(9) - (10)].y_word)<<9 | 010);;}
    break;

  case 80:
#line 1052 "/tmp/cc-as-comm2.i"
    {	add((yyvsp[(1) - (3)].y_word), (yyvsp[(2) - (3)].y_word));;}
    break;

  case 81:
#line 1054 "/tmp/cc-as-comm2.i"
    {	and((yyvsp[(1) - (3)].y_word), (yyvsp[(2) - (3)].y_word));;}
    break;

  case 82:
#line 1056 "/tmp/cc-as-comm2.i"
    {	shift_op((yyvsp[(1) - (3)].y_word), (yyvsp[(2) - (3)].y_word));;}
    break;

  case 83:
#line 1058 "/tmp/cc-as-comm2.i"
    {	branch((yyvsp[(1) - (2)].y_word), (yyvsp[(2) - (2)].y_expr));;}
    break;

  case 84:
#line 1060 "/tmp/cc-as-comm2.i"
    {	(yyvsp[(4) - (4)].y_expr).val -= (DOTVAL+2);
				 if (!((((((yyvsp[(4) - (4)].y_expr).val ) + 0x8000L) & ~0xFFFFL) == 0) )) Xnofit(); ;
				emit2((yyvsp[(1) - (4)].y_word) | (yyvsp[(2) - (4)].y_word));

				newrelo((yyvsp[(4) - (4)].y_expr).typ, RELPC|RELO2|RELBR|RELWR);

				emit2( ((int)((yyvsp[(4) - (4)].y_expr).val ) & 0xFFFF) );
			;}
    break;

  case 85:
#line 1069 "/tmp/cc-as-comm2.i"
    {	bitop((yyvsp[(1) - (2)].y_word));;}
    break;

  case 86:
#line 1071 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(2) - (5)].y_word) !=  0100 ) {
					serror("illegal size");
				}
				emit2((yyvsp[(1) - (5)].y_word) | mrg_2 | (yyvsp[(5) - (5)].y_word)<<9);
				ea_2( 0100 ,  0x01 );
			;}
    break;

  case 87:
#line 1078 "/tmp/cc-as-comm2.i"
    {	emit2(040700 | mrg_2 | (yyvsp[(4) - (4)].y_word)<<9);
				ea_2( 0200 ,  0x04 );
			;}
    break;

  case 88:
#line 1082 "/tmp/cc-as-comm2.i"
    {	emit2(((yyvsp[(1) - (2)].y_word)&0177700) | mrg_2);
				ea_2((yyvsp[(1) - (2)].y_word)&0300, (yyvsp[(1) - (2)].y_word)&017);
			;}
    break;

  case 89:
#line 1086 "/tmp/cc-as-comm2.i"
    {	emit2((yyvsp[(1) - (1)].y_word));;}
    break;

  case 90:
#line 1088 "/tmp/cc-as-comm2.i"
    {	cmp((yyvsp[(2) - (3)].y_word));;}
    break;

  case 91:
#line 1090 "/tmp/cc-as-comm2.i"
    {	move((yyvsp[(2) - (3)].y_word));;}
    break;

  case 92:
#line 1092 "/tmp/cc-as-comm2.i"
    {	movep((yyvsp[(2) - (3)].y_word));;}
    break;

  case 93:
#line 1094 "/tmp/cc-as-comm2.i"
    {	movem(0, (yyvsp[(2) - (5)].y_word), (yyvsp[(3) - (5)].y_word));;}
    break;

  case 94:
#line 1096 "/tmp/cc-as-comm2.i"
    {	movem(1, (yyvsp[(2) - (5)].y_word), (yyvsp[(5) - (5)].y_word));;}
    break;

  case 95:
#line 1098 "/tmp/cc-as-comm2.i"
    {	test68010();
				if (mrg_1 <= 017) {
					emit2(007000 | (yyvsp[(2) - (3)].y_word) | mrg_2);
					emit2(mrg_1 << 12 | 04000);
					ea_2((yyvsp[(2) - (3)].y_word), 0x08 | 0x02 );
				} else if (mrg_2 <= 017) {
					emit2(007000 | (yyvsp[(2) - (3)].y_word) | mrg_1);
					emit2(mrg_2 << 12);
					ea_1((yyvsp[(2) - (3)].y_word), 0x08 | 0x02 );
				} else
					badoperand();
			;}
    break;

  case 96:
#line 1111 "/tmp/cc-as-comm2.i"
    {	test68010();
				emit2(047172); emit2((yyvsp[(2) - (4)].y_word) | (yyvsp[(4) - (4)].y_word)<<12);
			;}
    break;

  case 97:
#line 1115 "/tmp/cc-as-comm2.i"
    {	test68010();
				emit2(047173); emit2((yyvsp[(4) - (4)].y_word) | (yyvsp[(2) - (4)].y_word)<<12);
			;}
    break;

  case 98:
#line 1119 "/tmp/cc-as-comm2.i"
    {	if (((yyvsp[(2) - (4)].y_word) & 010) == 0)
					emit2(
						(0140500|(yyvsp[(4) - (4)].y_word)|(yyvsp[(2) - (4)].y_word)<<9)
						+
						(((yyvsp[(4) - (4)].y_word)&010)<<3)
					);
				else
					emit2(
						(0140610|(yyvsp[(2) - (4)].y_word)|((yyvsp[(4) - (4)].y_word)&07)<<9)
						-
						(((yyvsp[(4) - (4)].y_word)&010)<<3)
					);
			;}
    break;

  case 99:
#line 1133 "/tmp/cc-as-comm2.i"
    {	checksize((yyvsp[(2) - (3)].y_word), 2|4); emit2(044000 | (yyvsp[(2) - (3)].y_word)+0100 | (yyvsp[(3) - (3)].y_word));;}
    break;

  case 100:
#line 1135 "/tmp/cc-as-comm2.i"
    {	emit2(044100 | (yyvsp[(2) - (2)].y_word));;}
    break;

  case 101:
#line 1137 "/tmp/cc-as-comm2.i"
    {	emit2((yyvsp[(1) - (2)].y_word)); ea_2( 0100 , 0);;}
    break;

  case 102:
#line 1139 "/tmp/cc-as-comm2.i"
    {	emit2(047120 | (yyvsp[(2) - (4)].y_word)); ea_2( 0100 , 0);;}
    break;

  case 103:
#line 1141 "/tmp/cc-as-comm2.i"
    {	emit2(047130 | (yyvsp[(2) - (2)].y_word));;}
    break;

  case 104:
#line 1143 "/tmp/cc-as-comm2.i"
    {	 if (!(((((yyvsp[(3) - (3)].y_valu) ) & ~((int)017)) == 0) )) Xnofit(); ; emit2(047100| ((short)((yyvsp[(3) - (3)].y_valu) ) & 017) );;}
    break;

  case 105:
#line 1145 "/tmp/cc-as-comm2.i"
    {	test68010();
				emit2(047164);
				ea_2( 0100 , 0);
			;}
    break;

  case 106:
#line 1150 "/tmp/cc-as-comm2.i"
    {	model = (yyvsp[(1) - (1)].y_word);;}
    break;

  case 109:
#line 1155 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (2)].y_word) | (yyvsp[(2) - (2)].y_word);;}
    break;

  case 111:
#line 1158 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(1) - (1)].y_word) != 075) badoperand(); (yyval.y_word) = 04000;;}
    break;

  case 113:
#line 1162 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (2)].y_word) | (yyvsp[(2) - (2)].y_word);;}
    break;

  case 115:
#line 1166 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (3)].y_word) | (yyvsp[(3) - (3)].y_word);;}
    break;

  case 116:
#line 1169 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = 1<<(yyvsp[(1) - (1)].y_word);;}
    break;

  case 117:
#line 1171 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(1) - (3)].y_word) > (yyvsp[(3) - (3)].y_word))
					badoperand();
				for ((yyval.y_word) = 0; (yyvsp[(1) - (3)].y_word) <= (yyvsp[(3) - (3)].y_word); (yyvsp[(1) - (3)].y_word)++)
					(yyval.y_word) |= (1<<(yyvsp[(1) - (3)].y_word));
			;}
    break;

  case 118:
#line 1178 "/tmp/cc-as-comm2.i"
    {	mrg_2 = (yyvsp[(1) - (1)].y_word);;}
    break;

  case 119:
#line 1180 "/tmp/cc-as-comm2.i"
    {	mrg_2 = 010 | (yyvsp[(1) - (1)].y_word);;}
    break;

  case 120:
#line 1182 "/tmp/cc-as-comm2.i"
    {	mrg_2 = (yyvsp[(1) - (1)].y_word);;}
    break;

  case 123:
#line 1188 "/tmp/cc-as-comm2.i"
    {	mrg_2 = 020 | (yyvsp[(2) - (3)].y_word);;}
    break;

  case 124:
#line 1190 "/tmp/cc-as-comm2.i"
    {	mrg_2 = 030 | (yyvsp[(2) - (4)].y_word);;}
    break;

  case 125:
#line 1192 "/tmp/cc-as-comm2.i"
    {	mrg_2 = 040 | (yyvsp[(3) - (4)].y_word);;}
    break;

  case 126:
#line 1194 "/tmp/cc-as-comm2.i"
    {	exp_2 = (yyvsp[(1) - (2)].y_expr); ea707172((yyvsp[(2) - (2)].y_word));
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			;}
    break;

  case 127:
#line 1198 "/tmp/cc-as-comm2.i"
    {	exp_2 = (yyvsp[(1) - (5)].y_expr); ea5x73((yyvsp[(3) - (5)].y_word), (yyvsp[(4) - (5)].y_word));
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			;}
    break;

  case 128:
#line 1202 "/tmp/cc-as-comm2.i"
    {	exp_2 = (yyvsp[(1) - (7)].y_expr); ea6x((yyvsp[(3) - (7)].y_word), (yyvsp[(5) - (7)].y_word), (yyvsp[(6) - (7)].y_word));
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			;}
    break;

  case 129:
#line 1206 "/tmp/cc-as-comm2.i"
    {	exp_2 = (yyvsp[(1) - (4)].y_expr); ea72();
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			;}
    break;

  case 130:
#line 1210 "/tmp/cc-as-comm2.i"
    {	exp_2 = (yyvsp[(1) - (7)].y_expr); ea73((yyvsp[(5) - (7)].y_word), (yyvsp[(6) - (7)].y_word));
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			;}
    break;

  case 131:
#line 1215 "/tmp/cc-as-comm2.i"
    {	mrg_2 = 074; exp_2 = (yyvsp[(2) - (2)].y_expr);
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			;}
    break;

  case 133:
#line 1221 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (1)].y_word) | 010;;}
    break;

  case 134:
#line 1224 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) =   0100  ;;}
    break;

  case 136:
#line 1228 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) =  0300 ;;}
    break;

  case 138:
#line 1232 "/tmp/cc-as-comm2.i"
    {	mrg_1 = mrg_2; exp_1 = exp_2;
				 {rel_1  =  rel_2 ;  rel_2  = 0;} ;
			;}
    break;

  case 140:
#line 1238 "/tmp/cc-as-comm2.i"
    {	co_id = (yyvsp[(1) - (1)].y_word); ;}
    break;

  case 142:
#line 1240 "/tmp/cc-as-comm2.i"
    {	co_id =  01000 ; ;}
    break;

  case 144:
#line 1244 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (5)].y_word),  00 );
				if ((mrg_2&070) == 010 && (yyvsp[(5) - (5)].y_word) != 001)
					badoperand();
				emit2((0170000|co_id|mrg_2));
				emit2((0100000|((yyvsp[(5) - (5)].y_word)<<10)));
				ea_2( 0200 , 0);
			;}
    break;

  case 145:
#line 1252 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (5)].y_word),  00 );
				if ((mrg_2&070) == 010 && (yyvsp[(3) - (5)].y_word) == 001)
					badoperand();
				emit2((0170000|co_id|mrg_2));
				emit2((0120000|((yyvsp[(3) - (5)].y_word)<<10)));
				ea_2( 0200 ,  0x08 );
			;}
    break;

  case 146:
#line 1260 "/tmp/cc-as-comm2.i"
    {	emit2(0170000|co_id);
				emit2(((yyvsp[(3) - (5)].y_word)<<10)|((yyvsp[(5) - (5)].y_word)<<7));
			;}
    break;

  case 147:
#line 1264 "/tmp/cc-as-comm2.i"
    {	ch_sz_dreg((yyvsp[(2) - (5)].y_word), mrg_2&070);
				emit2((0170000|co_id|mrg_2));
				emit2((0040000|((yyvsp[(2) - (5)].y_word)<<10)|((yyvsp[(5) - (5)].y_word)<<7)));
				ea_2( 0200 ,  0x01 );
			;}
    break;

  case 148:
#line 1270 "/tmp/cc-as-comm2.i"
    {	ch_sz_dreg((yyvsp[(2) - (5)].y_word), mrg_2&070);
				if ((yyvsp[(2) - (5)].y_word) ==  03 )
					serror("packed decimal needs k-factor");
				emit2((0170000|co_id|mrg_2));
				emit2((0060000|((yyvsp[(2) - (5)].y_word)<<10)|((yyvsp[(3) - (5)].y_word)<<7)));
				ea_2( 0200 ,  0x01 | 0x08 );
			;}
    break;

  case 149:
#line 1278 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (9)].y_word),  03 );
				 if (!(((((((yyvsp[(8) - (9)].y_valu) )+64 ) & ~((int)0177)) == 0) ) )) nofit() ;
				emit2((0170000|co_id|mrg_2));
				emit2((0066000|((yyvsp[(3) - (9)].y_word)<<7)| ((short)((yyvsp[(8) - (9)].y_valu) ) & 0177) ));
				ea_2( 0200 ,  0x02 | 0x01 | 0x08 );
			;}
    break;

  case 150:
#line 1285 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (8)].y_word),  03 );
				emit2((0170000|co_id|mrg_2));
				emit2((0076000|((yyvsp[(3) - (8)].y_word)<<7)|((yyvsp[(7) - (8)].y_word)<<4)));
				ea_2( 0200 ,  0x02 | 0x01 | 0x08 );
			;}
    break;

  case 151:
#line 1291 "/tmp/cc-as-comm2.i"
    {	 if (!(((((yyvsp[(4) - (6)].y_valu) ) & ~((int)0177)) == 0) )) nofit() ;
				check_fsize((yyvsp[(2) - (6)].y_word),  02 );
				emit2(0170000|co_id);
				emit2(056000|((yyvsp[(6) - (6)].y_word)<<7)| ((short)((yyvsp[(4) - (6)].y_valu) ) & 0177) );
			;}
    break;

  case 152:
#line 1297 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (5)].y_word),  02 );
				if ((mrg_2&070) == 030)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2(0160000 |
					(((mrg_2&070)==040 || ((yyvsp[(3) - (5)].y_word)&04000)) ?
						(yyvsp[(3) - (5)].y_word) :
						(010000|reverse((yyvsp[(3) - (5)].y_word),8))));
				ea_2( 0200 ,  0x02 | 0x08 );
			;}
    break;

  case 153:
#line 1308 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (5)].y_word),  02 );
				if ((mrg_2&070) == 040)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0150000|(((yyvsp[(5) - (5)].y_word)&04000)?(yyvsp[(5) - (5)].y_word):reverse((yyvsp[(5) - (5)].y_word),8))));
				ea_2( 0200 ,  0x02 );
			;}
    break;

  case 154:
#line 1316 "/tmp/cc-as-comm2.i"
    {	checksize((yyvsp[(2) - (5)].y_word), 4);
				if ((mrg_2&070) == 1 && (yyvsp[(3) - (5)].y_word)!= 02000)
					serror("bad addressing category");
				if ((mrg_2 & 070) == 0 &&
				    (yyvsp[(3) - (5)].y_word) != 02000 && (yyvsp[(3) - (5)].y_word) != 04000 && (yyvsp[(3) - (5)].y_word) != 010000)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0120000|(yyvsp[(3) - (5)].y_word)));
				ea_2( 0200 ,  0x08 );
			;}
    break;

  case 155:
#line 1327 "/tmp/cc-as-comm2.i"
    {	checksize((yyvsp[(2) - (5)].y_word), 4);
				if ((mrg_2&070) == 1 && (yyvsp[(5) - (5)].y_word)!= 02000)
					serror("bad addressing category");
				if ((mrg_2 & 070) == 0 &&
				    (yyvsp[(5) - (5)].y_word) != 02000 && (yyvsp[(5) - (5)].y_word) != 04000 && (yyvsp[(5) - (5)].y_word) != 010000)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0100000|(yyvsp[(5) - (5)].y_word)));
				ea_2( 0200 , 0);
			;}
    break;

  case 156:
#line 1338 "/tmp/cc-as-comm2.i"
    {	emit2((0170000|co_id|mrg_2));
				emit2((0040000|((yyvsp[(2) - (5)].y_word)<<10)|((yyvsp[(5) - (5)].y_word)<<7)|(yyvsp[(1) - (5)].y_word)));
				ch_sz_dreg((yyvsp[(2) - (5)].y_word), mrg_2&070);
				ea_2( 0200 ,  0x01 );
			;}
    break;

  case 157:
#line 1344 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (5)].y_word),  02 );
				emit2(0170000|co_id);
				emit2(((yyvsp[(3) - (5)].y_word)<<10)|((yyvsp[(5) - (5)].y_word)<<7)|(yyvsp[(1) - (5)].y_word));
			;}
    break;

  case 158:
#line 1349 "/tmp/cc-as-comm2.i"
    {	emit2((0170000|co_id|mrg_2));
				emit2((0040000|((yyvsp[(2) - (5)].y_word)<<10)|((yyvsp[(5) - (5)].y_word)<<7)|(yyvsp[(1) - (5)].y_word)));
				ch_sz_dreg((yyvsp[(2) - (5)].y_word), mrg_2&070);
				ea_2( 0200 ,  0x01 );
			;}
    break;

  case 159:
#line 1355 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (5)].y_word),  02 );
				emit2(0170000|co_id);
				emit2(((yyvsp[(3) - (5)].y_word)<<10)|((yyvsp[(5) - (5)].y_word)<<7)|(yyvsp[(1) - (5)].y_word));
			;}
    break;

  case 160:
#line 1360 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (3)].y_word),  02 );
				emit2(0170000|co_id);
				emit2(((yyvsp[(3) - (3)].y_word)<<10)|((yyvsp[(3) - (3)].y_word)<<7)|(yyvsp[(1) - (3)].y_word));
			;}
    break;

  case 161:
#line 1365 "/tmp/cc-as-comm2.i"
    {	emit2(0170000|co_id|mrg_2);
				emit2(0040000|((yyvsp[(2) - (7)].y_word)<<10)|((yyvsp[(7) - (7)].y_word)<<7)|(yyvsp[(1) - (7)].y_word)|(yyvsp[(5) - (7)].y_word));
				ea_2( 0200 ,  0x01 );
			;}
    break;

  case 162:
#line 1370 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (7)].y_word),  02 );
				emit2(0170000|co_id);
				emit2(((yyvsp[(3) - (7)].y_word)<<10)|((yyvsp[(7) - (7)].y_word)<<7)|(yyvsp[(1) - (7)].y_word)|(yyvsp[(5) - (7)].y_word));
			;}
    break;

  case 163:
#line 1375 "/tmp/cc-as-comm2.i"
    {	fbranch((yyvsp[(1) - (2)].y_word), (yyvsp[(2) - (2)].y_expr));;}
    break;

  case 164:
#line 1377 "/tmp/cc-as-comm2.i"
    {	emit2(0170110|co_id|(yyvsp[(2) - (4)].y_word));
				emit2((yyvsp[(1) - (4)].y_word));
				(yyvsp[(4) - (4)].y_expr).val -= DOTVAL;
				 if (!((((((yyvsp[(4) - (4)].y_expr).val ) + 0x8000L) & ~0xFFFFL) == 0) )) nofit() ;

				newrelo((yyvsp[(4) - (4)].y_expr).typ, RELPC|RELO2|RELBR|RELWR);

				emit2( ((int)((yyvsp[(4) - (4)].y_expr).val ) & 0xFFFF) );
			;}
    break;

  case 165:
#line 1387 "/tmp/cc-as-comm2.i"
    {	emit2(0170200|co_id);
				emit2(0);
			;}
    break;

  case 166:
#line 1391 "/tmp/cc-as-comm2.i"
    {	emit2(0170100|co_id|mrg_2);
				emit2((yyvsp[(1) - (2)].y_word));
				ea_2( 0000 ,  0x01 | 0x08 );
			;}
    break;

  case 167:
#line 1396 "/tmp/cc-as-comm2.i"
    {	emit2((0170000|co_id|mrg_2));
				emit2((0040072|((yyvsp[(2) - (3)].y_word)<<10)));
				ch_sz_dreg((yyvsp[(2) - (3)].y_word), mrg_2&070);
				ea_2( 0200 ,  0x01 );
			;}
    break;

  case 168:
#line 1402 "/tmp/cc-as-comm2.i"
    {	check_fsize((yyvsp[(2) - (3)].y_word),  02 );
				emit2(0170000|co_id);
				emit2(((yyvsp[(3) - (3)].y_word)<<10)|072);
			;}
    break;

  case 169:
#line 1407 "/tmp/cc-as-comm2.i"
    {	if ((mrg_2&070) == ((yyvsp[(1) - (2)].y_word)&070))
					badoperand();
				emit2((0170000|co_id|((yyvsp[(1) - (2)].y_word)&0700)|mrg_2));
				ea_2(0, (yyvsp[(1) - (2)].y_word)&07);
			;}
    break;

  case 170:
#line 1413 "/tmp/cc-as-comm2.i"
    {	emit2(0170174|co_id);
				emit2((yyvsp[(1) - (1)].y_word));
			;}
    break;

  case 171:
#line 1417 "/tmp/cc-as-comm2.i"
    {	checksize((yyvsp[(2) - (3)].y_word), 2|4);
				emit2((0170170|co_id|((yyvsp[(2) - (3)].y_word)== 0200 ?03:02)));
				emit2((yyvsp[(1) - (3)].y_word));
				ea_2((yyvsp[(2) - (3)].y_word),0);
			;}
    break;

  case 172:
#line 1424 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = 04000 | (yyvsp[(1) - (1)].y_word) << 4; ;}
    break;

  case 175:
#line 1429 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (3)].y_word) | (yyvsp[(3) - (3)].y_word);;}
    break;

  case 176:
#line 1432 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = 1 << (yyvsp[(1) - (1)].y_word); ;}
    break;

  case 177:
#line 1434 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(1) - (3)].y_word) > (yyvsp[(3) - (3)].y_word))
					badoperand();
				for ((yyval.y_word) = 0; (yyvsp[(1) - (3)].y_word) <= (yyvsp[(3) - (3)].y_word); (yyvsp[(1) - (3)].y_word)++)
					(yyval.y_word) |= (1 << (yyvsp[(1) - (3)].y_word));
			;}
    break;

  case 178:
#line 1441 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (1)].y_word) << 10; ;}
    break;

  case 179:
#line 1443 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) = (yyvsp[(1) - (3)].y_word) | ((yyvsp[(3) - (3)].y_word) << 10); ;}
    break;

  case 180:
#line 1446 "/tmp/cc-as-comm2.i"
    {	(yyval.y_word) =  02 ; ;}
    break;

  case 181:
#line 1448 "/tmp/cc-as-comm2.i"
    {	if ((yyvsp[(1) - (1)].y_word) ==  0200 )
					(yyval.y_word) =  00 ;
				else if ((yyvsp[(1) - (1)].y_word) ==  0100 )
					(yyval.y_word) =  04 ;
				else	(yyval.y_word) =  06 ;
			;}
    break;


/* Line 1267 of yacc.c.  */
#line 3626 "cc/as/generated/comm2.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1457 "/tmp/cc-as-comm2.i"
