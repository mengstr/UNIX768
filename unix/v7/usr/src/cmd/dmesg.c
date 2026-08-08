/*
 *	Suck up system messages
 */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <time.h>
#include <unistd.h>

#define KSYMS	"/etc/ksyms"

char	msgbuf[MSGBUFS];
char	*msgbufp;
int	sflg;
int	of	= -1;
long	msgbufaddr;
long	msgbufpaddr;

struct {
	char	*omsgflg;
	int	omindex;
	char	omsgbuf[MSGBUFS];
} omesg;

int ksyms(void);
int done(char *s);
int pdate(void);

int
main (int argc, char **argv)
{
	int mem;
	register char *mp, *omp, *mstart;
	int samef;

	if (argc>1 && argv[1][0] == '-') {
		sflg++;
		argc--;
		argv++;
	}
	if (sflg)
		of = open("/usr/adm/msgbuf", 2);
	read(of, (char *)&omesg, sizeof(omesg));
	lseek(of, 0L, 0);
	sflg = 0;
	if (ksyms() < 0)
		done("No ksyms\n");
	if ((mem = open((argc>1? argv[1]: "/dev/kmem"), 0)) < 0)
		done("No mem\n");
	lseek(mem, msgbufaddr, 0);
	read(mem, msgbuf, MSGBUFS);
	lseek(mem, msgbufpaddr, 0);
	read(mem, (char *)&msgbufp, sizeof(msgbufp));
	if (msgbufp < (char *)msgbufaddr ||
	    msgbufp >= (char *)msgbufaddr+MSGBUFS)
		done("Namelist mismatch\n");
	msgbufp += msgbuf - (char *)msgbufaddr;
	mstart = &msgbuf[omesg.omindex];
	omp = &omesg.omsgbuf[msgbufp-msgbuf];
	mp = msgbufp;
	samef = 1;
	do {
		if (*mp++ != *omp++) {
			mstart = msgbufp;
			samef = 0;
			pdate();
			printf("...\n");
			break;
		}
		if (mp == &msgbuf[MSGBUFS])
			mp = msgbuf;
		if (omp == &omesg.omsgbuf[MSGBUFS])
			omp = omesg.omsgbuf;
	} while (mp != mstart);
	if (samef && mstart == msgbufp)
		exit(0);
	mp = mstart;
	do {
		pdate();
		if (*mp)
			putchar(*mp);
		mp++;
		if (mp == &msgbuf[MSGBUFS])
			mp = msgbuf;
	} while (mp != msgbufp);
	done((char *)NULL);
}

int
ksyms (void)
{
	FILE *fp;
	char line[96], sym[64];
	register char *cp, *sp;
	long value;
	int found;

	if ((fp = fopen(KSYMS, "r")) == NULL)
		return(-1);
	found = 0;
	while (fgets(line, sizeof(line), fp) != NULL) {
		cp = line;
		while (*cp == ' ' || *cp == '\t')
			cp++;
		sp = sym;
		while (*cp && *cp != ' ' && *cp != '\t' && *cp != ':' &&
		    sp < &sym[sizeof(sym)-1])
			*sp++ = *cp++;
		*sp = 0;
		while (*cp && *cp != '\n') {
			if (cp[0] == 'v' && cp[1] == 'a' && cp[2] == 'l' &&
			    cp[3] == 'u' && cp[4] == 'e') {
				cp += 5;
				break;
			}
			if ((*cp >= '0' && *cp <= '9') ||
			    (cp[0] == '0' && cp[1] == 'x'))
				break;
			cp++;
		}
		while (*cp == ' ' || *cp == '\t')
			cp++;
		value = 0;
		if (cp[0] == '0' && cp[1] == 'x')
			cp += 2;
		while ((*cp >= '0' && *cp <= '9') ||
		    (*cp >= 'a' && *cp <= 'f') ||
		    (*cp >= 'A' && *cp <= 'F')) {
			value <<= 4;
			if (*cp >= '0' && *cp <= '9')
				value += *cp - '0';
			else if (*cp >= 'a' && *cp <= 'f')
				value += *cp - 'a' + 10;
			else
				value += *cp - 'A' + 10;
			cp++;
		}
		if (strcmp(sym, "_msgbuf") == 0) {
			msgbufaddr = value;
			found++;
		} else if (strcmp(sym, "_msgbufp") == 0) {
			msgbufpaddr = value;
			found++;
		}
		if (found >= 2) {
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	return(-1);
}

int
done (char *s)
{
	register char *p, *q;

	if (s && s!=omesg.omsgflg && sflg==0) {
		pdate();
		printf(s);
	}
	omesg.omsgflg = s;
	q = omesg.omsgbuf;
	for (p = msgbuf; p < &msgbuf[MSGBUFS]; )
		*q++ = *p++;
	omesg.omindex = msgbufp - msgbuf;
	write(of, (char *)&omesg, sizeof(omesg));
	exit(s!=NULL);
}

int
pdate (void)
{
	static firstime;
	time_t tbuf;
	register char *cp;
	register int i;

	if (firstime==0) {
		firstime++;
		time(&tbuf);
		cp = ctime(&tbuf)+4;
		putchar('\n');
		for (i=0; i<12 && cp[i]; i++)
			putchar(cp[i]);
		putchar('\n');
	}
}
