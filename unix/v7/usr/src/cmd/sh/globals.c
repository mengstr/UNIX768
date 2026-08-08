#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"

INT		ioset;
IOPTR		iotemp;
IOPTR		iopend;

INT		dolc;
STRING		*dolv;
DOLPTR		argfor;
ARGPTR		gchain;

INT		wdval;
INT		wdnum;
ARGPTR		wdarg;
INT		wdset;
BOOL		reserv;

STRING		cmdadr;
STRING		exitadr;
STRING		dolladr;
STRING		pcsadr;
STRING		pidadr;

STRING		tmpnam;
INT		serial;
INT		peekc;
STRING		comdiv;

INT		flags;
jmp_buf		subshell;
jmp_buf		errshell;

BOOL		trapnote;
BOOL		nosubst;

INT		exitval;
BOOL		execbrk;
INT		loopcnt;
INT		breakcnt;

BLKPTR		stakbsy;
STKPTR		stakbas;
STKPTR		brkend;
STKPTR		staktop;
