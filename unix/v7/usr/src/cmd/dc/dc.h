#define FATAL 0
#define NFATAL 1
#define BLK sizeof(struct blk)
#define PTRSZ sizeof(struct blk *)
#define HEADSZ 1024
#define STKSZ 100
#define RDSKSZ 100
#define TBLSZ 256
#define ARRAYST 0241
#define MAXIND 2048
#define NL 1
#define NG 2
#define NE 3
#define length(p) ((int)((p)->wt-(p)->beg))
#define position(p) ((int)((p)->rd-(p)->beg))
#define rewind(p) (p)->rd=(p)->beg
#define create(p)	(p)->rd = (p)->wt = (p)->beg
#define fsfile(p)	(p)->rd = (p)->wt
#define truncate(p)	(p)->wt = (p)->rd
#define sfeof(p)	(((p)->rd==(p)->wt)?1:0)
#define sfbeg(p)	(((p)->rd==(p)->beg)?1:0)
#define sungetc(p,c)	*(--(p)->rd)=c
#ifdef interdata
#define NEGBYTE 0200
#define MASK (-1 & ~0377)
#define sgetc(p)	( ((p)->rd==(p)->wt) ? EOF :( ((*(p)->rd & NEGBYTE) != 0) ? ( *(p)->rd++ | MASK): *(p)->rd++ ))
#define slookc(p)	( ((p)->rd==(p)->wt) ? EOF :( ((*(p)->rd & NEGBYTE) != 0) ? (*(p)->rd | MASK) : *(p)->rd ))
#define sbackc(p)	( ((p)->rd==(p)->beg) ? EOF :( ((*(--(p)->rd) & NEGBYTE) != 0) ? (*(p)->rd | MASK): *(p)->rd ))
#endif
#ifndef interdata
#define sgetc(p)	(((p)->rd==(p)->wt)?EOF:*(p)->rd++)
#define slookc(p)	(((p)->rd==(p)->wt)?EOF:*(p)->rd)
#define sbackc(p)	(((p)->rd==(p)->beg)?EOF:*(--(p)->rd))
#endif
#define sputc(p,c)	{if((p)->wt==(p)->last)more(p); *(p)->wt++ = c; }
#define salterc(p,c)	{if((p)->rd==(p)->last)more(p); *(p)->rd++ = c; if((p)->rd>(p)->wt)(p)->wt=(p)->rd;}
#define sunputc(p)	(*( (p)->rd = --(p)->wt))
#define zero(p)	for(pp=(p)->beg;pp<(p)->last;)*pp++='\0'
#define OUTC(x) {printf("%c",x); if(--count == 0){printf("\\\n"); count=ll;} }
#define TEST2	{if((count -= 2) <=0){printf("\\\n");count=ll;}}
#define EMPTY if(stkerr != 0){printf("stack empty\n"); continue; }
#define EMPTYR(x) if(stkerr!=0){pushp(x);printf("stack empty\n");continue;}
#define EMPTYS if(stkerr != 0){printf("stack empty\n"); return(1);}
#define EMPTYSR(x) if(stkerr !=0){printf("stack empty\n");pushp(x);return(1);}
#define error(p)	{printf(p); continue; }
#define errorrt(p)	{printf(p); return(1); }

struct blk {
	char	*rd;
	char	*wt;
	char	*beg;
	char	*last;
};

struct sym {
	struct	sym *next;
	struct	blk *val;
};

struct wblk {
	struct blk **rdw;
	struct blk **wtw;
	struct blk **begw;
	struct blk **lastw;
};

static struct blk *hfree;
static struct blk *arg1, *arg2;
static int svargc;
static char savk;
static char **svargv;
static int dbg;
static int ifile;
static FILE *curfile;
static struct blk *scalptr, *basptr, *tenptr, *inbas;
static struct blk *sqtemp, *chptr, *strptr, *divxyz;
static struct blk *stack[STKSZ];
static struct blk **stkptr, **stkbeg;
static struct blk **stkend;
static int stkerr;
static int lastchar;
static struct blk *readstk[RDSKSZ];
static struct blk **readptr;
static struct blk *rem;
static int k;
static struct blk *irem;
static int skd, skr;
static int neg;
static struct sym symlst[TBLSZ];
static struct sym *stable[TBLSZ];
static struct sym *sptr, *sfree;
static FILE *fsave;
static long rel;
static long nbytes;
static long all;
static long headmor;
static long obase;
static int fw, fw1, ll;
static void (*outdit)(struct blk *, int);
static int logo;
static int log10;
static int count;
static char *pp;
static char *dummy;

static void commnds(void);
static struct blk *dc_div(struct blk *, struct blk *);
static int dscale(void);
static struct blk *removr(struct blk *, int);
static struct blk *dc_sqrt(struct blk *);
static struct blk *dc_exp(struct blk *, struct blk *);
static void init(int, char **);
static void onintr(i16);
static void pushp(struct blk *);
static struct blk *pop(void);
static struct blk *readin(void);
static struct blk *add0(struct blk *, int);
static struct blk *mult(struct blk *, struct blk *);
static void chsign(struct blk *);
static int readc(void);
static void unreadc(int);
static void binop(int);
static void print(struct blk *);
static struct blk *getdec(struct blk *, int);
static void tenot(struct blk *, int);
static void oneot(struct blk *, int, int);
static void hexot(struct blk *, int);
static void bigot(struct blk *, int);
static struct blk *add(struct blk *, struct blk *);
static int eqk(void);
static struct blk *removc(struct blk *, int);
static struct blk *scalint(struct blk *);
static struct blk *scale(struct blk *, int);
static int subt(void);
static int command(void);
static int cond(int);
static void load(void);
static int dc_log2(long);
static struct blk *salloc(int);
static struct blk *morehd(void);
static struct blk *copy(struct blk *, int);
static void sdump(char *, struct blk *);
static void seekc(struct blk *, int);
static void salterwd(struct blk *, struct blk *);
static void more(struct blk *);
static void ospace(char *);
static void garbage(char *);
static void redef(struct blk *);
static void release(struct blk *);
static struct blk *getwd(struct blk *);
static void putwd(struct blk *, struct blk *);
static struct blk *lookwd(struct blk *);
static char *nalloc(char *, unsigned);
