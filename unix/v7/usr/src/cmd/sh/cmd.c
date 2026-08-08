#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"
#include	"sym.h"

LOCAL IOPTR	inout(IOPTR);
LOCAL VOID	chkword(void);
LOCAL VOID	chksym(int);
LOCAL TREPTR	term(int);
LOCAL TREPTR	makelist(int, TREPTR, TREPTR);
LOCAL TREPTR	list(int);
LOCAL REGPTR	syncase(int);
LOCAL TREPTR	item(int);
LOCAL VOID	skipnl(void);
LOCAL VOID	prsym(int);
LOCAL VOID	synbad(void);


/* ========	command line decoding	========*/




TREPTR	makefork(flgs, i)
	INT		flgs;
	TREPTR		i;
{
	REG FORKPTR	t;

	t=(FORKPTR)getstak(FORKTYPE);
	t->forktyp=flgs|TFORK; t->forktre=i; t->forkio=0;
	return((TREPTR)t);
}

LOCAL TREPTR	makelist(type,i,r)
	INT		type;
	TREPTR		i, r;
{
	REG LSTPTR	t;

	IF i==0 ORF r==0
	THEN	synbad();
	ELSE	t = (LSTPTR)getstak(LSTTYPE);
		t->lsttyp = type;
		t->lstlef = i; t->lstrit = r;
	FI
	return((TREPTR)t);
}

/*
 * cmd
 *	empty
 *	list
 *	list & [ cmd ]
 *	list [ ; cmd ]
 */

TREPTR	cmd(sym,flg)
	REG INT		sym;
	INT		flg;
{
	REG TREPTR	i, e;

	i = list(flg);

	IF wdval==NL
	THEN	IF flg&NLFLG
		THEN	wdval=';'; chkpr(NL);
		FI
	ELIF i==0 ANDF (flg&MTFLG)==0
	THEN	synbad();
	FI

	SWITCH wdval IN

	    case '&':
		IF i
		THEN	i = makefork(FINT|FPRS|FAMP, i);
		ELSE	synbad();
		FI

	    case ';':
		IF e=cmd(sym,flg|MTFLG)
		THEN	i=makelist(TLST, i, e);
		FI
		break;

	    case EOFSYM:
		IF sym==NL
		THEN	break;
		FI

	    default:
		IF sym
		THEN	chksym(sym);
		FI

	ENDSW
	return(i);
}

/*
 * list
 *	term
 *	list && term
 *	list || term
 */

LOCAL TREPTR	list(flg)
{
	REG TREPTR	r;
	REG INT		b;

	r = term(flg);
	WHILE r ANDF ((b=(wdval==ANDFSYM)) ORF wdval==ORFSYM)
	DO	r = makelist((b ? TAND : TORF), r, term(NLFLG));
	OD
	return(r);
}

/*
 * term
 *	item
 *	item |^ term
 */

LOCAL TREPTR	term(flg)
{
	REG TREPTR	t;

	reserv++;
	IF flg&NLFLG
	THEN	skipnl();
	ELSE	word();
	FI

	IF (t=item(TRUE)) ANDF (wdval=='^' ORF wdval=='|')
	THEN	return(makelist(TFIL, makefork(FPOU,t), makefork(FPIN|FPCL,term(NLFLG))));
	ELSE	return(t);
	FI
}

LOCAL REGPTR	syncase(esym)
	REG INT	esym;
{
	skipnl();
	IF wdval==esym
	THEN	return(0);
	ELSE	REG REGPTR	r=(REGPTR)getstak(REGTYPE);
		r->regptr=0;
		LOOP wdarg->argnxt=r->regptr;
		     r->regptr=wdarg;
		     IF wdval ORF ( word()!=')' ANDF wdval!='|' )
		     THEN synbad();
		     FI
		     IF wdval=='|'
		     THEN word();
		     ELSE break;
		     FI
		POOL
		r->regcom=cmd(0,NLFLG|MTFLG);
		IF wdval==ECSYM
		THEN	r->regnxt=syncase(esym);
		ELSE	chksym(esym);
			r->regnxt=0;
		FI
		return(r);
	FI
}

/*
 * item
 *
 *	( cmd ) [ < in  ] [ > out ]
 *	word word* [ < in ] [ > out ]
 *	if ... then ... else ... fi
 *	for ... while ... do ... done
 *	case ... in ... esac
 *	begin ... end
 */

LOCAL TREPTR	item(flag)
	BOOL		flag;
{
	REG TREPTR	t;
	REG IOPTR	io;

	IF flag
	THEN	io=inout((IOPTR)0);
	ELSE	io=0;
	FI

	SWITCH wdval IN

	    case CASYM:
		BEGIN
		   REG SWPTR	sw=(SWPTR)getstak(SWTYPE);
		   chkword();
		   sw->swarg=wdarg->argval;
		   skipnl(); chksym(INSYM|BRSYM);
		   sw->swlst=syncase(wdval==INSYM?ESSYM:KTSYM);
		   sw->swtyp=TSW;
		   t=(TREPTR)sw;
		   break;
		END

	    case IFSYM:
		BEGIN
		   REG IFPTR	ifp;
		   REG INT	w;
		   ifp=(IFPTR)getstak(IFTYPE);
		   ifp->iftyp=TIF;
		   ifp->iftre=cmd(THSYM,NLFLG);
		   ifp->thtre=cmd(ELSYM|FISYM|EFSYM,NLFLG);
		   ifp->eltre=((w=wdval)==ELSYM ? cmd(FISYM,NLFLG) : (w==EFSYM ? (wdval=IFSYM, item(0)) : 0));
		   t=(TREPTR)ifp;
		   IF w==EFSYM THEN return(t) FI
		   break;
		END

	    case FORSYM:
		BEGIN
		   REG FORPTR	forp=(FORPTR)getstak(FORTYPE);
		   forp->fortyp=TFOR;
		   forp->forlst=0;
		   chkword();
		   forp->fornam=wdarg->argval;
		   IF skipnl()==INSYM
		   THEN	chkword();
			forp->forlst=(COMPTR)item(0);
			IF wdval!=NL ANDF wdval!=';'
			THEN	synbad();
			FI
			chkpr(wdval); skipnl();
		   FI
		   chksym(DOSYM|BRSYM);
		   forp->fortre=cmd(wdval==DOSYM?ODSYM:KTSYM,NLFLG);
		   t=(TREPTR)forp;
		   break;
		END

	    case WHSYM:
	    case UNSYM:
		BEGIN
		   REG WHPTR	whp=(WHPTR)getstak(WHTYPE);
		   whp->whtyp=(wdval==WHSYM ? TWH : TUN);
		   whp->whtre = cmd(DOSYM,NLFLG);
		   whp->dotre = cmd(ODSYM,NLFLG);
		   t=(TREPTR)whp;
		   break;
		END

	    case BRSYM:
		t=cmd(KTSYM,NLFLG);
		break;

	    case '(':
		BEGIN
		   REG PARPTR	 p;
		   p=(PARPTR)getstak(PARTYPE);
		   p->partre=cmd(')',NLFLG);
		   p->partyp=TPAR;
		   t=makefork(0,(TREPTR)p);
		   break;
		END

	    default:
		IF io==0
		THEN	return(0);
		FI

	    case 0:
		BEGIN
		   REG ARGPTR	argp;
		   REG ARGPTR	*argtail;
		   REG ARGPTR	argset=0;
		   INT		keywd=1;
		   REG COMPTR	comp=(COMPTR)getstak(COMTYPE);
		   comp->comio=io; /*initial io chain*/
		   argtail = &(comp->comarg);
		   WHILE wdval==0
		   DO	argp = wdarg;
			IF wdset ANDF keywd
			THEN	argp->argnxt=argset; argset=argp;
			ELSE	*argtail=argp; argtail = &(argp->argnxt); keywd=flags&keyflg;
			FI
			word();
			IF flag
			THEN comp->comio=inout(comp->comio);
			FI
		   OD

		   comp->comtyp=TCOM; comp->comset=argset; *argtail=0;
		   return((TREPTR)comp);
		END

	ENDSW
	reserv++; word();
	IF io=inout(io)
	THEN	t=makefork(0,t); t->treio=io;
	FI
	return(t);
}


LOCAL VOID	skipnl()
{
	WHILE (reserv++, word()==NL) DO chkpr(NL) OD
	return(wdval);
}

LOCAL IOPTR	inout(lastio)
	IOPTR		lastio;
{
	REG INT		iof;
	REG IOPTR	iop;
	REG CHAR	c;

	iof=wdnum;

	SWITCH wdval IN

	    case DOCSYM:
		iof |= IODOC; break;

	    case APPSYM:
	    case '>':
		IF wdnum==0 THEN iof |= 1 FI
		iof |= IOPUT;
		IF wdval==APPSYM
		THEN	iof |= IOAPP; break;
		FI

	    case '<':
		IF (c=nextc(0))=='&'
		THEN	iof |= IOMOV;
		ELIF c=='>'
		THEN	iof |= IORDW;
		ELSE	peekc=c|MARK;
		FI
		break;

	    default:
		return(lastio);
	ENDSW

	chkword();
	iop=(IOPTR)getstak(IOTYPE); iop->ioname=wdarg->argval; iop->iofile=iof;
	IF iof&IODOC
	THEN iop->iolst=iopend; iopend=iop;
	FI
	word(); iop->ionxt=inout(lastio);
	return(iop);
}

LOCAL VOID	chkword()
{
	IF word()
	THEN	synbad();
	FI
}

LOCAL VOID	chksym(sym)
{
	REG INT		x = sym&wdval;
	IF ((x&SYMFLG) ? x : sym) != wdval
	THEN	synbad();
	FI
}

LOCAL VOID	prsym(sym)
{
	IF sym&SYMFLG
	THEN	REG SYSPTR	sp=reserved;
		WHILE sp->sysval
			ANDF sp->sysval!=sym
		DO sp++ OD
		prs(sp->sysnam);
	ELIF sym==EOFSYM
	THEN	prs(endoffile);
	ELSE	IF sym&SYMREP THEN prc(sym) FI
		IF sym==NL
		THEN	prs("newline");
		ELSE	prc(sym);
		FI
	FI
}

LOCAL VOID	synbad()
{
	prp(); prs(synmsg);
	IF (flags&ttyflg)==0
	THEN	prs(atline); prn(standin->flin);
	FI
	prs(colon);
	prc(LQ);
	IF wdval
	THEN	prsym(wdval);
	ELSE	prs(wdarg->argval);
	FI
	prc(RQ); prs(unexpected);
	newline();
	exitsh(SYNBAD);
}
