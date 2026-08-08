# include <stdio.h>
static int printable(int);

void allprint(char c) {
	extern FILE *yyout;
	switch(c){
		case '\n':
			fprintf(yyout,"\\n");
			break;
		case '\t':
			fprintf(yyout,"\\t");
			break;
		case '\b':
			fprintf(yyout,"\\b");
			break;
		case ' ':
			fprintf(yyout,"\\\bb");
			break;
		default:
			if(!printable(c))
				fprintf(yyout,"\\%-3o",c);
			else 
				putc(c,yyout);
			break;
		}
	return;
	}
void sprint(char *s) {
	while(*s)
		allprint(*s++);
	return;
	}
static int printable(int c) {
	return(040 < c && c < 0177);
	}
