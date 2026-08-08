#ifndef _V7_SH_PROTO_H_
#define _V7_SH_PROTO_H_

/* mode.h has already renamed the historical `struct void' tag. */
#ifdef void
#undef void
#endif

/*
 * 68000/32-bit compatibility prototypes for the historical Bourne shell.
 * Keep function definitions in K&R form; this header describes their
 * promoted target ABI to callers.
 */

/* arguments and storage */
int main(int, STRING *);
INT options(int, STRING *);
VOID setargs(STRING *);
DOLPTR freeargs(DOLPTR);
int clearup(void);
DOLPTR useargs(void);
VOID initblok(void);
ADDRESS alloc(unsigned int);
VOID addblok(unsigned int);
VOID free(BLKPTR);

/* parser, execution, and expansion */
int builtin(void);
TREPTR makefork(int, TREPTR);
TREPTR cmd(int, int);
int execute(TREPTR, int, INT *, INT *);
int execexp(STRING, int);
INT expand(STRING, int);
int gmatch(STRING, STRING);
int makearg(ARGPTR);
int word(void);
int nextc(int);
int readc(void);

/* errors, signals, and process completion */
int exitset(void);
int sigchk(void);
int failed(STRING, STRING);
int error(STRING);
int exitsh(int);
int done(void);
int rmtemp(IOPTR);
VOID fault(int);
int stdsigs(void);
int ignsig(int);
int getsig(int);
int oldsigs(void);
int clrsig(int);
int chktrap(void);

/* input, output, and macro expansion */
int initf(int);
int estabf(STRING);
int push(FILE);
int pop(void);
int chkpipe(i16 *);
int chkopen(STRING);
int rename(int, int);
int create(STRING);
int tmpfil(void);
int copy(IOPTR);
STRING macro(STRING);
int subst(int, int);
VOID initio(IOPTR);
STRING getpath(STRING);
INT pathopen(STRING, STRING);
STRING catpath(STRING, STRING);
VOID execa(STRING *);
int postclr(void);
VOID post(int);
VOID await(int);
int trim(STRING);
STRING mactrim(STRING);
STRING *scan(int);
INT getarg(COMPTR);

/* names and environment */
int syslook(STRING, SYSTAB);
int setlist(ARGPTR, int);
VOID setname(STRING, int);
int replace(STRING *, STRING);
int dfault(NAMPTR, STRING);
int assign(NAMPTR, STRING);
INT readvar(STRING *);
int assnum(STRING *, int);
STRING make(STRING);
NAMPTR lookup(STRING);
int namscan(VOID (*)(NAMPTR));
VOID printnam(NAMPTR);
VOID exname(NAMPTR);
VOID printflg(NAMPTR);
VOID sh_getenv(void);
VOID countnam(NAMPTR);
VOID pushnam(NAMPTR);
STRING *setenv(void);

/* formatting and strings */
int newline(void);
int blank(void);
int prp(void);
VOID prs(STRING);
VOID prc(int);
int prt(L_INT);
int prn(int);
int itos(int);
int stoi(STRING);
STRING movstr(STRING, STRING);
INT any(int, STRING);
INT cf(STRING, STRING);
INT length(STRING);

/* shell stack and break management */
BYTPTR setbrk(i32);
STKPTR getstak(int);
STKPTR locstak(void);
STKPTR savstak(void);
STKPTR endstak(STRING);
VOID tdystak(STKPTR);
int stakchk(void);
STKPTR cpystak(STKPTR);

/* main.c helpers used outside their defining source. */
int chkpr(int);
int settmp(void);
VOID Ldup(L_INT, L_INT);

/* V7 runtime and system-call ABI used by the shell. */
struct sgttyb;

i32 alarm(i32);
i32 chdir(char *);
i32 close(i32);
i32 creat(char *, i32);
i32 dup(i32);
i32 dup2(i32, i32);
i32 execve(char *, char **, char **);
i32 exit(int);
i32 fork(void);
i32 getpid(void);
i32 getuid(void);
i32 gtty(i32, struct sgttyb *);
i32 ioctl(i32, i32, char *);
i32 lseek(i32, i32, i32);
i32 open(char *, i32);
i32 pause(void);
i32 pipe(i16 *);
i32 read(i32, char *, i32);
L_INT signal(int, L_INT);
#define signal(sig, handler) signal((sig), (L_INT)(handler))
BYTPTR sbrk(i32);
i32 stat(char *, struct stat *);
i32 times(i32 *);
i32 umask(i32);
i32 unlink(char *);
i32 wait(i16 *);
i32 write(i32, char *, i32);

#endif
