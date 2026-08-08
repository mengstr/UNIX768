#include "types.h"

typedef i32 (*cmp_fn)(const void *a, const void *b);

static i32 randx = 1;
static cmp_fn qscmp;
static i32 qses;

static void qs1(char *a, char *l);
static void qsexc(char *i, char *j);
static void qstexc(char *i, char *j, char *k);

void
srand(u32 x)
{
	randx = x;
}

i32
rand(void)
{
	randx = randx * 1103515245 + 12345;
	return (i32)((randx >> 16) & 077777);
}

void
qsort(void *base, usize n, i32 es, cmp_fn fc)
{
	char *a = base;

	qscmp = fc;
	qses = es;
	qs1(a, a + n * es);
}

static void
qs1(char *a, char *l)
{
	register char *i;
	register char *j;
	register i32 es;
	char *lp;
	char *hp;
	i32 c;
	unsigned n;

	es = qses;
start:
	if ((n = l - a) <= es)
		return;
	n = es * (n / (2 * es));
	hp = lp = a + n;
	i = a;
	j = l - es;
	for (;;) {
		if (i < lp) {
			if ((c = (*qscmp)(i, lp)) == 0) {
				qsexc(i, lp -= es);
				continue;
			}
			if (c < 0) {
				i += es;
				continue;
			}
		}
loop:
		if (j > hp) {
			if ((c = (*qscmp)(hp, j)) == 0) {
				qsexc(hp += es, j);
				goto loop;
			}
			if (c > 0) {
				if (i == lp) {
					qstexc(i, hp += es, j);
					i = lp += es;
					goto loop;
				}
				qsexc(i, j);
				j -= es;
				i += es;
				continue;
			}
			j -= es;
			goto loop;
		}
		if (i == lp) {
			if (lp - a >= l - hp) {
				qs1(hp + es, l);
				l = lp;
			} else {
				qs1(a, lp);
				a = hp + es;
			}
			goto start;
		}
		qstexc(j, lp -= es, i);
		j = hp -= es;
	}
}

static void
qsexc(char *i, char *j)
{
	register char *ri;
	register char *rj;
	register char c;
	i32 n;

	n = qses;
	ri = i;
	rj = j;
	do {
		c = *ri;
		*ri++ = *rj;
		*rj++ = c;
	} while (--n);
}

static void
qstexc(char *i, char *j, char *k)
{
	register char *ri;
	register char *rj;
	register char *rk;
	i32 c;
	i32 n;

	n = qses;
	ri = i;
	rj = j;
	rk = k;
	do {
		c = *ri;
		*ri++ = *rk;
		*rk++ = *rj;
		*rj++ = c;
	} while (--n);
}
