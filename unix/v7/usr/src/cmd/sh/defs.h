#
/*
 *	UNIX shell
 */

/* error exits from various parts of shell */
#define ERROR	1
#define SYNBAD	2
#define SIGFAIL 3
#define SIGFLG	0200

/* command tree */
#define FPRS	020
#define FINT	040
#define FAMP	0100
#define FPIN	0400
#define FPOU	01000
#define FPCL	02000
#define FCMD	04000
#define COMMSK	017

#define TCOM	0
#define TPAR	1
#define TFIL	2
#define TLST	3
#define TIF	4
#define TWH	5
#define TUN	6
#define TSW	7
#define TAND	8
#define TORF	9
#define TFORK	10
#define TFOR	11

/* execute table */
#define SYSSET	1
#define SYSCD	2
#define SYSEXEC	3
#define SYSLOGIN 4
#define SYSTRAP	5
#define SYSEXIT	6
#define SYSSHFT 7
#define SYSWAIT	8
#define SYSCONT 9
#define SYSBREAK 10
#define SYSEVAL 11
#define SYSDOT	12
#define SYSRDONLY 13
#define SYSTIMES 14
#define SYSXPORT 15
#define SYSNULL 16
#define SYSREAD 17
#define SYSTST	18
#define	SYSUMASK	19

/* used for input and output of shell */
#define INIO 10
#define OTIO 11

/*io nodes*/
#define USERIO	10
#define IOUFD	15
#define IODOC	16
#define IOPUT	32
#define IOAPP	64
#define IOMOV	128
#define IORDW	256
#define INPIPE	0
#define OTPIPE	1

/* arg list terminator */
#define ENDARGS	0

#include	<sys/inttypes.h>
#include	"mac.h"
#include	"mode.h"
#include	"name.h"
#define		alloc malloc
#include	"proto.h"


/* result type declarations */
#if 0 /* Superseded by the ABI prototypes in proto.h. */
ADDRESS		alloc();
VOID		initblok();
VOID		addblok();
STRING		make();
STRING		movstr();
TREPTR		cmd();
TREPTR		makefork();
NAMPTR		lookup();
VOID		setname();
VOID		setargs();
DOLPTR		useargs();
DOLPTR		freeargs();
REAL		expr();
STRING		catpath();
STRING		getpath();
STRING		*scan();
STRING		mactrim();
STRING		macro();
VOID		await();
VOID		post();
VOID		exname();
VOID		printnam();
VOID		printflg();
VOID		prs();
VOID		prc();
VOID		getenv();
STRING		*setenv();
i32		alarm(i32);
i32		chdir(char *);
i32		close(i32);
i32		creat(char *, i32);
i32		dup(i32);
i32		exit(i32);
i32		fork();
i32		lseek(i32, i32, i32);
i32		open(char *, i32);
i32		pause();
i32		pipe(i16 *);
i32		read(i32, char *, i32);
i32		times(i32 *);
i32		umask(i32);
i32		unlink(char *);
i32		wait(i16 *);
i32		write(i32, char *, i32);
#endif

#define attrib(n,f)	(n->namflg |= f)
#define round(a,b)	(((long)((ADR(a)+b)-1))&~((long)(b)-1))
#define closepipe(x)	(close(x[INPIPE]), close(x[OTPIPE]))
#define eq(a,b)		(cf(a,b)==0)
#define max(a,b)	((a)>(b)?(a):(b))
#define assert(x)	;

/* temp files and io */
extern UFD	output;
extern INT	ioset;
extern IOPTR	iotemp;		/* files to be deleted sometime */
extern IOPTR	iopend;		/* documents waiting to be read at NL */

/* substitution */
extern INT	dolc;
extern STRING	*dolv;
extern DOLPTR	argfor;
extern ARGPTR	gchain;

/* stack */
#define		BLK(x)	((BLKPTR)(x))
#define		BYT(x)	((BYTPTR)(x))
#define		STK(x)	((STKPTR)(x))
#define		ADR(x)	((char*)(x))

/* stak stuff */
#include	"stak.h"

/* string constants */
extern MSG	atline;
extern MSG	readmsg;
extern MSG	colon;
extern MSG	minus;
extern MSG	nullstr;
extern MSG	sptbnl;
extern MSG	unexpected;
extern MSG	endoffile;
extern MSG	synmsg;

/* name tree and words */
extern SYSTAB	reserved;
extern INT	wdval;
extern INT	wdnum;
extern ARGPTR	wdarg;
extern INT	wdset;
extern BOOL	reserv;

/* prompting */
extern MSG	stdprompt;
extern MSG	supprompt;
extern MSG	profile;

/* built in names */
extern NAMNOD	fngnod;
extern NAMNOD	ifsnod;
extern NAMNOD	homenod;
extern NAMNOD	mailnod;
extern NAMNOD	pathnod;
extern NAMNOD	ps1nod;
extern NAMNOD	ps2nod;

/* special names */
extern MSG	flagadr;
extern STRING	cmdadr;
extern STRING	exitadr;
extern STRING	dolladr;
extern STRING	pcsadr;
extern STRING	pidadr;

extern MSG	defpath;

/* names always present */
extern MSG	mailname;
extern MSG	homename;
extern MSG	pathname;
extern MSG	fngname;
extern MSG	ifsname;
extern MSG	ps1name;
extern MSG	ps2name;

/* transput */
extern CHAR	tmpout[];
extern STRING	tmpnam;
extern INT	serial;
#define		TMPNAM 7
extern FILE	standin;
#define input	(standin->fdes)
#define eof	(standin->feof)
extern INT	peekc;
extern STRING	comdiv;
extern MSG	devnull;

/* flags */
#define		noexec	01
#define		intflg	02
#define		prompt	04
#define		setflg	010
#define		errflg	020
#define		ttyflg	040
#define		forked	0100
#define		oneflg	0200
#define		rshflg	0400
#define		waiting	01000
#define		stdflg	02000
#define		execpr	04000
#define		readpr	010000
#define		keyflg	020000
extern INT	flags;

/* error exits from various parts of shell */
#include	<setjmp.h>
extern jmp_buf	subshell;
extern jmp_buf	errshell;

/* fault handling */
#include	"brkincr.h"
extern POS	brkincr;

#define MINTRAP	0
#define MAXTRAP	17

#define INTR	2
#define QUIT	3
#define MEMF	11
#define ALARM	14
#define KILL	15
#define TRAPSET	2
#define SIGSET	4
#define SIGMOD	8

#if 0 /* Superseded by the ABI prototype in proto.h. */
VOID		fault();
#endif
extern BOOL	trapnote;
extern STRING	trapcom[];
extern BOOL	trapflg[];

/* name tree and words */
extern STRING	*environ;
extern CHAR	numbuf[];
extern MSG	export;
extern MSG	readonly;

/* execflgs */
extern INT	exitval;
extern BOOL	execbrk;
extern INT	loopcnt;
extern INT	breakcnt;

/* messages */
extern MSG	mailmsg;
extern MSG	coredump;
extern MSG	badopt;
extern MSG	badparam;
extern MSG	badsub;
extern MSG	nospace;
extern MSG	notfound;
extern MSG	badtrap;
extern MSG	baddir;
extern MSG	badshift;
extern MSG	illegal;
extern MSG	restricted;
extern MSG	execpmsg;
extern MSG	notid;
extern MSG	wtfailed;
extern MSG	badcreate;
extern MSG	piperr;
extern MSG	badopen;
extern MSG	badnum;
extern MSG	arglist;
extern MSG	txtbsy;
extern MSG	toobig;
extern MSG	badexec;
extern MSG	badfile;

extern address	end[];

#include	"ctype.h"
