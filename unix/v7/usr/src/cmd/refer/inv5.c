# include "refer.h"

static long getshort(FILE *);
int recopy(FILE *, FILE *, FILE *);

static long
getshort(f)
	FILE *f;
{
return((long)ref_getw(f));
}

int
recopy (ft, fb, fa)
	FILE *ft, *fb, *fa;
{
/* copy fb (old hash items/pointers) to ft (new ones) */
int n, i, iflong;
int *hpt_s;
long (*getfun)(FILE *);
long *hpt_l;
long k, lp;
if (fa==NULL)
	{
		err("No old pointers",0);
		return(0);
		}
	fread((char *)&n, sizeof(n), 1, fa);
	fread((char *)&iflong, sizeof(iflong), 1, fa);
if (iflong)
	{
	hpt_l = calloc(n+1, sizeof(*hpt_l));
	n = fread((char *)hpt_l, sizeof(*hpt_l), n, fa);
	}
else
	{
	hpt_s = calloc(n+1, sizeof(*hpt_s));
	n = fread((char *)hpt_s, sizeof(*hpt_s), n, fa);
	}
fclose(fa);
if (iflong)
	getfun = &getl;
else
	getfun = getshort;
for(i=0; i<n; i++)
	{
	if (iflong)
		lp = hpt_l[i];
	else
		lp = hpt_s[i];
	fseek(fb, lp, 0);
	while ( (k= (*getfun)(fb) ) != -1)
		fprintf(ft, "%04d %06ld\n",i,k);
	}
fclose(fb);
return(n);
}
