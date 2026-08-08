#include <stdio.h>
static int	slevel[10];
static int	clevel	= 0;
static int	spflg[20][10];
static int	sind[20][10];
static int	siflev[10];
static int	sifflg[10];
static int	iflev	= 0;
static int	ifflg	= -1;
static int	level	= 0;
static int	ind[10]	= {
	0,0,0,0,0,0,0,0,0,0 };
static int	eflg	= 0;
static int	paren	= 0;
static int	pflg[10] = {
	0,0,0,0,0,0,0,0,0,0 };
static char	lchar;
static char	pchar;
static int	aflg	= 0;
static int	ct;
static int	stabs[20][10];
static int	qflg	= 0;
static char	*wif[] = {
	"if",0};
static char	*welse[] = {
	"else",0};
static char	*wfor[] = {
	"for",0};
static char	*wds[] = {
	"case","default",0};
static int	j	= 0;
static char	string[200];
static char	cc;
static int	sflg	= 1;
static int	peek	= -1;
static int	tabs	= 0;
static int	lastchar;
static int	c;

static void ptabs(void);
static int getch(void);
static void cb_puts(void);
static int lookup(char **tab);
static int cb_gets(void);
static void gotelse(void);
static int getnl(void);
static void comment(void);

int
main(void)
{
	while((c = getch()) != EOF){
		switch(c){
		case ' ':
		case '\t':
			if(lookup(welse) == 1){
				gotelse();
				if(sflg == 0 || j > 0)string[j++] = c;
				cb_puts();
				sflg = 0;
				if(getnl() == 1){
					cb_puts();
					printf("\n");
					sflg = 1;
					pflg[level]++;
					tabs++;
				}
				continue;
			}
			if(sflg == 0 || j > 0)string[j++] = c;
			continue;
		case '\n':
			if((eflg = lookup(welse)) == 1)gotelse();
			cb_puts();
			printf("\n");
			sflg = 1;
			if(eflg == 1){
				pflg[level]++;
				tabs++;
			}
			else
				if(pchar == lchar)
					aflg = 1;
			continue;
		case '{':
			if(lookup(welse) == 1)gotelse();
			siflev[clevel] = iflev;
			sifflg[clevel] = ifflg;
			iflev = ifflg = 0;
			clevel++;
			if(sflg == 1 && pflg[level] != 0){
				pflg[level]--;
				tabs--;
			}
			string[j++] = c;
			cb_puts();
			getnl();
			cb_puts();
			printf("\n");
			tabs++;
			sflg = 1;
			if(pflg[level] > 0){
				ind[level] = 1;
				level++;
				slevel[level] = clevel;
			}
			continue;
		case '}':
			clevel--;
			if((iflev = siflev[clevel]-1) < 0)iflev = 0;
			ifflg = sifflg[clevel];
			if(pflg[level] >0 && ind[level] == 0){
				tabs -= pflg[level];
				pflg[level] = 0;
			}
			cb_puts();
			tabs--;
			ptabs();
			if((peek = getch()) == ';'){
				printf("%c;",c);
				peek = -1;
			}
			else printf("%c",c);
			getnl();
			cb_puts();
			printf("\n");
			sflg = 1;
			if(clevel < slevel[level])if(level > 0)level--;
			if(ind[level] != 0){
				tabs -= pflg[level];
				pflg[level] = 0;
				ind[level] = 0;
			}
			continue;
		case '"':
		case '\'':
			string[j++] = c;
			while((cc = getch()) != c){
				string[j++] = cc;
				if(cc == '\\'){
					string[j++] = getch();
				}
				if(cc == '\n'){
					cb_puts();
					sflg = 1;
				}
			}
			string[j++] = cc;
			if(getnl() == 1){
				lchar = cc;
				peek = '\n';
			}
			continue;
		case ';':
			string[j++] = c;
			cb_puts();
			if(pflg[level] > 0 && ind[level] == 0){
				tabs -= pflg[level];
				pflg[level] = 0;
			}
			getnl();
			cb_puts();
			printf("\n");
			sflg = 1;
			if(iflev > 0) {
				if(ifflg == 1){iflev--;
					ifflg = 0;
				}
				else iflev = 0;
			}
			continue;
		case '\\':
			string[j++] = c;
			string[j++] = getch();
			continue;
		case '?':
			qflg = 1;
			string[j++] = c;
			continue;
		case ':':
			string[j++] = c;
			if(qflg == 1){
				qflg = 0;
				continue;
			}
			if(lookup(wds) == 0){
				sflg = 0;
				cb_puts();
			}
			else{
				tabs--;
				cb_puts();
				tabs++;
			}
			if((peek = getch()) == ';'){
				printf(";");
				peek = -1;
			}
			getnl();
			cb_puts();
			printf("\n");
			sflg = 1;
			continue;
		case '/':
			string[j++] = c;
			if((peek = getch()) != '*')continue;
			string[j++] = peek;
			peek = -1;
			comment();
			continue;
		case ')':
			paren--;
			string[j++] = c;
			cb_puts();
			if(getnl() == 1){
				peek = '\n';
				if(paren != 0)aflg = 1;
				else if(tabs > 0){
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
			}
			continue;
		case '#':
			string[j++] = c;
			while((cc = getch()) != '\n')string[j++] = cc;
			string[j++] = cc;
			sflg = 0;
			cb_puts();
			sflg = 1;
			continue;
		case '(':
			string[j++] = c;
			paren++;
			if(lookup(wfor) == 1){
				while((c = cb_gets()) != ';');
				ct=0;
cont:
				while((c = cb_gets()) != ')'){
					if(c == '(') ct++;
				}
				if(ct != 0){
					ct--;
					goto cont;
				}
				paren--;
				cb_puts();
				if(getnl() == 1){
					peek = '\n';
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
				continue;
			}
			if(lookup(wif) == 1){
				cb_puts();
				stabs[clevel][iflev] = tabs;
				spflg[clevel][iflev] = pflg[level];
				sind[clevel][iflev] = ind[level];
				iflev++;
				ifflg = 1;
			}
			continue;
		default:
			string[j++] = c;
			if(c != ',')lchar = c;
		}
	}
	return(0);
}
static void
ptabs(void){
	int i;
	for(i=0; i < tabs; i++)printf("\t");
}
static int
getch(void){
	if(peek < 0 && lastchar != ' ' && lastchar != '\t')pchar = lastchar;
	lastchar = (peek<0) ? getc(stdin):peek;
	peek = -1;
	return(lastchar);
}
static void
cb_puts(void){
	if(j > 0){
		if(sflg != 0){
			ptabs();
			sflg = 0;
			if(aflg == 1){
				aflg = 0;
				if(tabs > 0)printf("    ");
			}
		}
		string[j] = '\0';
		printf("%s",string);
		j = 0;
	}
	else{
		if(sflg != 0){
			sflg = 0;
			aflg = 0;
		}
	}
}
static int
lookup(char **tab)
{
	char r;
	int l,kk,k,i;
	if(j < 1)return(0);
	kk=0;
	while(string[kk] == ' ')kk++;
	for(i=0; tab[i] != 0; i++){
		l=0;
		for(k=kk;(r = tab[i][l++]) == string[k] && r != '\0';k++);
		if(r == '\0' && (string[k] < 'a' || string[k] > 'z' || k >= j))return(1);
	}
	return(0);
}
static int
cb_gets(void){
	int ch;
beg:
	if((ch = string[j++] = getch()) == '\\'){
		string[j++] = getch();
		goto beg;
	}
	if(ch == '\'' || ch == '"'){
		while((cc = string[j++] = getch()) != ch)if(cc == '\\')string[j++] = getch();
		goto beg;
	}
	if(ch == '\n'){
		cb_puts();
		aflg = 1;
		goto beg;
	}
	else return(ch);
}
static void
gotelse(void){
	tabs = stabs[clevel][iflev];
	pflg[level] = spflg[clevel][iflev];
	ind[level] = sind[clevel][iflev];
	ifflg = 1;
}
static int
getnl(void){
	while((peek = getch()) == '\t' || peek == ' '){
		string[j++] = peek;
		peek = -1;
	}
	if((peek = getch()) == '/'){
		peek = -1;
		if((peek = getch()) == '*'){
			string[j++] = '/';
			string[j++] = '*';
			peek = -1;
			comment();
		}
		else string[j++] = '/';
	}
	if((peek = getch()) == '\n'){
		peek = -1;
		return(1);
	}
	return(0);
}
static void
comment(void){
rep:
	while((c = string[j++] = getch()) != '*')
		if(c == '\n'){
			cb_puts();
			sflg = 1;
		}
gotstar:
	if((c = string[j++] = getch()) != '/'){
		if(c == '*')goto gotstar;
		goto rep;
	}
}
