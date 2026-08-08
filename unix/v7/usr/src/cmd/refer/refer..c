# include "refer.h"
extern FILE *in;
extern int endpush, labels, sort, bare, keywant;
extern char *smallcaps;
extern char comname;
extern char *keystr;
extern int authrev;
extern int nmlen, dtlen;
extern char *data[], **search;
extern int refnum;
extern char *reftable[];
extern char *rtp, reftext[];
extern int sep;
extern char tfile[];
extern char gfile[];
extern char ofile[];
extern char hidenam[];
extern char *Ifile; extern int Iline;
extern FILE *fo, *ftemp;
# define FLAG 003
# define NRFTXT 2000
# define NTFILE 20
# define NRFTBL 200
# define LLINE 512
# define QLEN 300
# define ANSLEN 1000
# define TAGLEN 400
# define NSERCH 20

char *input(char *);
char *lookat(void);
void addch(char *, int);
void output(char *);
void append(char *);
void flout(void);
void doref(char *);
int newline(char *);
void choices(char *);
int control(int);
int common(char *);
int chkdup(char *);
void dumpold(void);
void recopy(char *);
void putsig(int, char **, int, char *, char *);
void putkey(int, char **, int, char *);
void putref(int, char **);
int tabs(char **, char *);
char *caps(char *, char *);
char *revauth(char *, char *);
int last(char *);
char *fpar(int, char **, char *, int, int, int);
int keylet(char *, int);
char *artskp(char *);
