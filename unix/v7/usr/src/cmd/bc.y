%{
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef char *bc_value;

static int cpeek(int, int, int);
static int getch(void);
static bc_value bundle(int, ...);
static void routput(bc_value);
static void output(bc_value);
static void conout(bc_value, bc_value);
static void pp(bc_value);
static void tp(bc_value);
static void yyinit(int, char **);
static void getout(void);
static bc_value getf(bc_value);
static bc_value geta(bc_value);

int yylex(void);
int yyerror(char *);
int yyparse(void);
extern bc_value yylval;
%}
%right '='
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMINUS

%term LETTER DIGIT SQRT LENGTH _IF  FFF EQ
%term _WHILE _FOR NE LE GE INCR DECR
%term _RETURN _BREAK _DEFINE BASE OBASE SCALE
%term EQPL EQMI EQMUL EQDIV EQREM EQEXP
%term _AUTO DOT
%term QSTR

%{
static FILE *in;
static char cary[1000], *cp = cary;
static char string[1000], *str = string;
static int crs = '0';
static int rcrs = '0';  /* reset crs */
static int bindx = 0;
static int lev = 0;
static int ln;
static char *ss;
static int bstack[10] = { 0 };
static char *numb[15] = {
  " 0", " 1", " 2", " 3", " 4", " 5",
  " 6", " 7", " 8", " 9", " 10", " 11",
  " 12", " 13", " 14" };
static bc_value pre, post;
%}
%%
start	: 
	|  start stat tail
		= output( $2 );
	|  start def dargs ')' '{' dlist slist '}'
		={	bundle( 6,pre, $7, post ,"0",numb[lev],"Q");
			conout( $$, $2 );
			rcrs = crs;
			output( "" );
			lev = bindx = 0;
			}
	;

dlist	:  tail
	| dlist _AUTO dlets tail
	;

stat	:  e 
		={ bundle(2, $1, "ps." ); }
	| 
		={ bundle(1, "" ); }
	|  QSTR
		={ bundle(3,"[",$1,"]P");}
	|  LETTER '=' e
		={ bundle(3, $3, "s", $1 ); }
	|  LETTER '[' e ']' '=' e
		={ bundle(4, $6, $3, ":", geta($1)); }
	|  LETTER EQOP e
		={ bundle(6, "l", $1, $3, $2, "s", $1 ); }
	|  LETTER '[' e ']' EQOP e
		={ bundle(8,$3, ";", geta($1), $6, $5, $3, ":", geta($1));}
	|  _BREAK
		={ bundle(2, numb[lev-bstack[bindx-1]], "Q" ); }
	|  _RETURN '(' e ')'
		= bundle(4, $3, post, numb[lev], "Q" );
	|  _RETURN '(' ')'
		= bundle(4, "0", post, numb[lev], "Q" );
	| _RETURN
		= bundle(4,"0",post,numb[lev],"Q");
	| SCALE '=' e
		= bundle(2, $3, "k");
	| SCALE EQOP e
		= bundle(4,"K",$3,$2,"k");
	| BASE '=' e
		= bundle(2,$3, "i");
	| BASE EQOP e
		= bundle(4,"I",$3,$2,"i");
	| OBASE '=' e
		= bundle(2,$3,"o");
	| OBASE EQOP e
		= bundle(4,"O",$3,$2,"o");
	|  '{' slist '}'
		={ $$ = $2; }
	|  FFF
		={ bundle(1,"fY"); }
	|  error
		={ bundle(1,"c"); }
	|  _IF CRS BLEV '(' re ')' stat
		={	conout( $7, $2 );
			bundle(3, $5, $2, " " );
			}
	|  _WHILE CRS '(' re ')' stat BLEV
		={	bundle(3, $6, $4, $2 );
			conout( $$, $2 );
			bundle(3, $4, $2, " " );
			}
	|  fprefix CRS re ';' e ')' stat BLEV
		={	bundle(5, $7, $5, "s.", $3, $2 );
			conout( $$, $2 );
			bundle(5, $1, "s.", $3, $2, " " );
			}
	|  '~' LETTER '=' e
		={	bundle(3,$4,"S",$2); }
	;

EQOP	:  EQPL
		={ $$ = "+"; }
	|  EQMI
		={ $$ = "-"; }
	|  EQMUL
		={ $$ = "*"; }
	|  EQDIV
		={ $$ = "/"; }
	|  EQREM
		={ $$ = "%%"; }
	|  EQEXP
		={ $$ = "^"; }
	;

fprefix	:  _FOR '(' e ';'
		={ $$ = $3; }
	;

BLEV	:
		={ --bindx; }
	;

slist	:  stat
	|  slist tail stat
		={ bundle(2, $1, $3 ); }
	;

tail	:  '\n'
		={ln++;}
	|  ';'
	;

re	:  e EQ e
		= bundle(3, $1, $3, "=" );
	|  e '<' e
		= bundle(3, $1, $3, ">" );
	|  e '>' e
		= bundle(3, $1, $3, "<" );
	|  e NE e
		= bundle(3, $1, $3, "!=" );
	|  e GE e
		= bundle(3, $1, $3, "!>" );
	|  e LE e
		= bundle(3, $1, $3, "!<" );
	|  e
		= bundle(2, $1, " 0!=" );
	;

e	:  e '+' e
		= bundle(3, $1, $3, "+" );
	|  e '-' e
		= bundle(3, $1, $3, "-" );
	| '-' e		%prec UMINUS
		= bundle(3, " 0", $2, "-" );
	|  e '*' e
		= bundle(3, $1, $3, "*" );
	|  e '/' e
		= bundle(3, $1, $3, "/" );
	|  e '%' e
		= bundle(3, $1, $3, "%%" );
	|  e '^' e
		= bundle(3, $1, $3, "^" );
	|  LETTER '[' e ']'
		={ bundle(3,$3, ";", geta($1)); }
	|  LETTER INCR
		= bundle(4, "l", $1, "d1+s", $1 );
	|  INCR LETTER
		= bundle(4, "l", $2, "1+ds", $2 );
	|  DECR LETTER
		= bundle(4, "l", $2, "1-ds", $2 );
	|  LETTER DECR
		= bundle(4, "l", $1, "d1-s", $1 );
	| LETTER '[' e ']' INCR
		= bundle(7,$3,";",geta($1),"d1+",$3,":",geta($1));
	| INCR LETTER '[' e ']'
		= bundle(7,$4,";",geta($2),"1+d",$4,":",geta($2));
	| LETTER '[' e ']' DECR
		= bundle(7,$3,";",geta($1),"d1-",$3,":",geta($1));
	| DECR LETTER '[' e ']'
		= bundle(7,$4,";",geta($2),"1-d",$4,":",geta($2));
	| SCALE INCR
		= bundle(1,"Kd1+k");
	| INCR SCALE
		= bundle(1,"K1+dk");
	| SCALE DECR
		= bundle(1,"Kd1-k");
	| DECR SCALE
		= bundle(1,"K1-dk");
	| BASE INCR
		= bundle(1,"Id1+i");
	| INCR BASE
		= bundle(1,"I1+di");
	| BASE DECR
		= bundle(1,"Id1-i");
	| DECR BASE
		= bundle(1,"I1-di");
	| OBASE INCR
		= bundle(1,"Od1+o");
	| INCR OBASE
		= bundle(1,"O1+do");
	| OBASE DECR
		= bundle(1,"Od1-o");
	| DECR OBASE
		= bundle(1,"O1-do");
	|  LETTER '(' cargs ')'
		= bundle(4, $3, "l", getf($1), "x" );
	|  LETTER '(' ')'
		= bundle(3, "l", getf($1), "x" );
	|  cons
		={ bundle(2, " ", $1 ); }
	|  DOT cons
		={ bundle(2, " .", $2 ); }
	|  cons DOT cons
		={ bundle(4, " ", $1, ".", $3 ); }
	|  cons DOT
		={ bundle(3, " ", $1, "." ); }
	|  DOT
		={ $$ = "l."; }
	|  LETTER
		= { bundle(2, "l", $1 ); }
	|  LETTER '=' e
		={ bundle(3, $3, "ds", $1 ); }
	|  LETTER EQOP e	%prec '='
		={ bundle(6, "l", $1, $3, $2, "ds", $1 ); }
	| LETTER '[' e ']' '=' e
		= { bundle(5,$6,"d",$3,":",geta($1)); }
	| LETTER '[' e ']' EQOP e
		= { bundle(9,$3,";",geta($1),$6,$5,"d",$3,":",geta($1)); }
	| LENGTH '(' e ')'
		= bundle(2,$3,"Z");
	| SCALE '(' e ')'
		= bundle(2,$3,"X");	/* must be before '(' e ')' */
	|  '(' e ')'
		= { $$ = $2; }
	|  '?'
		={ bundle(1, "?" ); }
	|  SQRT '(' e ')'
		={ bundle(2, $3, "v" ); }
	| '~' LETTER
		={ bundle(2,"L",$2); }
	| SCALE '=' e
		= bundle(2,$3,"dk");
	| SCALE EQOP e		%prec '='
		= bundle(4,"K",$3,$2,"dk");
	| BASE '=' e
		= bundle(2,$3,"di");
	| BASE EQOP e		%prec '='
		= bundle(4,"I",$3,$2,"di");
	| OBASE '=' e
		= bundle(2,$3,"do");
	| OBASE EQOP e		%prec '='
		= bundle(4,"O",$3,$2,"do");
	| SCALE
		= bundle(1,"K");
	| BASE
		= bundle(1,"I");
	| OBASE
		= bundle(1,"O");
	;

cargs	:  eora
	|  cargs ',' eora
		= bundle(2, $1, $3 );
	;
eora:	  e
	| LETTER '[' ']'
		=bundle(2,"l",geta($1));
	;

cons	:  constant
		={ *cp++ = '\0'; }

constant:
	  '_'
		={ $$ = cp; *cp++ = '_'; }
	|  DIGIT
		={ $$ = cp; *cp++ = *$1; }
	|  constant DIGIT
		={ *cp++ = *$2; }
	;

CRS	:
		={ $$ = cp; *cp++ = crs++; *cp++ = '\0';
			if(crs == '[')crs=+3;
			if(crs == 'a')crs='{';
			if(crs >= 0241){yyerror("program too big");
				getout();
			}
			bstack[bindx++] = lev++; }
	;

def	:  _DEFINE LETTER '('
		={	$$ = getf($2);
			pre = (bc_value)"";
			post = (bc_value)"";
			lev = 1;
			bstack[bindx=0] = 0;
			}
	;

dargs	:
	|  lora
		={ pp( $1 ); }
	|  dargs ',' lora
		={ pp( $3 ); }
	;

dlets	:  lora
		={ tp($1); }
	|  dlets ',' lora
		={ tp($3); }
	;
lora	:  LETTER
	|  LETTER '[' ']'
		={ $$ = geta($1); }
	;

%%
# define error 256

static int peekc = -1;
static int sargc;
static int ifile;
static char **sargv;

static char funtab[52] = {
	01,0,02,0,03,0,04,0,05,0,06,0,07,0,010,0,011,0,012,0,013,0,014,0,015,0,016,0,017,0,
	020,0,021,0,022,0,023,0,024,0,025,0,026,0,027,0,030,0,031,0,032,0 };
static char atab[52] = {
	0241,0,0242,0,0243,0,0244,0,0245,0,0246,0,0247,0,0250,0,0251,0,0252,0,0253,0,
	0254,0,0255,0,0256,0,0257,0,0260,0,0261,0,0262,0,0263,0,0264,0,0265,0,0266,0,
	0267,0,0270,0,0271,0,0272,0};
static char *letr[26] = {
  "a","b","c","d","e","f","g","h","i","j",
  "k","l","m","n","o","p","q","r","s","t",
  "u","v","w","x","y","z" } ;
static char digit_value[256];

int
yylex(void)
{
	int c, ch;
restart:
	c = getch();
	peekc = -1;
	while( c == ' ' || c == '\t' ) c = getch();
	if(c == '\\'){
		getch();
		goto restart;
	}
	if( c<= 'z' && c >= 'a' ) {
		/* look ahead to look for reserved words */
		peekc = getch();
		if( peekc >= 'a' && peekc <= 'z' ){ /* must be reserved word */
			if( c=='i' && peekc=='f' ){ c=_IF; goto skip; }
			if( c=='w' && peekc=='h' ){ c=_WHILE; goto skip; }
			if( c=='f' && peekc=='o' ){ c=_FOR; goto skip; }
			if( c=='s' && peekc=='q' ){ c=SQRT; goto skip; }
			if( c=='r' && peekc=='e' ){ c=_RETURN; goto skip; }
			if( c=='b' && peekc=='r' ){ c=_BREAK; goto skip; }
			if( c=='d' && peekc=='e' ){ c=_DEFINE; goto skip; }
			if( c=='s' && peekc=='c' ){ c= SCALE; goto skip; }
			if( c=='b' && peekc=='a' ){ c=BASE; goto skip; }
			if( c=='i' && peekc == 'b'){ c=BASE; goto skip; }
			if( c=='o' && peekc=='b' ){ c=OBASE; goto skip; }
			if( c=='d' && peekc=='i' ){ c=FFF; goto skip; }
			if( c=='a' && peekc=='u' ){ c=_AUTO; goto skip; }
			if( c == 'l' && peekc=='e'){ c=LENGTH; goto skip; }
			if( c == 'q' && peekc == 'u'){getout();}
			/* could not be found */
			return( error );
		skip:	/* skip over rest of word */
			peekc = -1;
			while( (ch = getch()) >= 'a' && ch <= 'z' );
			peekc = ch;
			return( c );
		}

		/* usual case; just one single letter */

		yylval = letr[c-'a'];
		return( LETTER );
	}
	if( c>= '0' && c <= '9' || c>= 'A' && c<= 'F' ){
		digit_value[c] = (char)c;
		yylval = &digit_value[c];
		return( DIGIT );
	}
	switch( c ){
	case '.':	return( DOT );
	case '=':
		switch( peekc = getch() ){
		case '=': c=EQ; goto gotit;
		case '+': c=EQPL; goto gotit;
		case '-': c=EQMI; goto gotit;
		case '*': c=EQMUL; goto gotit;
		case '/': c=EQDIV; goto gotit;
		case '%': c=EQREM; goto gotit;
		case '^': c=EQEXP; goto gotit;
		default:   return( '=' );
			  gotit:     peekc = -1; return(c);
		  }
	case '+':	return( cpeek( '+', INCR, '+' ) );
	case '-':	return( cpeek( '-', DECR, '-' ) );
	case '<':	return( cpeek( '=', LE, '<' ) );
	case '>':	return( cpeek( '=', GE, '>' ) );
	case '!':	return( cpeek( '=', NE, '!' ) );
	case '/':
		if((peekc = getch()) == '*'){
			peekc = -1;
			while((getch() != '*') || ((peekc = getch()) != '/'));
			peekc = -1;
			goto restart;
		}
		else return(c);
	case '"':	
		 yylval = str;
		 while((c=getch()) != '"'){*str++ = c;
			if(str >= &string[999]){yyerror("string space exceeded");
			getout();
		}
	}
	 *str++ = '\0';
	return(QSTR);
	default:	 return( c );
	}
}

static int
cpeek(int c, int yes, int no)
{
	if( (peekc=getch()) != c ) return( no );
	else {
		peekc = -1;
		return( yes );
	}
}

static int
getch(void)
{
	int ch;
loop:
	ch = (peekc < 0) ? getc(in) : peekc;
	peekc = -1;
	if(ch != EOF)return(ch);
	if(++ifile > sargc){
		if(ifile >= sargc+2)getout();
		in = stdin;
		ln = 0;
		goto loop;
	}
	fclose(in);
	if((in = fopen(sargv[ifile],"r")) != NULL){
		ln = 0;
		ss = sargv[ifile];
		goto loop;
	}
	yyerror("cannot open input file");
}
# define b_sp_max 3000
static bc_value b_space[b_sp_max];
static bc_value *b_sp_nxt = b_space;

static int bdebug = 0;

static bc_value
bundle(int count, ...)
{
	int i;
	bc_value *q;
	va_list ap;

	q = b_sp_nxt;
	if (bdebug)
		printf("bundle %d elements\n", count);
	va_start(ap, count);
	for (i = 0; i < count; i++) {
		if( b_sp_nxt >= & b_space[b_sp_max] ) yyerror( "bundling space exceeded" );
		*b_sp_nxt++ = va_arg(ap, bc_value);
	}
	va_end(ap);
	*b_sp_nxt++ = 0;
	yyval = (bc_value)q;
	return((bc_value)q);
}

static void
routput(bc_value value)
{
	bc_value *p;

	p = (bc_value *)value;
	if (bdebug)
		printf("routput\n");
	if( p >= &b_space[0] && p < &b_space[b_sp_max]){
		/* part of a bundle */
		while (*p != 0)
			routput(*p++);
	}
	else
		printf("%s", value);	 /* character string */
}

static void
output(bc_value p)
{
	routput(p);
	b_sp_nxt = & b_space[0];
	printf( "\n" );
	fflush(stdout);
	cp = cary;
	crs = rcrs;
}

static void
conout(bc_value p, bc_value s)
{
	printf("[");
	routput(p);
	printf("]s%s\n", s);
	fflush(stdout);
	lev--;
}

int
yyerror(char *s)
{
	if(ifile > sargc)ss="teletype";
	printf("c[%s on line %d, %s]pc\n", s ,ln+1,ss);
	fflush(stdout);
	cp = cary;
	crs = rcrs;
	bindx = 0;
	lev = 0;
	b_sp_nxt = &b_space[0];
	return 0;
}

static void
pp(bc_value s)
{
	/* puts the relevant stuff on pre and post for the letter s */

	bundle(3, "S", s, pre );
	pre = yyval;
	bundle(4, post, "L", s, "s." );
	post = yyval;
}

static void
tp(bc_value s)
{ /* same as pp, but for temps */
	bundle(3, "0S", s, pre );
	pre = yyval;
	bundle(4, post, "L", s, "s." );
	post = yyval;
}

static void
yyinit(int argc, char **argv)
{
	signal(SIGINT, SIG_IGN);	/* ignore all interrupts */
	sargv=argv;
	sargc= -- argc;
	if(sargc == 0)in=stdin;
	else if((in = fopen(sargv[1],"r")) == NULL)
		yyerror("cannot open input file");
	ifile = 1;
	ln = 0;
	ss = sargv[1];
}
static void
getout(void)
{
	printf("q");
	fflush(stdout);
	exit(0);
}

static bc_value
getf(bc_value value)
{
	char *p;

	p = value;
	return((bc_value)&funtab[2*(*p -0141)]);
}

static bc_value
geta(bc_value value)
{
	char *p;

	p = value;
	return((bc_value)&atab[2*(*p - 0141)]);
}

int
main(int argc, char **argv)
{
	i16 p[2];


	if (argc > 1 && *argv[1] == '-') {
		if((argv[1][1] == 'd')||(argv[1][1] == 'c')){
			yyinit(--argc, ++argv);
			yyparse();
			exit(0);
		}
		if(argv[1][1] != 'l'){
			printf("unrecognizable argument\n");
			fflush(stdout);
			exit(1);
		}
		argv[1] = "/usr/lib/lib.b";
	}
	pipe(p);
	if (fork()==0) {
		close(1);
		dup(p[1]);
		close(p[0]);
		close(p[1]);
		yyinit(argc, argv);
		yyparse();
		exit(0);
	}
	close(0);
	dup(p[0]);
	close(p[0]);
	close(p[1]);
	execl("/bin/dc", "dc", "-", (char *)0);
	execl("/usr/bin/dc", "dc", "-", (char *)0);
	return 1;
}
