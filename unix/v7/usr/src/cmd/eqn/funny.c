# include "e.h"
# include "e.def"

void
funny(int n) {
	char *f = "";

	yyval = oalloc();
	switch(n) {
	case SUM:
		f = "\\(*S"; break;
	case UNION:
		f = "\\(cu"; break;
	case INTER:	/* intersection */
		f = "\\(ca"; break;
	case PROD:
		f = "\\(*P"; break;
	default:
		error(FATAL, "funny type %d in funny", n);
	}
	printf(".ds %d \\s%d\\v'.3m'\\s+5%s\\s-5\\v'-.3m'\\s%d\n", YV, ps, f, ps);
	eht[yyval] = VERT( (ps+5)*6 -(ps*6*2)/10 );
	ebase[yyval] = VERT( (ps*6*3)/10 );
	if(dbg)printf(".\tfunny: S%d <- %s; h=%d b=%d\n", 
		YV, f, eht[yyval], ebase[yyval]);
	lfont[yyval] = rfont[yyval] = ROM;
}
