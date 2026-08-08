# include "refer.h"
void
shell (n, comp, exch)
	int n;
	int (*comp)(int, int);
	void (*exch)(int, int);
/* SORTS UP.  IF THERE ARE NO EXCHANGES (IEX=0) ON A SWEEP
  THE COMPARISON GAP (IGAP) IS HALVED FOR THE NEXT SWEEP */
{
	int igap, iplusg, iex, i, imax;
	igap=n;
while (igap > 1)
	{
	igap /= 2;
	imax = n-igap;
	do
		{
		iex=0;
		for(i=0; i<imax; i++)
			{
			iplusg = i + igap;
			if ((*comp) (i, iplusg) ) continue;
			(*exch) (i, iplusg);
			iex=1;
			}
		} while (iex>0);
	}
}
