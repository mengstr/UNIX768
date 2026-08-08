#include "../h/param.h"
#include "../h/tty.h"
#include "../h/systm.h"
#include "../h/conf.h"
#include "../h/buf.h"

void ndflush(struct clist *q, i32 cc);

struct cblock {
	struct cblock *c_next;
	char	c_info[CBSIZE];
};

char	cfree_space[(NCLIST * sizeof(struct cblock)) + CROUND];
struct	cblock	*cfree;
struct	cblock	*cfreelist;
i16	cbad;

/*
 * Character list get/put
 */
i32
getc(p)
register struct clist *p;
{
	register struct cblock *bp;
	register i32 c, s;

	s = spl6();
	if (p->c_cc <= 0) {
		c = -1;
		p->c_cc = 0;
		p->c_cf = p->c_cl = NULL;
	} else {
		c = *p->c_cf++ & 0377;
		if (--p->c_cc<=0) {
			bp = (struct cblock *)(p->c_cf-1);
			bp = (struct cblock *)((unsigned long)bp & ~(unsigned long)CROUND);
			p->c_cf = NULL;
			p->c_cl = NULL;
			bp->c_next = cfreelist;
			cfreelist = bp;
		} else if (((unsigned long)p->c_cf & (unsigned long)CROUND) == 0){
			bp = (struct cblock *)(p->c_cf);
			bp--;
			p->c_cf = bp->c_next->c_info;
			bp->c_next = cfreelist;
			cfreelist = bp;
		}
	}
	splx(s);
	return(c);
}

/*
 * copy clist to buffer.
 * return number of bytes moved.
 */
i32
q_to_b(q, cp, cc)
register struct clist *q;
register char *cp;
i32 cc;
{
	register struct cblock *bp;
	register i32 s;
	char *acp;

	if (cc <= 0)
		return(0);
	s = spl6();
	if (q->c_cc <= 0) {
		q->c_cc = 0;
		q->c_cf = q->c_cl = NULL;
		return(0);
	}
	acp = cp;
	cc++;

	while (--cc) {
		*cp++ = *q->c_cf++;
		if (--q->c_cc <= 0) {
			bp = (struct cblock *)(q->c_cf-1);
			bp = (struct cblock *)((unsigned long)bp & ~(unsigned long)CROUND);
			q->c_cf = q->c_cl = NULL;
			bp->c_next = cfreelist;
			cfreelist = bp;
			break;
		}
		if (((unsigned long)q->c_cf & (unsigned long)CROUND) == 0) {
			bp = (struct cblock *)(q->c_cf);
			bp--;
			q->c_cf = bp->c_next->c_info;
			bp->c_next = cfreelist;
			cfreelist = bp;
		}
	}
	splx(s);
	return(cp-acp);
}


/*
 * Return count of contiguous characters
 * in clist starting at q->c_cf.
 * Stop counting if flag&character is non-null.
 */
i32
ndqb(register struct clist *q, i32 flag)
{
	register i32 cc;
	i32 s;

	s = spl6();
	if (q->c_cc <= 0) {
		cc = -q->c_cc;
		goto out;
	}
	cc = (i32)(((((unsigned long)q->c_cf) + CBSIZE) &
	    ~(unsigned long)CROUND) - (unsigned long)q->c_cf);
	if (q->c_cc < cc)
		cc = q->c_cc;
	if (flag) {
		register char *p, *end;

		p = q->c_cf;
		end = p;
		end += cc;
		while (p < end) {
			if (*p & flag) {
				cc = (i32)(p - q->c_cf);
				break;
			}
			p++;
		}
	}
out:
	splx(s);
	return(cc);
}



/*
 * Update clist to show that cc characters
 * were removed.  It is assumed that cc < CBSIZE.
 */
void ndflush(q, cc)
register struct clist *q;
i32 cc;
{
	register i32 s;

	s = spl6();
	if (q->c_cc < 0) {
		if (q->c_cf != NULL) {
			q->c_cc += cc;
			q->c_cf += cc;
			goto out;
		}
		q->c_cc = 0;
		goto out;
	}
	if (q->c_cc == 0) {
		goto out;
	}
	if (cc > CBSIZE || cc <= 0) {
		cbad++;
		goto out;
	}
	q->c_cc -= cc;
	q->c_cf += cc;
	if (((unsigned long)q->c_cf & (unsigned long)CROUND) == 0) {
		register struct cblock *bp;

		bp = (struct cblock *)(q->c_cf) -1;
		if (bp->c_next) {
			q->c_cf = bp->c_next->c_info;
		} else {
			q->c_cf = q->c_cl = NULL;
		}
		bp->c_next = cfreelist;
		cfreelist = bp;
	} else
	if (q->c_cc == 0) {
		register struct cblock *bp;
		q->c_cf = (char *)((unsigned long)q->c_cf & ~(unsigned long)CROUND);
		bp = (struct cblock *)(q->c_cf);
		bp->c_next = cfreelist;
		cfreelist = bp;
		q->c_cf = q->c_cl = NULL;
	}
out:
	splx(s);
}
i32
putc(c, p)
i32 c;
register struct clist *p;
{
	register struct cblock *bp;
	register char *cp;
	register i32 s;

	s = spl6();
	if ((cp = p->c_cl) == NULL || p->c_cc < 0 ) {
		if ((bp = cfreelist) == NULL) {
			splx(s);
			return(-1);
		}
		cfreelist = bp->c_next;
		bp->c_next = NULL;
		p->c_cf = cp = bp->c_info;
	} else if (((unsigned long)cp & (unsigned long)CROUND) == 0) {
		bp = (struct cblock *)cp - 1;
		if ((bp->c_next = cfreelist) == NULL) {
			splx(s);
			return(-1);
		}
		bp = bp->c_next;
		cfreelist = bp->c_next;
		bp->c_next = NULL;
		cp = bp->c_info;
	}
	*cp++ = c;
	p->c_cc++;
	p->c_cl = cp;
	splx(s);
	return(0);
}



/*
 * copy buffer to clist.
 * return number of bytes not transfered.
 */
i32
b_to_q(cp, cc, q)
register char *cp;
struct clist *q;
i32 cc;
{
	register char *cq;
	register struct cblock *bp;
	register i32 s, acc;

	if (cc <= 0)
		return(0);
	acc = cc;


	s = spl6();
	if ((cq = q->c_cl) == NULL || q->c_cc < 0) {
		if ((bp = cfreelist) == NULL) 
			goto out;
		cfreelist = bp->c_next;
		bp->c_next = NULL;
		q->c_cf = cq = bp->c_info;
	}

	while (cc) {
		if (((unsigned long)cq & (unsigned long)CROUND) == 0) {
			bp = (struct cblock *) cq - 1;
			if ((bp->c_next = cfreelist) == NULL) 
				goto out;
			bp = bp->c_next;
			cfreelist = bp->c_next;
			bp->c_next = NULL;
			cq = bp->c_info;
		}
		*cq++ = *cp++;
		cc--;
	}
out:
	q->c_cl = cq;
	q->c_cc += acc-cc;
	splx(s);
	return(cc);
}

/*
 * Initialize clist by freeing all character blocks, then count
 * number of character devices. (Once-only routine)
 */
void cinit()
{
	register unsigned long ccp;
	register struct cblock *cp;
	register struct cdevsw *cdp;

	ccp = (unsigned long)cfree_space;
	ccp = (ccp+CROUND) & ~CROUND;
	cfree = (struct cblock *)ccp;
	for(cp = cfree; cp < &cfree[NCLIST]; cp++) {
		cp->c_next = cfreelist;
		cfreelist = cp;
	}
	ccp = 0;
	for(cdp = cdevsw; cdp->d_open; cdp++)
		ccp++;
	nchrdev = ccp;
}


/*
 * integer (2-byte) get/put
 * using clists
 */
i32
getw(register struct clist *p)
{
	register i32 s;

	if (p->c_cc <= 1)
		return(-1);
	s = getc(p);
	return(s | (getc(p)<<8));
}

i32
putw(i32 c, register struct clist *p)
{
	register i32 s;

	s = spl6();
	if (cfreelist==NULL) {
		splx(s);
		return(-1);
	}
	putc(c, p);
	putc(c>>8, p);
	splx(s);
	return(0);
}
