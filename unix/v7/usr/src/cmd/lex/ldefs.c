# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# define cfree(p, n, s) free(p)
# define PP 1
# ifdef unix

# define CWIDTH 7
# define CMASK 0177
# define ASCII 1
# endif

# ifdef gcos
# define CWIDTH 9
# define CMASK 0777
# define ASCII 1
# endif

# ifdef ibm
# define CWIDTH 8
# define CMASK 0377
# define EBCDIC 1
# endif

# ifdef ASCII
# define NCH 128
# endif

# ifdef EBCDIC
# define NCH 256
# endif


# define TOKENSIZE 1000
# define DEFSIZE 40
# define DEFCHAR 1000
# define STARTCHAR 100
# define STARTSIZE 256
# define CCLSIZE 1000
# ifdef SMALL
# define TREESIZE 600
# define NTRANS 1500
# define NSTATES 300
# define MAXPOS 1500
# define NOUTPUT 1500
# endif

# ifndef SMALL
# define TREESIZE 1000
# define NSTATES 500
# define MAXPOS 2500
# define NTRANS 2000
# define NOUTPUT 3000
# endif
# define NACTIONS 100
# define ALITTLEEXTRA 30

# define RCCL NCH+90
# define RNCCL NCH+91
# define RSTR NCH+92
# define RSCON NCH+93
# define RNEWE NCH+94
# define FINAL NCH+95
# define RNULLS NCH+96
# define RCAT NCH+97
# define STAR NCH+98
# define PLUS NCH+99
# define QUEST NCH+100
# define DIV NCH+101
# define BAR NCH+102
# define CARAT NCH+103
# define S1FINAL NCH+104
# define S2FINAL NCH+105

# define DEFSECTION 1
# define RULESECTION 2
# define ENDSECTION 5
# define TRUE 1
# define FALSE 0

# define PC 1
# define PS 1

# ifdef DEBUG
# define LINESIZE 110
extern int yydebug;
extern int debug;		/* 1 = on */
extern int charc;
# endif

# ifndef DEBUG
# define freturn(s) s
# endif

extern int sargc;
extern char **sargv;
extern char buf[520];
extern int ratfor;		/* 1 = ratfor, 0 = C */
extern int yyline;		/* line number of file */
extern int sect;
extern int eof;
extern int lgatflg;
extern int divflg;
extern int funcflag;
extern int pflag;
extern int casecount;
extern int chset;	/* 1 = char set modified */
extern FILE *fin, *fout, *fother, *errorf;
extern int fptr;
extern char *ratname, *cname;
extern int prev;	/* previous input character */
extern int pres;	/* present input character */
extern int peek;	/* next input character */
extern int *name;
extern i32 *left;
extern i32 *right;
extern int *parent;
extern char *nullstr;
extern int tptr;
extern char pushc[TOKENSIZE];
extern char *pushptr;
extern char slist[STARTSIZE];
extern char *slptr;
extern char **def, **subs, *dchar;
extern char **sname, *schar;
extern char *ccl;
extern char *ccptr;
extern char *dp, *sp;
extern int dptr, sptr;
extern char *bptr;		/* store input position */
extern char *tmpstat;
extern int count;
extern int **foll;
extern int *nxtpos;
extern int *positions;
extern int *gotof;
extern int *nexts;
extern char *nchar;
extern int **state;
extern int *sfall;		/* fallback state num */
extern char *cpackflg;		/* true if state has been character packed */
extern int *atable, aptr;
extern int nptr;
extern char symbol[NCH];
extern char cindex[NCH];
extern int xstate;
extern int stnum;
extern int ctable[];
extern int ZCH;
extern int ccount;
extern char match[NCH];
extern char extra[NACTIONS];
extern char *pcptr, *pchar;
extern int pchlen;
extern int nstates, maxpos;
extern int yytop;
extern int report;
extern int ntrans, treesize, outsize;
extern long rcount;
extern int optim;
extern int *verify, *advance, *stoff;
extern int scon;
extern char *psave;
void *myalloc(int, int);
char *getl(char *);
int space(int);
int digit(int);
void error(char *, ...);
void warning(char *, ...);
int lexindex(int, const char *);
int alpha(int);
int printable(int);
void lgate(void);
void scopy(char *, char *);
int siconv(char *);
int slength(char *);
int scomp(char *, char *);
int ctrans(char **);
void cclinter(int);
int usescape(int);
int lookup(char *, char **);
int cpyact(void);
int gch(void);
int mn0(int);
int mn1(int, i32);
int mn2(int, i32, i32);
int dupl(int);
void munput(int, i32);
void cfoll(int);
void first(int);
void follow(int);
void cgoto(void);
void mkmatch(void);
void layout(void);
void phead1(void);
void phead2(void);
void ptail(void);
void statistics(void);
int yylex(void);
int yyparse(void);
int yyerror(char *);
# ifdef DEBUG
void buserr(i16);
void segviol(i16);
void allprint(char);
void strpt(char *);
void sect1dump(void);
void sect2dump(void);
void pfoll(void);
void stprt(int);
void pccl(void);
# endif
