# define NUMC 128
# define NUMS 3
# define	NUMA	8
# define PTRI 1600
# define CHARI 16000
# define OUT	3

# define SKIP	0
# define COLLECT	1
# define SKIP2	2
# define WIDTH 6
struct	tempent {
		char	*beg;
		int	ct;
		long	wdno;
	};

struct	htab {
		int	hsiz;
		int	ssiz;
		int	nsym;
		int	curb;
		int	*hptr;
		char	*symt;
		};

extern struct tempent temp[30];

extern int	lflag;
extern int	puncfl;
extern int	hsw;
extern int	san;
extern int	t1;

extern char	tab[NUMS][NUMC];

int	coll(void);
int	save(void);
int	hyphen(void);
int	ctout(void);
int	bsp(void);
int	gobble2(void);
int	bslash(void);
int	punc(void);
int	search(char *, int, struct htab *, int);
int	put(char *, int);
int	flsh(void);

extern char	line[300];
extern int	l;
extern int	lno;
extern int	c;

extern int	only;
extern int	cs;
extern int	(*flag[8])(void);
extern int	fl;
extern int	wdflg;
extern long	wdnum;
extern char	num[WIDTH + 1];
extern int	igflg;

extern struct htab itab;

extern int	ipsp[PTRI];
extern char	issp[CHARI];

extern FILE	*fi;
