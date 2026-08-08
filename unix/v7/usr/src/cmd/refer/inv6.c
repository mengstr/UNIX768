# include "refer.h"
void
whash(ft, fa, fb, nhash, iflong, ptotct, phused)
	FILE *fa, *fb, *ft;
	int nhash, *phused;
	long *ptotct;
{
char line[100];
int hash = 0, hused = 0;
long totct = 0L;
int ct = 0;
long point;
long opoint = -1;
int m;
int k; long lp;
long *hpt;
int *hfreq = NULL;

hpt = calloc (nhash+1, sizeof(*hpt));
_assert (hpt != NULL);
hfreq = calloc (nhash, sizeof(*hfreq));
_assert (hfreq !=NULL);
hpt[0] = 0;
lp= 0;
while (fgets(line, 100, ft))
	{
	totct++;
	sscanf(line, "%d %ld", &k, &point);
	if (hash < k)
		{
		hused++;
		if (iflong) putl(-1L, fb); else ref_putw(-1, fb);
		hfreq[hash]=ct;
		while (hash<k)
			{
			hpt[++hash] = lp;
			hfreq[hash] = 0;
			}
		hpt[hash] = lp += iflong? sizeof(long) : sizeof(int);
		opoint= -1;
		ct=0;
		}
	if (point!=opoint)
		{
		if (iflong)
			putl(opoint=point, fb);
		else
			ref_putw( (int)(opoint=point), fb);
		lp += iflong? sizeof(long) : sizeof(int);
		ct++;
		}
	}
if (iflong) putl(-1L, fb); else ref_putw(-1,fb);
while (hash<nhash)
	hpt[++hash]=lp;
fwrite((char *)&nhash, sizeof(nhash), 1, fa);
fwrite((char *)&iflong, sizeof(iflong), 1, fa);
fwrite((char *)hpt, sizeof(*hpt), nhash, fa);
fwrite((char *)hfreq, sizeof(*hfreq), nhash, fa);
*ptotct = totct;
*phused = hused;
}
void
putl(ll, f)
	long ll;
	FILE *f;
{
if (fwrite((char *)&ll, sizeof(ll), 1, f) != 1)
	err("write error");
}
long getl(f)
	FILE *f;
{
long value;
if (fread((char *)&value, sizeof(value), 1, f) != 1)
	return(-1L);
return(value);
}
