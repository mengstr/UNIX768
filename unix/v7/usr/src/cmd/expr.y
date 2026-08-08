/* Yacc productions for "expr" command: */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *rel(int, char *, char *);
static char *arith(int, char *, char *);
static char *conj(int, char *, char *);
static char *expr_substr(char *, char *, char *);
static char *expr_length(char *);
static char *expr_index(char *, char *);
static char *expr_match(char *, char *);
static int ematch(char *, char *);
static void errxx(int);
static char *expr_compile(char *, char *, char *, int);
static int step(char *, char *);
static int advance(char *, char *);
static void getrnge(char *);
static int ecmp(char *, char *, int);
static long expr_atol(char *);

int yylex(void);
int yyerror(char *);
int yyparse(void);
extern long yylval;
%}

%token OR AND ADD SUBT MULT DIV REM EQ GT GEQ LT LEQ NEQ
%token A_STRING SUBSTR LENGTH INDEX NOARG MATCH

/* operators listed below in increasing precedence: */
%left OR
%left AND
%left EQ LT GT GEQ LEQ NEQ
%left ADD SUBT
%left MULT DIV REM
%left MCH
%left MATCH
%left SUBSTR
%left LENGTH INDEX
%%

/* a single `expression' is evaluated and printed: */

expression:	expr NOARG = {
			printf("%s\n", $1);
			exit((!strcmp($1,"0")||!strcmp($1,"\0"))? 1: 0);
			}
	;


expr:	'(' expr ')' = { $$ = $2; }
	| expr OR expr   = { $$ = conj(OR, $1, $3); }
	| expr AND expr   = { $$ = conj(AND, $1, $3); }
	| expr EQ expr   = { $$ = rel(EQ, $1, $3); }
	| expr GT expr   = { $$ = rel(GT, $1, $3); }
	| expr GEQ expr   = { $$ = rel(GEQ, $1, $3); }
	| expr LT expr   = { $$ = rel(LT, $1, $3); }
	| expr LEQ expr   = { $$ = rel(LEQ, $1, $3); }
	| expr NEQ expr   = { $$ = rel(NEQ, $1, $3); }
	| expr ADD expr   = { $$ = arith(ADD, $1, $3); }
	| expr SUBT expr   = { $$ = arith(SUBT, $1, $3); }
	| expr MULT expr   = { $$ = arith(MULT, $1, $3); }
	| expr DIV expr   = { $$ = arith(DIV, $1, $3); }
	| expr REM expr   = { $$ = arith(REM, $1, $3); }
	| expr MCH expr	 = { $$ = expr_match($1, $3); }
	| MATCH expr expr = { $$ = expr_match($2, $3); }
	| SUBSTR expr expr expr = { $$ = expr_substr($2, $3, $4); }
	| LENGTH expr       = { $$ = expr_length($2); }
	| INDEX expr expr = { $$ = expr_index($2, $3); }
	| A_STRING
	;
%%
/*	expression command */
#define ESIZE	256
#define error(c)	errxx(c)
#define EQL(x,y) !strcmp(x,y)
char	**Av;
int	Ac;
int	Argi;

char Mstring[1][128];
extern int nbra;

int main(int argc, char **argv) {
	Ac = argc;
	Argi = 1;
	Av = argv;
	return yyparse();
}

char *operators[] = { "|", "&", "+", "-", "*", "/", "%", ":",
	"=", "==", "<", "<=", ">", ">=", "!=",
	"match", "substr", "length", "index", "\0" };
int op[] = { OR, AND, ADD,  SUBT, MULT, DIV, REM, MCH,
	EQ, EQ, LT, LEQ, GT, GEQ, NEQ,
	MATCH, SUBSTR, LENGTH, INDEX };
int yylex(void) {
	register char *p;
	register i;

	if(Argi >= Ac) return NOARG;

	p = Av[Argi++];

	if(*p == '(' || *p == ')')
		return (int)*p;
	for(i = 0; *operators[i]; ++i)
		if(EQL(operators[i], p))
			return op[i];

	yylval = p;
	return A_STRING;
}

static char *rel(op, r1, r2) int op; register char *r1, *r2; {
	register i;

	if(ematch(r1, "-*[0-9]*$") && ematch(r2, "[0-9]*$"))
		i = expr_atol(r1) - expr_atol(r2);
	else
		i = strcmp(r1, r2);
	switch(op) {
	case EQ: i = i==0; break;
	case GT: i = i>0; break;
	case GEQ: i = i>=0; break;
	case LT: i = i<0; break;
	case LEQ: i = i<=0; break;
	case NEQ: i = i!=0; break;
	}
	return i? "1": "0";
}

static char *arith(op, r1, r2) int op; char *r1, *r2; {
	long i1, i2;
	register char *rv;

	if(!(ematch(r1, "[0-9]*$") && ematch(r2, "[0-9]*$")))
		yyerror("non-numeric argument");
	i1 = expr_atol(r1);
	i2 = expr_atol(r2);

	switch(op) {
	case ADD: i1 = i1 + i2; break;
	case SUBT: i1 = i1 - i2; break;
	case MULT: i1 = i1 * i2; break;
	case DIV: i1 = i1 / i2; break;
	case REM: i1 = i1 % i2; break;
	}
	rv = malloc(16);
	sprintf(rv, "%ld", i1);
	return rv;
}
static char *conj(op, r1, r2) int op; char *r1, *r2; {
	register char *rv;

	switch(op) {

	case OR:
		if(EQL(r1, "0")
		|| EQL(r1, ""))
			if(EQL(r2, "0")
			|| EQL(r2, ""))
				rv = "0";
			else
				rv = r2;
		else
			rv = r1;
		break;
	case AND:
		if(EQL(r1, "0")
		|| EQL(r1, ""))
			rv = "0";
		else if(EQL(r2, "0")
		|| EQL(r2, ""))
			rv = "0";
		else
			rv = r1;
		break;
	}
	return rv;
}

static char *expr_substr(v, s, w) char *v, *s, *w; {
register si, wi;
register char *res;

	si = expr_atol(s);
	wi = expr_atol(w);
	while(--si) if(*v) ++v;

	res = v;

	while(wi--) if(*v) ++v;

	*v = '\0';
	return res;
}

static char *expr_length(s) register char *s; {
	register i = 0;
	register char *rv;

	while(*s++) ++i;

	rv = malloc(8);
	sprintf(rv, "%d", i);
	return rv;
}

static char *expr_index(s, t) char *s, *t; {
	register i, j;
	register char *rv;

	for(i = 0; s[i] ; ++i)
		for(j = 0; t[j] ; ++j)
			if(s[i]==t[j]) {
				sprintf(rv = malloc(8), "%d", ++i);
				return rv;
			}
	return "0";
}

static char *expr_match(s, p)
char *s, *p;
{
	register char *rv;

	sprintf(rv = malloc(8), "%d", ematch(s, p));
	if(nbra) {
		rv = malloc(strlen(Mstring[0])+1);
		strcpy(rv, Mstring[0]);
	}
	return rv;
}

#define INIT	register char *sp = instring;
#define GETC()		(*sp++)
#define PEEKC()		(*sp)
#define UNGETC(c)	(--sp)
#define RETURN(c)	return(c)
#define ERROR(c)	errxx(c)


static int ematch(s, p)
char *s;
register char *p;
{
	static char expbuf[ESIZE];
	register num;
	extern char *braslist[], *braelist[], *loc2;

	expr_compile(p, expbuf, &expbuf[ESIZE], 0);
	if(nbra > 1)
		yyerror("Too many '\\('s");
	if(advance(s, expbuf)) {
		if(nbra == 1) {
			p = braslist[0];
			num = braelist[0] - p;
			strncpy(Mstring[0], p, num);
			Mstring[0][num] = '\0';
		}
		return(loc2-s);
	}
	return(0);
}

static void errxx(c)
int c;
{
	(void)c;
	yyerror("RE error");
}

#define	CBRA	2
#define	CCHR	4
#define	CDOT	8
#define	CCL	12
#define	CDOL	20
#define	CEOF	22
#define	CKET	24
#define	CBACK	36

#define	STAR	01
#define RNGE	03

#define	NBRA	9

#define PLACE(c)	ep[c >> 3] |= bittab[c & 07]
#define ISTHERE(c)	(ep[c >> 3] & bittab[c & 07])

char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra;
char *loc1, *loc2, *locs;
int	sed;

int	circf;
int	low;
int	size;

char	bittab[] = {
	1,
	2,
	4,
	8,
	16,
	32,
	64,
	128
};

static char *expr_compile(instring, ep, endbuf, seof)
register char *ep;
char *instring, *endbuf;
{
	INIT	/* Dependent declarations and initializations */
	register c;
	register eof = seof;
	char *lastep = instring;
	int cclcnt;
	char bracket[NBRA], *bracketp;
	int closed;
	char neg;
	int lc;
	int i, cflg;

	lastep = 0;
	if((c = GETC()) == eof) {
		if(*ep == 0 && !sed)
			ERROR(41);
		RETURN(ep);
	}
	bracketp = bracket;
	circf = closed = nbra = 0;
	if (c == '^')
		circf++;
	else
		UNGETC(c);
	for (;;) {
		if (ep >= endbuf)
			ERROR(50);
		if((c = GETC()) != '*' && ((c != '\\') || (PEEKC() != '{')))
			lastep = ep;
		if (c == eof) {
			*ep++ = CEOF;
			RETURN(ep);
		}
		switch (c) {

		case '.':
			*ep++ = CDOT;
			continue;

		case '\n':
			ERROR(36);
		case '*':
			if (lastep==0 || *lastep==CBRA || *lastep==CKET)
				goto defchar;
			*lastep |= STAR;
			continue;

		case '$':
			if(PEEKC() != eof)
				goto defchar;
			*ep++ = CDOL;
			continue;

		case '[':
			if(&ep[17] >= endbuf)
				ERROR(50);

			*ep++ = CCL;
			lc = 0;
			for(i = 0; i < 16; i++)
				ep[i] = 0;

			neg = 0;
			if((c = GETC()) == '^') {
				neg = 1;
				c = GETC();
			}

			do {
				if(c == '\0' || c == '\n')
					ERROR(49);
				if(c == '-' && lc != 0) {
					if ((c = GETC()) == ']') {
						PLACE('-');
						break;
					}
					while(lc < c) {
						PLACE(lc);
						lc++;
					}
				}
				lc = c;
				PLACE(c);
			} while((c = GETC()) != ']');
			if(neg) {
				for(cclcnt = 0; cclcnt < 16; cclcnt++)
					ep[cclcnt] ^= -1;
				ep[0] &= 0376;
			}

			ep += 16;

			continue;

		case '\\':
			switch(c = GETC()) {

			case '(':
				if(nbra >= NBRA)
					ERROR(43);
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;

			case ')':
				if(bracketp <= bracket)
					ERROR(42);
				*ep++ = CKET;
				*ep++ = *--bracketp;
				closed++;
				continue;

			case '{':
				if(lastep == (char *) (0))
					goto defchar;
				*lastep |= RNGE;
				cflg = 0;
			nlim:
				c = GETC();
				i = 0;
				do {
					if ('0' <= c && c <= '9')
						i = 10 * i + c - '0';
					else
						ERROR(16);
				} while(((c = GETC()) != '\\') && (c != ','));
				if (i > 255)
					ERROR(11);
				*ep++ = i;
				if (c == ',') {
					if(cflg++)
						ERROR(44);
					if((c = GETC()) == '\\')
						*ep++ = 255;
					else {
						UNGETC(c);
						goto nlim; /* get 2'nd number */
					}
				}
				if(GETC() != '}')
					ERROR(45);
				if(!cflg)	/* one number */
					*ep++ = i;
				else if((ep[-1] & 0377) < (ep[-2] & 0377))
					ERROR(46);
				continue;

			case '\n':
				ERROR(36);

			case 'n':
				c = '\n';
				goto defchar;

			default:
				if(c >= '1' && c <= '9') {
					if((c -= '1') >= closed)
						ERROR(25);
					*ep++ = CBACK;
					*ep++ = c;
					continue;
				}
			}
			/* Drop through to default to use \ to turn off special chars */

		defchar:
		default:
			lastep = ep;
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
}

static int step(p1, p2)
register char *p1, *p2;
{
	register c;

	if (circf) {
		loc1 = p1;
		return(advance(p1, p2));
	}
	/* fast check for first character */
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1 != c)
				continue;
			if (advance(p1, p2)) {
				loc1 = p1;
				return(1);
			}
		} while (*p1++);
		return(0);
	}
		/* regular algorithm */
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return(1);
		}
	} while (*p1++);
	return(0);
}

static int advance(lp, ep)
register char *lp, *ep;
{
	register char *curlp;
	char c;
	char *bbeg;
	int ct;

	for (;;) switch (*ep++) {

	case CCHR:
		if (*ep++ == *lp++)
			continue;
		return(0);

	case CDOT:
		if (*lp++)
			continue;
		return(0);

	case CDOL:
		if (*lp==0)
			continue;
		return(0);

	case CEOF:
		loc2 = lp;
		return(1);

	case CCL:
		c = *lp++ & 0177;
		if(ISTHERE(c)) {
			ep += 16;
			continue;
		}
		return(0);
	case CBRA:
		braslist[*ep++] = lp;
		continue;

	case CKET:
		braelist[*ep++] = lp;
		continue;

	case CCHR|RNGE:
		c = *ep++;
		getrnge(ep);
		while(low--)
			if(*lp++ != c)
				return(0);
		curlp = lp;
		while(size--) 
			if(*lp++ != c)
				break;
		if(size < 0)
			lp++;
		ep += 2;
		goto star;

	case CDOT|RNGE:
		getrnge(ep);
		while(low--)
			if(*lp++ == '\0')
				return(0);
		curlp = lp;
		while(size--)
			if(*lp++ == '\0')
				break;
		if(size < 0)
			lp++;
		ep += 2;
		goto star;

	case CCL|RNGE:
		getrnge(ep + 16);
		while(low--) {
			c = *lp++ & 0177;
			if(!ISTHERE(c))
				return(0);
		}
		curlp = lp;
		while(size--) {
			c = *lp++ & 0177;
			if(!ISTHERE(c))
				break;
		}
		if(size < 0)
			lp++;
		ep += 18;		/* 16 + 2 */
		goto star;

	case CBACK:
		bbeg = braslist[*ep];
		ct = braelist[*ep++] - bbeg;

		if(ecmp(bbeg, lp, ct)) {
			lp += ct;
			continue;
		}
		return(0);

	case CBACK|STAR:
		bbeg = braslist[*ep];
		ct = braelist[*ep++] - bbeg;
		curlp = lp;
		while(ecmp(bbeg, lp, ct))
			lp += ct;

		while(lp >= curlp) {
			if(advance(lp, ep))	return(1);
			lp -= ct;
		}
		return(0);


	case CDOT|STAR:
		curlp = lp;
		while (*lp++);
		goto star;

	case CCHR|STAR:
		curlp = lp;
		while (*lp++ == *ep);
		ep++;
		goto star;

	case CCL|STAR:
		curlp = lp;
		do {
			c = *lp++ & 0177;
		} while(ISTHERE(c));
		ep += 16;
		goto star;

	star:
		do {
			if(--lp == locs)
				break;
			if (advance(lp, ep))
				return(1);
		} while (lp > curlp);
		return(0);

	}
}

static void getrnge(str)
register char *str;
{
	low = *str++ & 0377;
	size = *str == 255 ? 20000 : (*str &0377) - low;
}

static int ecmp(a, b, count)
register char	*a, *b;
register	count;
{
	if(a == b) /* should have been caught in compile() */
		error(51);
	while(count--)
		if(*a++ != *b++)	return(0);
	return(1);
}

static long expr_atol(s)
register char *s;
{
	register int neg;
	register long n;

	neg = 0;
	if (*s == '-') {
		neg = 1;
		s++;
	}
	n = 0;
	while (*s >= '0' && *s <= '9')
		n = n * 10 + *s++ - '0';
	return neg ? -n : n;
}

int yyerror(s)
char *s;
{
	fprintf(stderr, "%s\n", s);
	exit(2);
	return 2;
}
