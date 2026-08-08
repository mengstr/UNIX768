#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	FATAL	1
#define	ROM	'1'
#define	ITAL	'2'
#define	BLD	'3'

#define	VERT(n)	((((n)+1)/3)*3)
#define	EFFPS(p)	((p) >= 6 ? (p) : 6)

extern int	dbg;
extern int	ct;
extern int	lp[];
extern int	used[];	/* available registers */
extern int	ps;	/* dflt init pt size */
extern int	deltaps;	/* default change in ps */
extern int	gsize;	/* global size */
extern int	gfont;	/* global font */
extern int	ft;	/* dflt font */
extern FILE	*curfile;	/* current input file */
extern int	ifile;	/* input file number */
extern int	linect;	/* line number in current file */
extern int	eqline;	/* line where eqn started */
extern int	svargc;
extern char	**svargv;
extern int	eht[];
extern int	ebase[];
extern int	lfont[];
extern int	rfont[];
typedef long	eqn_value;

extern eqn_value	yyval;
extern eqn_value	yylval;
#define	YV	((int)yyval)
extern int	eqnreg, eqnht;
extern int	lefteq, righteq;
extern int	lastchar;	/* last character read by lex */
extern int	markline;	/* 1 if this EQ/EN contains mark or lineup */

typedef struct s_tbl {
	char	*name;
	char	*defn;
	struct s_tbl *next;
} tbl;

extern tbl	*keytbl[];
extern tbl	*restbl[];
extern tbl	*deftbl[];

int	yyparse(void);
int	yylex(void);
void	yyerror(char *);
int	gtc(void);
void	getstr(char *, int);
void	putout(int);
int	max(int, int);
int	oalloc(void);
void	ofree(int);
void	setps(int);
void	nrwid(int, int, int);
void	error(int, char *, ...);
void	init_tbl(void);
tbl	*lookup(tbl **, char *, char *);
void	eqnbox(int, int, int);
void	setfont(char);
void	font(int, int);
void	fatbox(int);
void	globfont(void);
void	fromto(int, int, int);
void	funny(int);
void	integral(int, int, int);
void	setintegral(void);
void	mark(int);
void	lineup(int);
void	column(int, int);
void	matrix(int);
void	move(int, int, int);
void	boverb(int, int);
void	paren(int, int, int);
void	lpile(int, int, int);
void	bshiftb(int, int, int);
void	shift(int);
void	shift2(int, int, int);
void	setsize(char *);
void	size(int, int);
void	globsize(void);
void	eqnsqrt(int);
void	text(int, char *);
void	diacrit(int, int);
