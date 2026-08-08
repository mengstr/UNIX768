#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NF 10
#define NL 300
#define NC 200
#define SL 100
#define NA 10

int tflag;
int xx[NL];
char score[NL];
int rights;
int wrongs;
int guesses;
FILE *input;
int nl = 0;
int na = NA;
int inc;
int ptr  = 0;
int nc = 0;
char line[150];
char response[100];
char *tmp[NF];
int select[NF];
static i32 readline(void);
static i32 cmp(char *left, char *right);
static i32 disj(i32 strict);
static i32 string(i32 strict);
static i32 eat(i32 strict, char c);
static i32 fold(char c);
static void publish(char *text);
static void pub1(i32 show);
static i32 segment(char *text, char *words[]);
static i32 perm(char *left[], int left_count, char *right[], int right_count,
	int permutation[]);
static i32 find(char *words[], int word_count);
static void readindex(void);
static void talloc(void);
static i32 query(char *reply);
static i32 next(void);
static void done(i16 signo);
static void instruct(char *info);
static void badinfo(void);
static void dunno(void);

static i32
readline(void)
{
	char *t;
	register c;
loop:
	for (t=line; c=getc(input), *t=c, c!=EOF; t++) {
		nc++;
		if(*t==' '&&(t==line||t[-1]==' '))
			t--;
		if(*t=='\n') {
			if(t[-1]=='\\')		/*inexact test*/
				continue;
			while(t>line&&t[-1]==' ')
				*--t = '\n';
			*++t = 0;
			return(1);
		}
		if(t-line>=NC) {
			printf("Too hard for me\n");
			do {
				*line = getc(input);
				if(*line==0377)
					return(0);
			} while(*line!='\n');
			goto loop;
		}
	}
	return(0);
}

char *eu;
char *ev;
static i32
cmp(char *u, char *v)
{
	int x;
	eu = u;
	ev = v;
	x = disj(1);
	if(x!=1)
		return(x);
	return(eat(1,0));
}

static i32
disj(i32 s)
{
	int t, x;
	char *u;
	u = eu;
	t = 0;
	for(;;) {
		x = string(s);
		if(x>1)
			return(x);
		switch(*ev) {
		case 0:
		case ']':
		case '}':
			return(t|x&s);
		case '|':
			ev++;
			t |= s;
			s = 0;
			continue;
		}
		if(s) eu = u;
		if(string(0)>1)
			return(2);
		switch(*ev) {
		case 0:
		case ']':
			return(0);
		case '}':
			return(1);
		case '|':
			ev++;
			continue;
		default:
			return(2);
		}
	}
}

static i32
string(i32 s)
{
	int x;
	for(;;) {
		switch(*ev) {
		case 0:
		case '|':
		case ']':
		case '}':
			return(1);
		case '\\':
			ev++;
			if(*ev==0)
				return(2);
			if(*ev=='\n') {
				ev++;
				continue;
			}
		default:
			if(eat(s,*ev)==1)
				continue;
			return(0);
		case '[':
			ev++;
			x = disj(s);
			if(*ev!=']' || x>1)
				return(2);
			ev++;
			if(s==0)
				continue;
			if(x==0)
				return(0);
			continue;
		case '{':
			ev++;
			x = disj(s);
			if(*ev!='}'||x>1)
				return(2);
			ev++;
			continue;
		}
	}
}

static i32
eat(i32 s, char c)
{
	if(*ev!=c)
		return(2);
	if(s==0) {
		ev++;
		return(1);
	}
	if(fold(*eu)!=fold(c))
		return(0);
	eu++;
	ev++;
	return(1);
}

static i32
fold(char c)
{
	if(c<'A'||c>'Z')
		return(c);
	return(c|040);
}

static void
publish(char *t)
{
	ev = t;
	pub1(1);
}

static void
pub1(i32 s)
{
	for(;;ev++){
		switch(*ev) {
		case '|':
			s = 0;
			continue;
		case ']':
		case '}':
		case 0:
			return;
		case '[':
		case '{':
			ev++;
			pub1(s);
			continue;
		case '\\':
			if(*++ev=='\n')
				continue;
		default:
			if(s)
				putchar(*ev);
		}
	}
}

static i32
segment(char *u, char *w[])
{
	char *s;
	int i;
	char *t;
	s = u;
	for(i=0;i<NF;i++) {
		u = s;
		t = w[i];
		while(*s!=':'&&*s!='\n'&&s-u<SL) {
			if(*s=='\\')  {
				if(s[1] == '\n') {
					s += 2;
					continue;
				}
				*t++ = *s++;
			}
			*t++ = *s++;
		}

		while(*s!=':'&&*s!='\n')
			s++;
		*t = 0;
		if(*s++=='\n') {
			return(i+1);
		}
	}
	printf("Too many facts about one thing\n");
	return(0);
}

static i32
perm(char *u[], int m, char *v[], int n, int p[])
{
	int i, j;
	int x;
	for(i=0;i<m;i++) {
		for(j=0;j<n;j++) {
			x = cmp(u[i],v[j]);
			if(x>1) badinfo();
			if(x==0)
				continue;
			p[i] = j;
			goto uloop;
		}
		return(0);
uloop:		;
	}
	return(1);
}

static i32
find(char *u[], int m)
{
	int n;
	while(readline()){
		n = segment(line,tmp);
		if(perm(u,m,tmp+1,n-1,select))
			return(1);
	}
	return(0);
}

static void
readindex(void)
{
	xx[0] = nc = 0;
	while(readline()) {
		xx[++nl] = nc;
		if(nl>=NL) {
			printf("I've forgotten some of it;\n");
			printf("I remember %d items.\n", nl);
			break;
		}
	}
}

static void
talloc(void)
{
	int i;
	for(i=0;i<NF;i++)
		tmp[i] = malloc(SL);
}

int
main(int argc, char *argv[])
{
	register j;
	int i;
	int x;
	int z;
	char *info;
	time_t tm;
	int count;
	info = "/usr/games/quiz.k/index";
	time(&tm);
	inc = (int)tm&077774|01;
loop:
	if(argc>1&&*argv[1]=='-') {
		switch(argv[1][1]) {
		case 'i':
			if(argc>2) 
				info = argv[2];
			argc -= 2;
			argv += 2;
			goto loop;
		case 't':
			tflag = 1;
			argc--;
			argv++;
			goto loop;
		}
	}
	input = fopen(info,"r");
	if(input==NULL) {
		printf("No info\n");
		exit(0);
	}
	talloc();
	if(argc<=2)
		instruct(info);
	signal(SIGINT, done);
	argv[argc] = 0;
	if(find(&argv[1],argc-1)==0)
		dunno();
	fclose(input);
	input = fopen(tmp[0],"r");
	if(input==NULL)
		dunno();
	readindex();
	if(!tflag || na>nl)
		na = nl;
	stdout->_flag |= _IONBF;
	for(;;) {
		i = next();
		fseek(input,xx[i]+0L,0);
		z = xx[i+1]-xx[i];
		for(j=0;j<z;j++)
			line[j] = getc(input);
		segment(line,tmp);
		if(*tmp[select[0]] == '\0' || *tmp[select[1]] == '\0') {
			score[i] = 1;
			continue;
		}
		publish(tmp[select[0]]);
		printf("\n");
		for(count=0;;count++) {
			if(query(response)==0) {
				publish(tmp[select[1]]);
				printf("\n");
				if(count==0) wrongs++;
				score[i] = tflag?-1:1;
				break;
			}
			x = cmp(response,tmp[select[1]]);
			if(x>1) badinfo();
			if(x==1) {
				printf("Right!\n");
				if(count==0) rights++;
				if(++score[i]>=1 && na<nl)
					na++;
				break;
			}
			printf("What?\n");
			if(count==0) wrongs++;
			score[i] = tflag?-1:1;
		}
		guesses += count;
	}
}

static i32
query(char *r)
{
	char *t;
	for(t=r;;t++) {
		if(read(0,t,1)==0)
			done(0);
		if(*t==' '&&(t==r||t[-1]==' '))
			t--;
		if(*t=='\n') {
			while(t>r&&t[-1]==' ')
				*--t = '\n';
			break;
		}
	}
	*t = 0;
	return(t-r);
}

static i32
next(void)
{
	int flag;
	inc = inc*3125&077777;
	ptr = (inc>>2)%na;
	flag = 0;
	while(score[ptr]>0)
		if(++ptr>=na) {
			ptr = 0;
			if(flag) done(0);
			flag = 1;
		}
	return(ptr);
}

static void
done(i16 signo)
{
	(void)signo;
	printf("\nRights %d, wrongs %d, ", rights, wrongs);
	if(guesses)
		printf("extra guesses %d, ", guesses);
	printf("score %d%%\n",100*rights/(rights+wrongs));
	exit(0);
}
static void
instruct(char *info)
{
	int i, n;
	printf("Subjects:\n\n");
	while(readline()) {
		printf("-");
		n = segment(line,tmp);
		for(i=1;i<n;i++) {
			printf(" ");
			publish(tmp[i]);
		}
		printf("\n");
	}
	printf("\n");
	input = fopen(info,"r");
	if(input==NULL)
		abort();
	readline();
	segment(line,tmp);
	printf("For example,\n");
	printf("    quiz ");
	publish(tmp[1]);
	printf(" ");
	publish(tmp[2]);
	printf("\nasks you a ");
	publish(tmp[1]);
	printf(" and you answer the ");
	publish(tmp[2]);
	printf("\n    quiz ");
	publish(tmp[2]);
	printf(" ");
	publish(tmp[1]);
	printf("\nworks the other way around\n");
	printf("\nType empty line to get correct answer.\n");
	exit(0);
}

static void
badinfo(void)
{
	printf("Bad info %s\n",line);
}

static void
dunno(void)
{
	printf("I don't know about that\n");
	exit(0);
}
