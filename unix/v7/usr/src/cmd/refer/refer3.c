# include "refer..c"
int
corout(in, out, rprog, arg, outlen)
		char *in, *out, *rprog, *arg;
		int outlen;
{
# define move(x, y) close(y); dup(x); close(x);
i16 pipev[2], fr1, fr2, fw1, fw2;
int n;

pipe (pipev); fr1= pipev[0]; fw1 = pipev[1];
pipe (pipev); fr2= pipev[0]; fw2 = pipev[1];
if (fork()==0)
	{
	close (fw1); close (fr2);
	move (fr1, 0);
	move (fw2, 1);
	execl(rprog, "deliv", arg, (char *)0);
	err ("Can't run %s", rprog);
	}
close(fw2); close(fr1);
write (fw1, in, (u16)strlen(in));
close(fw1);
wait((i16 *)0);
n = read (fr2, out, outlen);
out[n]=0;
close(fr2);
return(n);
}
