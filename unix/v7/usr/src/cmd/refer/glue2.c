# include "refer.h"
 char refdir[50];
void
savedir(void)
{
if (refdir[0]==0)
	corout ("", refdir, "/bin/pwd", "", 50);
trimnl(refdir);
}
void
restodir(void)
{
chdir(refdir);
}
