/* t..c : external declarations */

# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
# include <string.h>

# define MAXLIN 200
# define MAXHEAD 30
# define MAXCOL 20
# define MAXCHS 2000
# define MAXRPT 100
# define CLLEN 10
# define SHORTLINE 4
extern int nlin, ncol, iline, nclin, nslin;
extern int style[MAXHEAD][MAXCOL];
extern int ctop[MAXHEAD][MAXCOL];
extern char font[MAXHEAD][MAXCOL][2];
extern char csize[MAXHEAD][MAXCOL][4];
extern char vsize[MAXHEAD][MAXCOL][4];
extern char cll[MAXCOL][CLLEN];
extern int stynum[];
extern int F1, F2;
extern int lefline[MAXHEAD][MAXCOL];
extern int fullbot[];
extern char *instead[];
extern int expflg;
extern int ctrflg;
extern int evenflg;
extern int evenup[];
extern int boxflg;
extern int dboxflg;
extern int linsize;
extern int tab;
extern int pr1403;
extern int linsize, delim1, delim2;
extern int allflg;
extern int textflg;
extern int left1flg;
extern int rightl;
struct colstr {char *col, *rcol;};
extern struct colstr *table[];
extern char *cspace, *cstore;
extern char *exstore, *exlim;
extern int sep[];
extern int used[], lused[], rused[];
extern int linestop[];
extern char *leftover;
extern char *last, *ifile;
extern int texname;
extern int texct, texmax;
extern char texstr[];
extern int linstart;


extern FILE *tabin, *tabout;

int swapin(void);
void error(char *);
int gets1(char *);
void un1getc(int);
int get1char(void);
void tableput(void);
void getcomm(void);
void getspec(void);
void readspec(void);
void gettbl(void);
int vspand(int, int, int);
int vspen(char *);
void maktab(void);
int filler(char *);
void runout(void);
void runtabs(int, int);
int ifline(char *);
void need(void);
void putline(int, int);
void putfont(char *);
void putsize(char *);
void yetmore(void);
void checkuse(void);
int real(char *);
char *chspace(void);
struct colstr *alocv(int);
void release(void);
void choochar(void);
int point(char *);
void savefill(void);
void rstofill(void);
void endoff(void);
void ifdivert(void);
void saveline(void);
void restline(void);
void cleanfc(void);
char *gettext(char *, int, int, char *, char *);
void untext(void);
int interv(int, int);
int interh(int, int);
char *maknew(char *);
int match(char *, char *);
int prefix(char *, char *);
int letter(int);
int numb(char *);
int digit(int);
int max(int, int);
void tcopy(char *, char *);
int ctype(int, int);
int min(int, int);
int fspan(int, int);
int lspan(int, int);
int ctspan(int, int);
void tohcol(int);
int allh(int);
int thish(int, int);
void makeline(int, int, int);
void fullwide(int, int);
void drawline(int, int, int, int, int, int);
void getstop(void);
int left(int, int, int *);
int lefdata(int, int);
int next(int);
int prev(int);
void drawvert(int, int, int, int);
int barent(char *);

# define PTRVAL(p) ((int)(long)(p))
# define CRIGHT 80
# define CLEFT 40
# define CMID 60
# define S1 31
# define S2 32
# define TMP 38
# define SF 35
# define SL 34
# define LSIZE 33
# define SIND 37
# define SVS 36
/* this refers to the relative position of lines */
# define LEFT 1
# define RIGHT 2
# define THRU 3
# define TOP 1
# define BOT 2
