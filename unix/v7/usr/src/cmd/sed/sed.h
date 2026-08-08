#
/*
 * sed -- stream  editor
 *
 *
 */

#define CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	CNL	8
#define	CDOL	10
#define	CEOF	11
#define CKET	12
#define CNULL	13
#define CLNUM	14
#define CEND	16
#define CDONT	17
#define	CBACK	18

#define	STAR	01

#define NLINES	256
#define	DEPTH	20
#define PTRSIZE	100
#define RESIZE	5000
#define	ABUFSIZE	20
#define	LBSIZE	4000
#define	ESIZE	256
#define	LABSIZE	50
#define NBRA	9

#ifdef SED_EXTERN
#define SED_STORAGE	extern
#else
#define SED_STORAGE
#endif

SED_STORAGE FILE	*fin;
SED_STORAGE union reptr	*abuf[ABUFSIZE];
SED_STORAGE union reptr **aptr;
SED_STORAGE char	*lastre;
SED_STORAGE char	ibuf[512];
SED_STORAGE char	*cbp;
SED_STORAGE char	*ebp;
SED_STORAGE char	genbuf[LBSIZE];
SED_STORAGE char	*loc1;
SED_STORAGE char	*loc2;
SED_STORAGE char	*locs;
SED_STORAGE char	seof;
SED_STORAGE char	*reend;
SED_STORAGE char	*lbend;
SED_STORAGE char	*hend;
SED_STORAGE char	*lcomend;
SED_STORAGE union reptr	*ptrend;
SED_STORAGE int	eflag;
SED_STORAGE int	dolflag;
SED_STORAGE int	sflag;
SED_STORAGE int	jflag;
SED_STORAGE int	numbra;
SED_STORAGE int	delflag;
SED_STORAGE long	lnum;
SED_STORAGE char	linebuf[LBSIZE+1];
SED_STORAGE char	holdsp[LBSIZE+1];
SED_STORAGE char	*spend;
SED_STORAGE char	*hspend;
SED_STORAGE int	nflag;
SED_STORAGE int	gflag;
SED_STORAGE char	*braelist[NBRA];
SED_STORAGE char	*braslist[NBRA];
SED_STORAGE long	tlno[NLINES];
SED_STORAGE int	nlno;
SED_STORAGE char	fname[12][40];
SED_STORAGE FILE	*fcode[12];
SED_STORAGE int	nfiles;

#define ACOM	01
#define BCOM	020
#define CCOM	02
#define	CDCOM	025
#define	CNCOM	022
#define COCOM	017
#define	CPCOM	023
#define DCOM	03
#define ECOM	015
#define EQCOM	013
#define FCOM	016
#define GCOM	027
#define CGCOM	030
#define HCOM	031
#define CHCOM	032
#define ICOM	04
#define LCOM	05
#define NCOM	012
#define PCOM	010
#define QCOM	011
#define RCOM	06
#define SCOM	07
#define TCOM	021
#define WCOM	014
#define	CWCOM	024
#define	YCOM	026
#define XCOM	033

SED_STORAGE char	*cp;
SED_STORAGE char	*reend;
SED_STORAGE char	*lbend;

SED_STORAGE union	reptr {
	struct reptr1 {
		char	*ad1;
		char	*ad2;
		char	*re1;
		char	*rhs;
		FILE	*fcode;
		char	command;
		char	gfl;
		char	pfl;
		char	inar;
		char	negfl;
	} r1;
	struct reptr2 {
		char	*ad1;
		char	*ad2;
		union reptr	*lb1;
		char	*rhs;
		FILE	*fcode;
		char	command;
		char	gfl;
		char	pfl;
		char	inar;
		char	negfl;
	} r2;
} ptrspace[PTRSIZE], *rep;

#define	ad1	r1.ad1
#define	ad2	r1.ad2
#define	re1	r1.re1
#define	rhs	r1.rhs
#define	gfl	r1.gfl
#define	pfl	r1.pfl
#define	inar	r1.inar
#define	negfl	r1.negfl
#define	lb1	r2.lb1


SED_STORAGE char	respace[RESIZE];

SED_STORAGE struct label {
	char	asc[9];
	union reptr	*chain;
	union reptr	*address;
} ltab[LABSIZE];

SED_STORAGE struct label	*lab;
SED_STORAGE struct label	*labend;

SED_STORAGE int	f;
SED_STORAGE int	depth;

SED_STORAGE int	eargc;
SED_STORAGE char	**eargv;

extern	char	bittab[];

SED_STORAGE union reptr	**cmpend[DEPTH];
SED_STORAGE int	depth;
SED_STORAGE union reptr	*pending;
SED_STORAGE char	*badp;
SED_STORAGE char	bad;
char *compile(char *expbuf);
char *ycomp(char *expbuf);
char *address(char *expbuf);
char *text(char *textbuf);
char *compsub(char *rhsbuf);
struct label *search(struct label *ptr);
char *gline(char *addr);
char *place(char *asp, char *al1, char *al2);
int fcomp(void);
int rline(char *lbuf);
int dechain(void);
int execute(char *file);
int match(char *expbuf, int gf);
int advance(char *alp, char *aep);
int substitute(union reptr *ipc);
int dosub(char *rhsbuf);
int command(union reptr *ipc);
int ecmp(char *a, char *b, int count);
int arout(void);
int cmp(char *a, char *b);
SED_STORAGE char	compfl;
