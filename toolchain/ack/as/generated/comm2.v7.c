
# line 14 "comm2.y"











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

# line 334 "comm2.y"
typedef union  {
	 short 	y_word;
	 int64_t 	y_valu;
	expr_t	y_expr;
	item_t	*y_item;
} YYSTYPE;
# define STRING 257
# define IDENT 258
# define FBSYM 259
# define CODE1 260
# define CODE2 261
# define CODE4 262
# define NUMBER0 263
# define NUMBER1 264
# define NUMBER2 265
# define NUMBER3 266
# define NUMBER4 267
# define NUMBER8 268
# define NUMBERF 269
# define DOT 270
# define EXTERN 271
# define DATA 272
# define DATA8 273
# define DATAF 274
# define ASCII 275
# define SECTION 276
# define COMMON 277
# define BASE 278
# define SYMB 279
# define SYMD 280
# define ALIGN 281
# define ASSERT 282
# define SPACE 283
# define SEEK 284
# define LINE 285
# define FILe 286
# define LIST 287
# define OP_OO 288
# define OP_AA 289
# define OP_EQ 290
# define OP_NE 291
# define OP_LE 292
# define OP_GE 293
# define OP_LL 294
# define OP_RR 295
# define SIZE 296
# define DREG 297
# define AREG 298
# define PC 299
# define CREG 300
# define SPEC 301
# define ABCD 302
# define ADDX 303
# define ADD 304
# define AND 305
# define BITOP 306
# define SHIFT 307
# define SZ_EA 308
# define OP_EA 309
# define OP_NOOP 310
# define OP_EA_D 311
# define LEA 312
# define DBR 313
# define BR 314
# define CMP 315
# define MOVE 316
# define MOVEP 317
# define MOVEM 318
# define MOVEC 319
# define MOVES 320
# define SWAP 321
# define LINK 322
# define UNLK 323
# define TRAP 324
# define STOP 325
# define EXG 326
# define OP_EXT 327
# define RTD 328
# define MODEL 329
# define FPCR 330
# define FPREG 331
# define FMOVE 332
# define FMOVECR 333
# define FMOVEM 334
# define FDYADIC 335
# define FMONADIC 336
# define FSINCOS 337
# define FBCC 338
# define FDBCC 339
# define FNOP 340
# define FSCC 341
# define FTST 342
# define FSAVRES 343
# define FTRAPCC 344
# define FSIZE 345
# define CP 346
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 1236 "comm2.y"


short yyexca[] ={
-1, 1,
	0, -1,
	302, 75,
	303, 75,
	304, 75,
	305, 75,
	306, 75,
	307, 75,
	308, 75,
	309, 75,
	310, 75,
	311, 75,
	312, 75,
	313, 75,
	314, 75,
	315, 75,
	316, 75,
	317, 75,
	318, 75,
	319, 75,
	320, 75,
	321, 75,
	322, 75,
	323, 75,
	324, 75,
	325, 75,
	326, 75,
	327, 75,
	328, 75,
	329, 75,
	332, 75,
	333, 75,
	334, 75,
	335, 75,
	336, 75,
	337, 75,
	338, 75,
	339, 75,
	340, 75,
	341, 75,
	342, 75,
	343, 75,
	344, 75,
	346, 75,
	59, 11,
	10, 11,
	-2, 0,
	};
# define YYNPROD 182
# define YYLAST 980
short yyact[]={

 154, 295, 138, 252, 150, 297, 293, 350, 351, 382,
 381, 291, 373, 368,  38,  50, 367, 366,  38, 256,
 365,  38,  38,  38,  62,  38, 364, 363,  54, 359,
 104, 296, 299, 361, 310, 168, 169, 383, 166, 167,
 296, 278,  37, 273, 125, 126, 127, 128, 166, 167,
  59,  60, 253,  64, 182, 183, 184, 185, 186, 187,
 188, 189, 190, 191, 192, 193, 194, 269, 255, 146,
 168, 275, 276, 168, 169, 379, 143, 142, 229, 175,
 174, 156, 321, 268, 137, 159, 261, 161, 162, 163,
 124, 247, 171, 172, 144, 122, 120, 139, 121, 242,
 123, 141, 178, 265, 179, 241,  68,  38, 197, 198,
 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
 209, 210, 211, 212, 213, 214, 135,  33, 239, 218,
 216,  38,  57, 219,  38, 196,  36,  35, 124, 111,
 105,  69,  55, 122, 120,  52, 121,  51, 123, 157,
 158,  66, 387, 385, 127, 372, 233, 164, 181, 217,
  32, 114, 220, 115, 375,  28, 155, 240,  29, 374,
  30, 152, 335, 124,  44, 124, 153,  38, 122, 120,
 122, 121, 280, 123, 251, 123, 332, 165, 170, 260,
 369, 333, 282, 151, 140, 110, 355, 334, 124, 111,
 335, 346, 155, 122, 120, 249, 121, 152, 123,  31,
  44, 319, 153, 279, 320, 311, 280, 342, 341, 340,
 339, 114,  43, 115, 338, 109, 234, 337, 270,  98,
  99,  72,  73,  77,  74, 101, 100,  81,  78,  79,
  76,  75,  82,  83,  84,  85,  87,  86,  90,  92,
  93,  94,  91,  88,  89,  95,  96,  46,  43, 155,
 292, 262, 336, 264, 152, 331, 155,  44,  38, 153,
 330, 152, 328, 102,  44, 315, 153, 327, 160, 326,
 318, 306, 155, 281, 322, 173, 221, 152, 277, 177,
  44,  38, 153,  46, 267, 124, 312, 248, 246, 245,
 122, 120, 386, 121, 244, 123, 235, 343, 228, 227,
 222, 134,  38, 133, 132,  43, 131, 130, 114, 329,
 115, 287,  43, 129, 106, 378, 298, 300, 302, 304,
  67, 377, 274, 307, 358, 356, 136, 380,  43, 250,
 345, 314, 362, 357, 283, 344, 285, 119, 118, 231,
  46, 370, 371, 155, 347, 316, 313,  46, 152, 155,
 272,  44, 376, 153, 152, 223, 155,  44, 254, 153,
 257, 258, 259,  46, 290, 176, 195, 263,  34,  53,
  38, 103, 325, 180, 271,  97, 323,  39,  42,  48,
  49, 112, 113, 116, 117, 119, 118, 352, 354,  40,
  58,  47,  70, 284, 360,  27,  65,  63, 384,  43,
 324,  61,  56, 266,   7,  43, 152,   1, 348,  44,
 349, 153, 294,  39,  42,  48,  49,  80, 147, 148,
  71,  41, 149,   0,   0,  40,   0,  47,   0,   0,
   0,   0, 286,   0,  46,   0,   0,   0,   0,   0,
  46, 112, 113, 116, 117, 119, 118,   0,   0, 309,
   0, 288, 289,   0, 147, 148,   0,  43, 149, 317,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  39,  42,  48,  49,   0,   0,   0,  39,  42,  48,
  49,   0,  40,   0,  47,   0,   0,   0, 353,  40,
   0,  47,  46,  39,  42,  48,  49,   0, 155, 152,
   0,   0,  44, 152, 153,  40,  44,  47, 153,   0,
   0, 147, 148,   0,   0, 149,   0,   0, 147, 148,
   0,   0, 149, 124, 111,   0, 232,   0, 122, 120,
   0, 121,   0, 123, 147, 148,   0,   0, 149,   0,
 116, 117, 119, 118,   0, 308, 114,   0, 115,   0,
  43,   0, 305,   0,  43,   0,   0,   0,   0,   0,
   0,   0,   8,   0,  39,  42,  48,  49, 303,   0,
  39,  42,  48,  49,   0,   0,  40,   0,  47,   0,
 110,   0,  40,   0,  47,  46,   0, 145,   0,  46,
   0, 124, 111,   0,   0,   0, 122, 120,   0, 121,
   0, 123,   0,   0,   0, 147, 148,   0,   0, 149,
 109, 147, 148,   0, 114, 149, 115,   0,   0,   0,
   0,   0,  39,  42,  48,  49,   0, 124, 111,   0,
   0,   0, 122, 120,  40, 121,  47, 123,   0, 301,
   0,   0,   0,   0, 299,   0,   0, 215, 110,   0,
 114,   0, 115,   0, 124, 111,   0,   0,   0, 122,
 120,   0, 121, 293, 123,   0, 124, 111,   0,   0,
   0, 122, 120,   0, 121, 124, 123, 114, 109, 115,
 122, 120,   0, 121, 110, 123,   0,   0,  44, 114,
  45, 115,   0,   0,   0,   0,   0, 296, 114,   0,
 115,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 110,   0,   0, 109,  39,  42,  48,  49,  39,
  42,  48,  49, 110,   0,   0, 224,  40, 225,  47,
 226,  40,   0,  47,   0, 152,  43,   0,  44, 230,
 153, 109,  44,   0,  45,   0,   0, 236, 237, 238,
   0, 243,   0,   0,   0,   0, 168, 169,   0,   0,
 147, 148,   0,   0, 149,   0,   0,   0,   0,   0,
   0,  46,   0,   0, 107, 108, 112, 113, 116, 117,
 119, 118, 141,   9,   0,   2,  43,   4,   5,   6,
  43,   0,   0,   0,   0,   3,   0,   0,  19,  23,
  24,  25,  26,  11,  12,  13,  15,  16,  20,  14,
  21,  22,  17,  18,  10,   0,   0,   0,   0,   0,
   0,  46,   0,   0,   0,  46,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 107, 108, 112, 113, 116, 117, 119, 118,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 107, 108,
 112, 113, 116, 117, 119, 118,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  39,  42,  48,  49,   0, 108, 112, 113, 116,
 117, 119, 118,  40,   0,  47,   0,   0,   0, 112,
 113, 116, 117, 119, 118,   0,   0,   0, 112, 113,
 116, 117, 119, 118,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  39,  42,  48,  49,  39,  42,  48,  49,   0,
   0,   0,   0,  40,   0,  47,   0,  40,   0,  47 };
short yypact[]={

-1000, 537, 107, 112,-1000,-1000,-1000, 150,-141, 368,
-1000,-121,-122, 655, 655,-110,-112, 655,-115,-126,
 655, 655, 655, 655, 655,  61,-116, -73,-1000, 655,
-1000,-1000,-1000,-117,-1000,-1000, 280,-1000, 600,-1000,
-1000,-1000,-1000, 655, 655, 655, 655,-1000,-1000,-1000,
 600, 279, 273,-1000,-1000,-1000, 272,-1000,-1000,-1000,
-1000, 270, 600, 269,-1000, 267,-1000,-1000,-143,-1000,
-1000,  39,-199,-195,-199, 655,-203, 473,-199, 473,
 473,-1000,-199,-195,-199,-199,-199,-262,-224,-199,
-204, 331,-218,-219, 340, 331,-1000,-1000,-1000,-199,
-1000,-199,-1000,-278, 600, 366, 655, 655, 655, 655,
 655, 655, 655, 655, 655, 655, 655, 655, 655, 655,
 655, 655, 655, 655, 655, 564,-1000,-1000,-1000, 655,
 655,-129, 655, 655,  61,-1000, 266, 325, 473,-1000,
 473,-1000, 473, 600, 265,-1000, 264,-1000,-1000,-1000,
-1000,-1000,-220, 709, 496, 655, 473, 262,-1000, 473,
 473, 473, 469, 473, 260, 255,-1000,-1000,-1000,-1000,
 254,-206,-1000,-1000, 253,-1000, 655,-1000,-1000,-1000,
-278,-1000,-293,-293,-277,-293,-293,-293, 655,-211,
-1000, 473,-293, 473,-193,-1000,-1000, 627, 101, 639,
 161, 648, 258, 258,  53,  53,  53,  53, 136, 136,
 138, 138,-1000,-1000,-1000,-1000, 600, 250,-1000, 600,
-1000,-1000,-214,-231,-1000,-1000,-1000, 655,-1000, 319,
-255,-1000,-227, 600, 244,-257,-1000,-1000,-1000, 169,
 239,-1000, 147,-1000,-224,-252,-224,-1000, 331,-1000,
-1000, 131,-1000,-1000, 339, 376, 324, 318, 247, 231,
 600, 237,-1000, 224,-1000, 331, 171, 655,-1000, 315,
 600, 473, 232, 314,-195, 236, 170,-215,-1000, 705,
-224,-224,-224,-1000,-1000,-1000,-1000, 235, 233, 228,
 655, 226, 221,-1000, 139,-1000, 146, 153, 218,-1000,
 183, 180, 176, 175, 174, 173, 655,-1000,-1000,-1000,
 171, 655,-1000, 157,-1000,-1000,-1000, 313,-224,-1000,
-224,-1000,-1000,-1000, 135,-1000,-323, 473, 167, 152,
 705,-291,-300,-302, 473,-297,-298,-304,-305,-311,
-314,-315,-318, 600,-1000,-1000, 145,-1000,-199,-199,
-1000,-1000,-1000,-1000,  32,-319,-1000,-1000,-1000,-1000,
-1000,-1000, 125,-1000,-1000,-1000,-1000, 111, 106, 322,
 290, 284,  40,-1000,-321,-322,-261,-1000,-1000, 655,
  28,-1000,-1000, 261,  27,-1000,-1000,-1000 };
short yypgo[]={

   0,  28, 379,  34,   0, 431, 430, 427, 128, 105,
  99,   2, 194, 157, 184,  11,   5, 422,   1, 417,
 414, 413, 412, 411, 407, 406, 151, 405, 402, 597,
  69,   4, 193, 385, 384, 383, 158, 381 };
short yyr1[]={

   0,  19,  19,  19,  19,  19,  19,  19,  19,  19,
  19,  20,  20,  20,  20,  20,  20,  20,  21,  20,
  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
  20,  22,  22,  23,  23,  24,  24,  26,  26,  25,
  25,   4,   4,   4,   4,   4,   4,   4,   4,   4,
   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
   4,   4,   4,   4,   4,   4,   4,   4,   5,   5,
   1,   2,   2,   3,   3,  27,  20,  28,  28,  28,
  28,  28,  28,  28,  28,  28,  28,  28,  28,  28,
  28,  28,  28,  28,  28,  28,  28,  28,  28,  28,
  28,  28,  28,  28,  28,  28,  28,   6,   6,  13,
  13,   7,   7,   8,   8,   9,   9,  30,  30,  30,
  30,  30,  31,  31,  31,  31,  31,  31,  31,  31,
  32,  10,  10,  11,  11,  12,  12,  34,  29,  35,
  33,  37,  33,  36,  36,  36,  36,  36,  36,  36,
  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
  36,  15,  15,  17,  17,  18,  18,  16,  16,  14,
  14,  14 };
short yyr2[]={

   0,   0,   3,   3,   2,   2,   2,   3,   3,   5,
   3,   0,   3,   1,   2,   4,   2,   2,   0,   7,
   6,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   1,   3,   1,   3,   1,   3,   1,   2,   1,
   3,   1,   1,   1,   1,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   2,   2,   2,   1,   1,   1,
   1,   0,   1,   0,   2,   0,   2,   4,  10,   3,
   3,   3,   2,   4,   2,   5,   4,   2,   1,   3,
   3,   3,   5,   5,   3,   4,   4,   4,   3,   2,
   2,   4,   2,   3,   2,   1,   1,   1,   2,   1,
   1,   1,   2,   1,   3,   1,   3,   1,   1,   1,
   1,   1,   3,   4,   4,   2,   5,   7,   4,   7,
   2,   1,   1,   0,   1,   0,   1,   0,   4,   0,
   3,   0,   2,   5,   5,   5,   5,   5,   9,   8,
   6,   5,   5,   5,   5,   5,   5,   5,   5,   3,
   7,   7,   2,   4,   1,   2,   3,   3,   2,   1,
   3,   1,   1,   1,   3,   1,   3,   1,   3,   0,
   1,   1 };
short yychk[]={

-1000, -19, 258, 268, 260, 261, 262, -20,  35, 256,
 287, 276, 277, 278, 282, 279, 280, 285, 286, 271,
 281, 283, 284, 272, 273, 274, 275, -27,  58,  61,
  58,  59,  10, 268,  10, 258, 258,  -1,  -4, 256,
 268,  -5, 257,  91,  43,  45, 126, 270, 258, 259,
  -4, 257, 257,  -2,  -1, 257, -22, 258,  -2,  -1,
  -1, -23,  -4, -24,  -1, -25, -26, 269,  45, 257,
 -28,  -6, 304, 305, 307, 314, 313, 306, 311, 312,
  -7, 310, 315, 316, 317, 318, 320, 319, 326, 327,
 321, 325, 322, 323, 324, 328, 329, -33, 302, 303,
 309, 308, 346, -37,  -4, 257,  44, 288, 289, 124,
  94,  38, 290, 291,  60,  62, 292, 293, 295, 294,
  43,  45,  42,  47,  37,  -4,  -4,  -4,  -4,  44,
  44,  44,  44,  44,  44, 269, 297,  45, -11, 296,
 -12, 296, -11,  -4, 297, -29, -30, 297, 298, 301,
 -31, -32,  40,  45,  -4,  35, -11, -30, -30, -11,
 -12, -11, -11, -11, -13, -10, 300, 301, 297, 298,
 -10, -11, 297, -32, 298, 298,  35, -32, -11, -11,
 -35, -36, 332, 333, 334, 335, 336, 337, 338, 339,
 340, 341, 342, 343, 344,  10,  -1,  -4,  -4,  -4,
  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,
  -4,  -4,  -4,  -4,  -4,  93,  -4,  -1, 258,  -4,
  -1, -26,  44,  40, -29, -29, -29,  44,  44, 298,
  40, -12,  40,  -4, -30,  44, -29, -29, -29,  -8,
 -31,  -9, -10, -29,  44,  44,  44, 297,  44,  -1,
 -36, -14, 296, 345, -14, 345, 296, -14, -14, -14,
  -4, 297, -30, -14, -30, 296, -21,  44, 297, 298,
  -4, -34,  41, 298, -10, 298, 299,  44, 298,  44,
  47,  44,  45, -10, -13, -10, -32, -30, 330, 331,
  35, -15, -31, 297, -17, -18, 331, -16, -30, 330,
 -30, 331, -30, 331, -30, 331,  44, -30, 331, -32,
  -3,  44,  -1,  41, -30,  43,  41, -12,  44,  41,
  44, 297, -31,  -9,  -8, -10,  44,  44,  44,  -1,
  44,  44,  47,  45,  44,  47,  44,  44,  44,  44,
  44,  44,  44,  -4,  -3,  -1,  44,  41, -10, -10,
 330, 331, -30, 331, -30,  44, -31, -15, -18, 331,
 -30, 330, -16, 331, 331, 331, 331, 331, 331,  45,
 -11, -11, 123, 331,  58,  58,  40,  41,  41,  35,
 297, 331, 331, 298,  -1, 125,  41, 125 };
short yydef[]={

   1,  -2,   0,   0,   4,   5,   6,   0,   0,   0,
  13,   0,   0,   0,   0,   0,   0,  71,   0,   0,
  71,   0,   0,   0,   0,   0,   0, 141,   2,   0,
   3,   7,   8,   0,  10,  14,   0,  16,  70,  41,
  42,  43,  44,   0,   0,   0,   0,  67,  68,  69,
  17,   0,   0,  21,  72,  22,  23,  31,  24,  25,
  26,  27,  33,  28,  35,  29,  39,  37,   0,  30,
  76,   0, 133, 135, 133,   0,   0,   0, 133,   0,
   0,  88, 133, 135, 133, 133, 133,   0,   0, 133,
   0,   0,   0,   0,   0,   0, 105, 106, 107, 133,
 111, 133, 139,   0,  12,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  64,  65,  66,   0,
   0,   0,   0,   0,   0,  38,   0,   0,   0, 134,
   0, 136,   0,  82,   0,  84,   0, 117, 118, 119,
 120, 121,   0,   0, 135,   0,   0,   0,  87,   0,
   0,   0,   0,   0,   0,   0, 109, 110, 131, 132,
   0,   0,  99, 100,   0, 102,   0, 104, 108, 112,
   0, 142, 179, 179,   0, 179, 179, 179,   0,   0,
 164,   0, 179,   0, 169,   9,  15,  46,  47,  48,
  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,
  59,  60,  61,  62,  63,  45,  18,   0,  32,  34,
  36,  40,   0,   0,  79,  80,  81,   0, 137,   0,
   0, 125,   0, 130,   0,   0,  89,  90,  91,   0,
   0, 113, 115,  94,   0,   0,   0,  98,   0, 103,
 140,   0, 180, 181,   0,   0,   0,   0,   0,   0,
 162,   0, 165,   0, 168,   0,  73,   0,  77,   0,
  83,   0, 122,   0, 135, 132,   0,   0,  86,   0,
   0,   0,   0,  95,  96,  97, 101,   0,   0,   0,
   0,   0,   0, 171, 172, 173, 175,   0,   0, 177,
   0,   0,   0, 159,   0,   0,   0, 166, 167, 170,
  73,   0,  20,   0, 138, 123, 124,   0,   0, 128,
   0,  85,  92, 114,  93, 116,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 163,  19,  74,   0, 126, 133, 133,
 143, 146, 144, 145, 147,   0, 151, 152, 174, 176,
 153, 178, 154, 155, 156, 157, 158,   0,   0,   0,
   0,   0,   0, 150,   0,   0,   0, 127, 129,   0,
   0, 160, 161,   0,   0, 149,  78, 148 };
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

int yydebug = 0; /* 1 for debugging */
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

int yyparse(void) {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

			if( yydebug ) printf( "error recovery discards char %d\n", yychar );

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

		if( yydebug ) printf("reduce %d\n",yyn);
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 2:
# line 459 "comm2.y"
{	newident(yypvt[-1].y_item, DOTTYP); newlabel(yypvt[-1].y_item);} break;
case 3:
# line 461 "comm2.y"
{	if (yypvt[-1].y_valu < 0 || yypvt[-1].y_valu > 9) {
					serror("bad f/b label");
					yypvt[-1].y_valu = 0;
				}
				newlabel(fb_shift((int)yypvt[-1].y_valu));
			} break;
case 4:
# line 468 "comm2.y"
{	emit1((int)yypvt[-0].y_valu);  if (listflag) listline(0 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;} break;
case 5:
# line 470 "comm2.y"
{	emit2((int)yypvt[-0].y_valu);  if (listflag) listline(0 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;} break;
case 6:
# line 472 "comm2.y"
{	emit4((long)yypvt[-0].y_valu);  if (listflag) listline(0 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;} break;
case 8:
# line 475 "comm2.y"
{	lineno++;  if (listflag) listline(1 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;  assert(relonami == 0) ;} break;
case 9:
# line 477 "comm2.y"
{	lineno = yypvt[-2].y_valu;
				if (modulename) strncpy(modulename, stringbuf,  200 -1);
				 if (listflag) listline(1 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;  assert(relonami == 0) ;
			} break;
case 10:
# line 482 "comm2.y"
{	serror("syntax error"); yyerrok;
				lineno++;  if (listflag) listline(1 ); else if (listtemp) { listflag = listtemp; listeoln = 1; } ;  assert(relonami == 0) ;
			} break;
case 12:
# line 491 "comm2.y"
{

				if (listflag & 1)
					listcolm += printx( 8 , yypvt[-0].y_expr.val);

				newequate(yypvt[-2].y_item, yypvt[-0].y_expr.typ);
				store(yypvt[-2].y_item, yypvt[-0].y_expr.val);
			} break;
case 13:
# line 501 "comm2.y"
{	if (yypvt[-0].y_word)
					listtemp = listmode;
				else if ((dflag & 01000) == 0)
					listtemp = 0;
			} break;
case 14:
# line 508 "comm2.y"
{	newsect(yypvt[-0].y_item);} break;
case 15:
# line 510 "comm2.y"
{	newcomm(yypvt[-2].y_item, yypvt[-0].y_valu);} break;
case 16:
# line 512 "comm2.y"
{	if (pass ==  0 ) newbase(yypvt[-0].y_valu);} break;
case 17:
# line 514 "comm2.y"
{	if (yypvt[-0].y_expr.val == 0 && pass ==  2 )
					warning("assertion failed");
			} break;
case 18:
# line 517 "comm2.y"
{ o_it = last_it; } break;
case 19:
# line 519 "comm2.y"
{	if ((sflag &  010 ) &&  (pass !=  0 ) ) {

					if (
						pass ==  2
						&&
						(yypvt[-3].y_expr.typ & S_TYP) == S_UND
					   ) {
						serror("expression undefined");
						relonami = -1;
					}
					if (
						 (pass !=  0 )
						&&
						(yypvt[-3].y_expr.typ & S_COM)
					   ) {
						




						yypvt[-3].y_expr.typ = S_CRS;
						yypvt[-3].y_expr.val = new_string(o_it->i_name);
						relonami = 0;
					}


					newsymb(
						*stringbuf ? stringbuf : (char *) 0,
						(short)(
							(yypvt[-3].y_expr.typ & (S_EXT|S_TYP))
							|
							((unsigned short)yypvt[-1].y_valu<<8)
						),
						(short)yypvt[-0].y_valu,
						yypvt[-3].y_expr.val
					);
				}
			} break;
case 20:
# line 558 "comm2.y"
{	if ((sflag &  010 ) &&  (pass !=  0 ) ) {
					newsymb(
						*stringbuf ? stringbuf : (char *) 0,
						(short)(
							(DOTTYP & (S_EXT|S_TYP))
							|
							((unsigned short)yypvt[-2].y_valu<<8)
						),
						(short)yypvt[-0].y_valu,
						( int64_t )DOTVAL
					);
				}
			} break;
case 21:
# line 572 "comm2.y"
{	if ((sflag &  020 ) &&  (pass !=  0 ) ) {
					if (yypvt[-0].y_valu)
						hllino = (short)yypvt[-0].y_valu;
					else
						hllino++;
					newsymb(
						(char *)0,
						(DOTTYP | S_LIN),
						hllino,
						( int64_t )DOTVAL
					);
				}
			} break;
case 22:
# line 586 "comm2.y"
{	if ((sflag &  020 ) &&  (pass !=  0 ) ) {
					hllino = 0;
					newsymb(
						stringbuf,
						(DOTTYP | S_FIL),
						0,
						( int64_t )DOTVAL
					);
				}
			} break;
case 24:
# line 598 "comm2.y"
{	align(yypvt[-0].y_valu);} break;
case 25:
# line 600 "comm2.y"
{	if (DOTSCT == NULL)
					nosect();
				DOTVAL += yypvt[-0].y_valu;
				DOTSCT->s_zero += yypvt[-0].y_valu;
			} break;
case 26:
# line 606 "comm2.y"
{	if (DOTSCT == NULL)
					nosect();
				if (yypvt[-0].y_valu < DOTVAL)
					serror("cannot move location counter backwards");
				if (pass ==  0 )
					DOTSCT->s_flag |=  2 ;
				DOTSCT->s_zero += yypvt[-0].y_valu - DOTVAL;
				DOTVAL = yypvt[-0].y_valu;
			} break;
case 30:
# line 619 "comm2.y"
{	emitstr(yypvt[-1].y_word);} break;
case 31:
# line 623 "comm2.y"
{	yypvt[-0].y_item->i_type |= S_EXT;} break;
case 32:
# line 625 "comm2.y"
{	yypvt[-0].y_item->i_type |= S_EXT;} break;
case 33:
# line 629 "comm2.y"
{

				if ( 1  != 0 &&  (pass !=  0 ) )
					newrelo(yypvt[-0].y_expr.typ, (int)yypvt[-1].y_word| (RELBR|RELWR) );

				emitx(yypvt[-0].y_expr.val, (int)yypvt[-1].y_word);
			} break;
case 34:
# line 637 "comm2.y"
{

				if ( 1  != 0 &&  (pass !=  0 ) )
					newrelo(yypvt[-0].y_expr.typ, (int)yypvt[-3].y_word| (RELBR|RELWR) );

				emitx(yypvt[-0].y_expr.val, (int)yypvt[-3].y_word);
			} break;
case 35:
# line 648 "comm2.y"
{	emit8(yypvt[-0].y_valu);} break;
case 36:
# line 650 "comm2.y"
{	emit8(yypvt[-0].y_valu);} break;
case 37:
# line 655 "comm2.y"
{
				emitf((int)yypvt[-2].y_word, 0);
			} break;
case 38:
# line 659 "comm2.y"
{
				emitf((int)yypvt[-3].y_word, 1);
			} break;
case 41:
# line 670 "comm2.y"
{	serror("expr syntax err");
				yyval.y_expr.val = 0; yyval.y_expr.typ = S_UND;
			} break;
case 42:
# line 674 "comm2.y"
{	yyval.y_expr.val = yypvt[-0].y_valu;
				yyval.y_expr.typ = S_ABS;
			} break;
case 43:
# line 678 "comm2.y"
{	yyval.y_expr.val = load(yypvt[-0].y_item);
				last_it = yypvt[-0].y_item;
				yyval.y_expr.typ = yypvt[-0].y_item->i_type & ~S_EXT;
			} break;
case 44:
# line 683 "comm2.y"
{	if (stringlen != 1)
					serror("too many chars");
				yyval.y_expr.val = stringbuf[0];
				yyval.y_expr.typ = S_ABS;
			} break;
case 45:
# line 689 "comm2.y"
{	yyval.y_expr = yypvt[-1].y_expr;} break;
case 46:
# line 691 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val || yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 47:
# line 695 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val && yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 48:
# line 699 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val | yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 49:
# line 703 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val ^ yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 50:
# line 707 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val & yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 51:
# line 711 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val == yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '>');
			} break;
case 52:
# line 715 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val != yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '>');
			} break;
case 53:
# line 719 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val < yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '>');
			} break;
case 54:
# line 723 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val > yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '>');
			} break;
case 55:
# line 727 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val <= yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '>');
			} break;
case 56:
# line 731 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val >= yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '>');
			} break;
case 57:
# line 735 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val >> yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 58:
# line 739 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val << yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 59:
# line 743 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val + yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '+');
			} break;
case 60:
# line 747 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val - yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, '-');
			} break;
case 61:
# line 751 "comm2.y"
{	yyval.y_expr.val = (yypvt[-2].y_expr.val * yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 62:
# line 755 "comm2.y"
{	if (yypvt[-0].y_expr.val == 0) {
					if (pass ==  2 )
						serror("divide by zero");
					yyval.y_expr.val = 0;
				} else
					yyval.y_expr.val = (yypvt[-2].y_expr.val / yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 63:
# line 764 "comm2.y"
{	if (yypvt[-0].y_expr.val == 0) {
					if (pass ==  2 )
						serror("divide by zero");
					yyval.y_expr.val = 0;
				} else
					yyval.y_expr.val = (yypvt[-2].y_expr.val % yypvt[-0].y_expr.val);
				yyval.y_expr.typ = combine(yypvt[-2].y_expr.typ, yypvt[-0].y_expr.typ, 0);
			} break;
case 64:
# line 773 "comm2.y"
{	yyval.y_expr.val = yypvt[-0].y_expr.val;
				yyval.y_expr.typ = combine(S_ABS, yypvt[-0].y_expr.typ, 0);
			} break;
case 65:
# line 777 "comm2.y"
{	yyval.y_expr.val = -yypvt[-0].y_expr.val;
				yyval.y_expr.typ = combine(S_ABS, yypvt[-0].y_expr.typ, 0);
			} break;
case 66:
# line 781 "comm2.y"
{	yyval.y_expr.val = ~yypvt[-0].y_expr.val;
				yyval.y_expr.typ = combine(S_ABS, yypvt[-0].y_expr.typ, 0);
			} break;
case 67:
# line 785 "comm2.y"
{	yyval.y_expr.val = DOTVAL;
				yyval.y_expr.typ = DOTTYP| 0x0400 ;
			} break;
case 70:
# line 793 "comm2.y"
{	if ((yypvt[-0].y_expr.typ & ~S_EXT) != S_ABS)
					serror("must be absolute");
				yyval.y_valu = yypvt[-0].y_expr.val;
			} break;
case 71:
# line 800 "comm2.y"
{	yyval.y_valu = 0;} break;
case 72:
# line 802 "comm2.y"
{	yyval.y_valu = yypvt[-0].y_valu;} break;
case 73:
# line 806 "comm2.y"
{	yyval.y_valu = 0;} break;
case 74:
# line 808 "comm2.y"
{	yyval.y_valu = yypvt[-0].y_valu;} break;
case 75:
# line 821 "comm2.y"
{ curr_instr = curr_token; } break;
case 77:
# line 827 "comm2.y"
{	emit2(yypvt[-3].y_word | yypvt[-2].y_word | yypvt[-0].y_word<<9);} break;
case 78:
# line 829 "comm2.y"
{	emit2(yypvt[-9].y_word | yypvt[-6].y_word | yypvt[-1].y_word<<9 | 010);} break;
case 79:
# line 831 "comm2.y"
{	add(yypvt[-2].y_word, yypvt[-1].y_word);} break;
case 80:
# line 833 "comm2.y"
{	and(yypvt[-2].y_word, yypvt[-1].y_word);} break;
case 81:
# line 835 "comm2.y"
{	shift_op(yypvt[-2].y_word, yypvt[-1].y_word);} break;
case 82:
# line 837 "comm2.y"
{	branch(yypvt[-1].y_word, yypvt[-0].y_expr);} break;
case 83:
# line 839 "comm2.y"
{	yypvt[-0].y_expr.val -= (DOTVAL+2);
				 if (!(((((yypvt[-0].y_expr.val ) + 0x8000L) & ~0xFFFFL) == 0) )) Xnofit(); ;
				emit2(yypvt[-3].y_word | yypvt[-2].y_word);

				newrelo(yypvt[-0].y_expr.typ, RELPC|RELO2|RELBR|RELWR);

				emit2( ((int)(yypvt[-0].y_expr.val ) & 0xFFFF) );
			} break;
case 84:
# line 848 "comm2.y"
{	bitop(yypvt[-1].y_word);} break;
case 85:
# line 850 "comm2.y"
{	if (yypvt[-3].y_word !=  0100 ) {
					serror("illegal size");
				}
				emit2(yypvt[-4].y_word | mrg_2 | yypvt[-0].y_word<<9);
				ea_2( 0100 ,  0x01 );
			} break;
case 86:
# line 857 "comm2.y"
{	emit2(040700 | mrg_2 | yypvt[-0].y_word<<9);
				ea_2( 0200 ,  0x04 );
			} break;
case 87:
# line 861 "comm2.y"
{	emit2((yypvt[-1].y_word&0177700) | mrg_2);
				ea_2(yypvt[-1].y_word&0300, yypvt[-1].y_word&017);
			} break;
case 88:
# line 865 "comm2.y"
{	emit2(yypvt[-0].y_word);} break;
case 89:
# line 867 "comm2.y"
{	cmp(yypvt[-1].y_word);} break;
case 90:
# line 869 "comm2.y"
{	move(yypvt[-1].y_word);} break;
case 91:
# line 871 "comm2.y"
{	movep(yypvt[-1].y_word);} break;
case 92:
# line 873 "comm2.y"
{	movem(0, yypvt[-3].y_word, yypvt[-2].y_word);} break;
case 93:
# line 875 "comm2.y"
{	movem(1, yypvt[-3].y_word, yypvt[-0].y_word);} break;
case 94:
# line 877 "comm2.y"
{	test68010();
				if (mrg_1 <= 017) {
					emit2(007000 | yypvt[-1].y_word | mrg_2);
					emit2(mrg_1 << 12 | 04000);
					ea_2(yypvt[-1].y_word, 0x08 | 0x02 );
				} else if (mrg_2 <= 017) {
					emit2(007000 | yypvt[-1].y_word | mrg_1);
					emit2(mrg_2 << 12);
					ea_1(yypvt[-1].y_word, 0x08 | 0x02 );
				} else
					badoperand();
			} break;
case 95:
# line 890 "comm2.y"
{	test68010();
				emit2(047172); emit2(yypvt[-2].y_word | yypvt[-0].y_word<<12);
			} break;
case 96:
# line 894 "comm2.y"
{	test68010();
				emit2(047173); emit2(yypvt[-0].y_word | yypvt[-2].y_word<<12);
			} break;
case 97:
# line 898 "comm2.y"
{	if ((yypvt[-2].y_word & 010) == 0)
					emit2(
						(0140500|yypvt[-0].y_word|yypvt[-2].y_word<<9)
						+
						((yypvt[-0].y_word&010)<<3)
					);
				else
					emit2(
						(0140610|yypvt[-2].y_word|(yypvt[-0].y_word&07)<<9)
						-
						((yypvt[-0].y_word&010)<<3)
					);
			} break;
case 98:
# line 912 "comm2.y"
{	checksize(yypvt[-1].y_word, 2|4); emit2(044000 | yypvt[-1].y_word+0100 | yypvt[-0].y_word);} break;
case 99:
# line 914 "comm2.y"
{	emit2(044100 | yypvt[-0].y_word);} break;
case 100:
# line 916 "comm2.y"
{	emit2(yypvt[-1].y_word); ea_2( 0100 , 0);} break;
case 101:
# line 918 "comm2.y"
{	emit2(047120 | yypvt[-2].y_word); ea_2( 0100 , 0);} break;
case 102:
# line 920 "comm2.y"
{	emit2(047130 | yypvt[-0].y_word);} break;
case 103:
# line 922 "comm2.y"
{	 if (!((((yypvt[-0].y_valu ) & ~((int)017)) == 0) )) Xnofit(); ; emit2(047100| ((short)(yypvt[-0].y_valu ) & 017) );} break;
case 104:
# line 924 "comm2.y"
{	test68010();
				emit2(047164);
				ea_2( 0100 , 0);
			} break;
case 105:
# line 929 "comm2.y"
{	model = yypvt[-0].y_word;} break;
case 108:
# line 934 "comm2.y"
{	yyval.y_word = yypvt[-1].y_word | yypvt[-0].y_word;} break;
case 110:
# line 937 "comm2.y"
{	if (yypvt[-0].y_word != 075) badoperand(); yyval.y_word = 04000;} break;
case 112:
# line 941 "comm2.y"
{	yyval.y_word = yypvt[-1].y_word | yypvt[-0].y_word;} break;
case 114:
# line 945 "comm2.y"
{	yyval.y_word = yypvt[-2].y_word | yypvt[-0].y_word;} break;
case 115:
# line 948 "comm2.y"
{	yyval.y_word = 1<<yypvt[-0].y_word;} break;
case 116:
# line 950 "comm2.y"
{	if (yypvt[-2].y_word > yypvt[-0].y_word)
					badoperand();
				for (yyval.y_word = 0; yypvt[-2].y_word <= yypvt[-0].y_word; yypvt[-2].y_word++)
					yyval.y_word |= (1<<yypvt[-2].y_word);
			} break;
case 117:
# line 957 "comm2.y"
{	mrg_2 = yypvt[-0].y_word;} break;
case 118:
# line 959 "comm2.y"
{	mrg_2 = 010 | yypvt[-0].y_word;} break;
case 119:
# line 961 "comm2.y"
{	mrg_2 = yypvt[-0].y_word;} break;
case 122:
# line 967 "comm2.y"
{	mrg_2 = 020 | yypvt[-1].y_word;} break;
case 123:
# line 969 "comm2.y"
{	mrg_2 = 030 | yypvt[-2].y_word;} break;
case 124:
# line 971 "comm2.y"
{	mrg_2 = 040 | yypvt[-1].y_word;} break;
case 125:
# line 973 "comm2.y"
{	exp_2 = yypvt[-1].y_expr; ea707172(yypvt[-0].y_word);
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			} break;
case 126:
# line 977 "comm2.y"
{	exp_2 = yypvt[-4].y_expr; ea5x73(yypvt[-2].y_word, yypvt[-1].y_word);
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			} break;
case 127:
# line 981 "comm2.y"
{	exp_2 = yypvt[-6].y_expr; ea6x(yypvt[-4].y_word, yypvt[-2].y_word, yypvt[-1].y_word);
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			} break;
case 128:
# line 985 "comm2.y"
{	exp_2 = yypvt[-3].y_expr; ea72();
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			} break;
case 129:
# line 989 "comm2.y"
{	exp_2 = yypvt[-6].y_expr; ea73(yypvt[-2].y_word, yypvt[-1].y_word);
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			} break;
case 130:
# line 994 "comm2.y"
{	mrg_2 = 074; exp_2 = yypvt[-0].y_expr;
				 {rel_2  =  relonami ;  relonami  = 0;} ;
			} break;
case 132:
# line 1000 "comm2.y"
{	yyval.y_word = yypvt[-0].y_word | 010;} break;
case 133:
# line 1003 "comm2.y"
{	yyval.y_word =   0100  ;} break;
case 135:
# line 1007 "comm2.y"
{	yyval.y_word =  0300 ;} break;
case 137:
# line 1011 "comm2.y"
{	mrg_1 = mrg_2; exp_1 = exp_2;
				 {rel_1  =  rel_2 ;  rel_2  = 0;} ;
			} break;
case 139:
# line 1017 "comm2.y"
{	co_id = yypvt[-0].y_word; } break;
case 141:
# line 1019 "comm2.y"
{	co_id =  01000 ; } break;
case 143:
# line 1023 "comm2.y"
{	check_fsize(yypvt[-3].y_word,  00 );
				if ((mrg_2&070) == 010 && yypvt[-0].y_word != 001)
					badoperand();
				emit2((0170000|co_id|mrg_2));
				emit2((0100000|(yypvt[-0].y_word<<10)));
				ea_2( 0200 , 0);
			} break;
case 144:
# line 1031 "comm2.y"
{	check_fsize(yypvt[-3].y_word,  00 );
				if ((mrg_2&070) == 010 && yypvt[-2].y_word == 001)
					badoperand();
				emit2((0170000|co_id|mrg_2));
				emit2((0120000|(yypvt[-2].y_word<<10)));
				ea_2( 0200 ,  0x08 );
			} break;
case 145:
# line 1039 "comm2.y"
{	emit2(0170000|co_id);
				emit2((yypvt[-2].y_word<<10)|(yypvt[-0].y_word<<7));
			} break;
case 146:
# line 1043 "comm2.y"
{	ch_sz_dreg(yypvt[-3].y_word, mrg_2&070);
				emit2((0170000|co_id|mrg_2));
				emit2((0040000|(yypvt[-3].y_word<<10)|(yypvt[-0].y_word<<7)));
				ea_2( 0200 ,  0x01 );
			} break;
case 147:
# line 1049 "comm2.y"
{	ch_sz_dreg(yypvt[-3].y_word, mrg_2&070);
				if (yypvt[-3].y_word ==  03 )
					serror("packed decimal needs k-factor");
				emit2((0170000|co_id|mrg_2));
				emit2((0060000|(yypvt[-3].y_word<<10)|(yypvt[-2].y_word<<7)));
				ea_2( 0200 ,  0x01 | 0x08 );
			} break;
case 148:
# line 1057 "comm2.y"
{	check_fsize(yypvt[-7].y_word,  03 );
				 if (!((((((yypvt[-1].y_valu )+64 ) & ~((int)0177)) == 0) ) )) nofit() ;
				emit2((0170000|co_id|mrg_2));
				emit2((0066000|(yypvt[-6].y_word<<7)| ((short)(yypvt[-1].y_valu ) & 0177) ));
				ea_2( 0200 ,  0x02 | 0x01 | 0x08 );
			} break;
case 149:
# line 1064 "comm2.y"
{	check_fsize(yypvt[-6].y_word,  03 );
				emit2((0170000|co_id|mrg_2));
				emit2((0076000|(yypvt[-5].y_word<<7)|(yypvt[-1].y_word<<4)));
				ea_2( 0200 ,  0x02 | 0x01 | 0x08 );
			} break;
case 150:
# line 1070 "comm2.y"
{	 if (!((((yypvt[-2].y_valu ) & ~((int)0177)) == 0) )) nofit() ;
				check_fsize(yypvt[-4].y_word,  02 );
				emit2(0170000|co_id);
				emit2(056000|(yypvt[-0].y_word<<7)| ((short)(yypvt[-2].y_valu ) & 0177) );
			} break;
case 151:
# line 1076 "comm2.y"
{	check_fsize(yypvt[-3].y_word,  02 );
				if ((mrg_2&070) == 030)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2(0160000 |
					(((mrg_2&070)==040 || (yypvt[-2].y_word&04000)) ?
						yypvt[-2].y_word :
						(010000|reverse(yypvt[-2].y_word,8))));
				ea_2( 0200 ,  0x02 | 0x08 );
			} break;
case 152:
# line 1087 "comm2.y"
{	check_fsize(yypvt[-3].y_word,  02 );
				if ((mrg_2&070) == 040)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0150000|((yypvt[-0].y_word&04000)?yypvt[-0].y_word:reverse(yypvt[-0].y_word,8))));
				ea_2( 0200 ,  0x02 );
			} break;
case 153:
# line 1095 "comm2.y"
{	checksize(yypvt[-3].y_word, 4);
				if ((mrg_2&070) == 1 && yypvt[-2].y_word!= 02000)
					serror("bad addressing category");
				if ((mrg_2 & 070) == 0 &&
				    yypvt[-2].y_word != 02000 && yypvt[-2].y_word != 04000 && yypvt[-2].y_word != 010000)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0120000|yypvt[-2].y_word));
				ea_2( 0200 ,  0x08 );
			} break;
case 154:
# line 1106 "comm2.y"
{	checksize(yypvt[-3].y_word, 4);
				if ((mrg_2&070) == 1 && yypvt[-0].y_word!= 02000)
					serror("bad addressing category");
				if ((mrg_2 & 070) == 0 &&
				    yypvt[-0].y_word != 02000 && yypvt[-0].y_word != 04000 && yypvt[-0].y_word != 010000)
					serror("bad addressing category");
				emit2((0170000|co_id|mrg_2));
				emit2((0100000|yypvt[-0].y_word));
				ea_2( 0200 , 0);
			} break;
case 155:
# line 1117 "comm2.y"
{	emit2((0170000|co_id|mrg_2));
				emit2((0040000|(yypvt[-3].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-4].y_word));
				ch_sz_dreg(yypvt[-3].y_word, mrg_2&070);
				ea_2( 0200 ,  0x01 );
			} break;
case 156:
# line 1123 "comm2.y"
{	check_fsize(yypvt[-3].y_word,  02 );
				emit2(0170000|co_id);
				emit2((yypvt[-2].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-4].y_word);
			} break;
case 157:
# line 1128 "comm2.y"
{	emit2((0170000|co_id|mrg_2));
				emit2((0040000|(yypvt[-3].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-4].y_word));
				ch_sz_dreg(yypvt[-3].y_word, mrg_2&070);
				ea_2( 0200 ,  0x01 );
			} break;
case 158:
# line 1134 "comm2.y"
{	check_fsize(yypvt[-3].y_word,  02 );
				emit2(0170000|co_id);
				emit2((yypvt[-2].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-4].y_word);
			} break;
case 159:
# line 1139 "comm2.y"
{	check_fsize(yypvt[-1].y_word,  02 );
				emit2(0170000|co_id);
				emit2((yypvt[-0].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-2].y_word);
			} break;
case 160:
# line 1144 "comm2.y"
{	emit2(0170000|co_id|mrg_2);
				emit2(0040000|(yypvt[-5].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-6].y_word|yypvt[-2].y_word);
				ea_2( 0200 ,  0x01 );
			} break;
case 161:
# line 1149 "comm2.y"
{	check_fsize(yypvt[-5].y_word,  02 );
				emit2(0170000|co_id);
				emit2((yypvt[-4].y_word<<10)|(yypvt[-0].y_word<<7)|yypvt[-6].y_word|yypvt[-2].y_word);
			} break;
case 162:
# line 1154 "comm2.y"
{	fbranch(yypvt[-1].y_word, yypvt[-0].y_expr);} break;
case 163:
# line 1156 "comm2.y"
{	emit2(0170110|co_id|yypvt[-2].y_word);
				emit2(yypvt[-3].y_word);
				yypvt[-0].y_expr.val -= DOTVAL;
				 if (!(((((yypvt[-0].y_expr.val ) + 0x8000L) & ~0xFFFFL) == 0) )) nofit() ;

				newrelo(yypvt[-0].y_expr.typ, RELPC|RELO2|RELBR|RELWR);

				emit2( ((int)(yypvt[-0].y_expr.val ) & 0xFFFF) );
			} break;
case 164:
# line 1166 "comm2.y"
{	emit2(0170200|co_id);
				emit2(0);
			} break;
case 165:
# line 1170 "comm2.y"
{	emit2(0170100|co_id|mrg_2);
				emit2(yypvt[-1].y_word);
				ea_2( 0000 ,  0x01 | 0x08 );
			} break;
case 166:
# line 1175 "comm2.y"
{	emit2((0170000|co_id|mrg_2));
				emit2((0040072|(yypvt[-1].y_word<<10)));
				ch_sz_dreg(yypvt[-1].y_word, mrg_2&070);
				ea_2( 0200 ,  0x01 );
			} break;
case 167:
# line 1181 "comm2.y"
{	check_fsize(yypvt[-1].y_word,  02 );
				emit2(0170000|co_id);
				emit2((yypvt[-0].y_word<<10)|072);
			} break;
case 168:
# line 1186 "comm2.y"
{	if ((mrg_2&070) == (yypvt[-1].y_word&070))
					badoperand();
				emit2((0170000|co_id|(yypvt[-1].y_word&0700)|mrg_2));
				ea_2(0, yypvt[-1].y_word&07);
			} break;
case 169:
# line 1192 "comm2.y"
{	emit2(0170174|co_id);
				emit2(yypvt[-0].y_word);
			} break;
case 170:
# line 1196 "comm2.y"
{	checksize(yypvt[-1].y_word, 2|4);
				emit2((0170170|co_id|(yypvt[-1].y_word== 0200 ?03:02)));
				emit2(yypvt[-2].y_word);
				ea_2(yypvt[-1].y_word,0);
			} break;
case 171:
# line 1203 "comm2.y"
{	yyval.y_word = 04000 | yypvt[-0].y_word << 4; } break;
case 174:
# line 1208 "comm2.y"
{	yyval.y_word = yypvt[-2].y_word | yypvt[-0].y_word;} break;
case 175:
# line 1211 "comm2.y"
{	yyval.y_word = 1 << yypvt[-0].y_word; } break;
case 176:
# line 1213 "comm2.y"
{	if (yypvt[-2].y_word > yypvt[-0].y_word)
					badoperand();
				for (yyval.y_word = 0; yypvt[-2].y_word <= yypvt[-0].y_word; yypvt[-2].y_word++)
					yyval.y_word |= (1 << yypvt[-2].y_word);
			} break;
case 177:
# line 1220 "comm2.y"
{	yyval.y_word = yypvt[-0].y_word << 10; } break;
case 178:
# line 1222 "comm2.y"
{	yyval.y_word = yypvt[-2].y_word | (yypvt[-0].y_word << 10); } break;
case 179:
# line 1225 "comm2.y"
{	yyval.y_word =  02 ; } break;
case 180:
# line 1227 "comm2.y"
{	if (yypvt[-0].y_word ==  0200 )
					yyval.y_word =  00 ;
				else if (yypvt[-0].y_word ==  0100 )
					yyval.y_word =  04 ;
				else	yyval.y_word =  06 ;
			} break;
		}
		goto yystack;  /* stack new state and value */

	}
