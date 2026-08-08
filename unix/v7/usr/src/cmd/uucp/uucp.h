#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct pack;
struct userpath;

/* Shared UUCP interfaces.  Explicit declarations are required on the
 * 32-bit port: an implicit int return truncates pointers to 16 bits. */
int anlwrk(char *, char **);
int iswrk(char *, char *, char *, char *);
int gtwvec(char *, char *, char *, char **);
int gtwrk(char *, char *, char **, int);
i32 compar(const void *, const void *);
int copy(char *, char *, char *, char *);
int chkpth(char *, char *, char *);
int rdpth(struct userpath *);
int callback(char *);
int chklnk(char *);
int chkperm(char *, char *, char *);
int cleanup(int);
int cntrl(int, char *);
int rmesg(int, char *);
int wmesg(int, char *);
int notify(char *, char *, char *, char *, char *);
int startup(int);
char fptcl(char *);
char *blptcl(char *);
int stptcl(char *);
int conn(char *);
char *lastc(char *);
int gdial(void);
int ckdev(char *, char *, int);
int getto(char **);
int call(char **);
int dialup(char *, char **);
int clsacu(void);
int direct(char **);
int finds(char *, char **);
int login(int, char **, int);
int fixline(int, int);
int fixmode(int);
int expect(char *, int);
int sendthem(char *, int);
int genbrk(int, int, int);
int notin(char *, char *);
int ifdate(char *);
int xcp(char *, char *);
int xmv(char *, char *);
int expfile(char *);
int isdir(char *);
int mkdirs(char *);
int gename(int, char *, int, char *);
int getseq(char *);
int getargs(char *, char **);
char *getprm(char *, char *);
int split(char *, char *, char *);
int guinfo(int, char *, char *);
int gninfo(char *, int *, char *);
int pkfail(void);
int gturnon(void);
int gturnoff(void);
int gwrmsg(int, char *, int);
int grdmsg(char *, int);
int gwrdata(FILE *, int);
int grddata(int, FILE *);
int grdblk(char *, int, int);
int gwrblk(char *, unsigned, int);
int gnamef(FILE *, char *);
int gnsys(char *, char *, int);
int srchst(char *, char **, int);
int gnxseq(char *);
int cmtseq(void);
int ulkseq(void);
int gwd(char *);
int imsg(char *, int);
int omsg(int, char *, int);
char *lastpart(char *);
int logent(char *, char *);
int mlogent(FILE *, char *, char *);
int logcls(void);
int syslog(char *);
int pkon(int, int);
int pkoff(int);
int prefix(char *, char *);
int sdmail(char *, i32);
int mailst(char *, char *);
int shio(char *, char *, char *, char *);
int sysacct(long, time_t);
int systat(char *, int, char *);
int rmstat(char *);
int callok(char *);
int ulockf(char *, time_t);
int stlock(char *);
int rmlock(char *);
int isalock(char *);
int unlock(char *);
int onelock(int, char *, char *);
int lock(int, char *, char **);
int delock(char *);
int mlock(char *);
int chkpre(char *);
int stpre(char *);
int appendf(FILE *, char *);
int gtxfile(char *);
int gotfiles(char *);
int rmxfiles(char *);
int mvxfiles(char *);
int cmdok(char *, char *);
int versys(char *);
int xuucico(char *);
int xuuxqt(void);
int xuucp(char *);
	/*  some system names  */
#define MYNAME		"unix768"

#define THISDIR		"/usr/lib/uucp"
#define SQFILE		"/usr/lib/uucp/SQFILE"
#define SQTMP		"/usr/lib/uucp/SQTMP"
#define SYSFILE		"/usr/lib/uucp/L.sys"
#define SYSFILECR	"/usr/lib/uucp/L.sys.cr"
#define DEVFILE		"/usr/lib/uucp/L-devices"
#define DIALFILE	"/usr/lib/uucp/L-dialcodes"
#define USERFILE	"/usr/lib/uucp/USERFILE"

#define SPOOL		"/usr/spool/uucp"
#define LOGDIR		"/usr/spool/uucp"
#define SEQFILE		"/usr/spool/uucp/SEQF"
#define SQLOCK		"/usr/spool/uucp/LCK.SQ"
#define SYSLOG		"/usr/spool/uucp/SYSLOG"
#define XQTDIR		"/usr/spool/uucp/.XQTDIR"

#define SEQLOCK		"LCK.SEQL"
#define CMDPRE		'C'
#define DATAPRE		'D'
#define XQTPRE		'X'

#define LOGPREFIX	"LOG."
#define LOGLOCK	"/usr/spool/uucp/LCK.LOG"
#define LOGFILE	"/usr/spool/uucp/LOGFILE"

#define RMTDEBUG	"AUDIT"
#define SQTIME		60L

#define DEBUG(l, f, s) if (Debug >= l) fprintf(stderr, f, s)

#define ASSERT(e, f, v) if (!(e)) {\
fprintf(stderr, "AERROR - (%s) ", "e");\
fprintf(stderr, f, v);\
cleanup(FAIL);};

#define FMV(p, n) close(n); dup(p[n]); close(p[n]);

#define SAME 0
#define FAIL -1
#define SUCCESS 0
#define MASTER 1
#define SLAVE 0
#define MAXFULLNAME 100
#define MAXMSGTIME 45
#define MAXCHARTIME 15
#define NAMESIZE 15
#define ACULAST "-<"
#define EOTMSG "\004\n\004\n"
#define CALLBACK 1
#define LINKLEVEL 1

	/*  commands  */
#define SHELL		"/bin/sh"
#define MAIL		"mail"
#define UUCICO		"/usr/lib/uucp/uucico"
#define UUXQT		"/usr/lib/uucp/uuxqt"
#define UUCP		"usr/lib/uucp/uucp"


	/*  call connect fail stuff  */
#define CF_SYSTEM	-1
#define CF_TIME		-2
#define CF_LOCK		-3
#define CF_DIAL		-5
#define CF_LOGIN	-6

	/*  system status stuff  */
#define SS_OK		0
#define SS_FAIL		4
#define SS_NODEVICE	1
#define SS_CALLBACK	2
#define SS_INPROGRESS	3
#define SS_BADSEQ	5

	/*  fail/retry parameters  */
#define RETRYTIME 3300L
#define INPROGTIME 7200L
#define MAXRECALLS 10

	/*  stuff for command execution  */
#define X_RQDFILE	'F'
#define X_STDIN		'I'
#define X_STDOUT	'O'
#define X_CMD		'C'
#define X_USER		'U'
#define X_SENDFILE	'S'
#define X_LOCK		"LCK.XQT"
#define X_LOCKTIME	3600L

int Ifn, Ofn;
char Rmtname[10];
char User[10];
char Loginuser[10];
char *Thisdir;
char *Spool;
char *Myname;
char *Sysfiles[];
char *Devfile;
char *Dialfile;
int Debug;
int Pkdebug;
int Pkdrvon;
int Bspeed;

#define WKDSIZE	100	/*  size of work dir name  */
char Wrkdir[WKDSIZE];
