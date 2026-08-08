/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 535 "/tmp/cc-as-comm2.i"
{
	 short 	y_word;
	 int64_t 	y_valu;
	expr_t	y_expr;
	item_t	*y_item;

}
/* Line 1529 of yacc.c.  */
#line 241 "cc/as/generated/comm2.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

