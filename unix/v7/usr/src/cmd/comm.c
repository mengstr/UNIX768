#include <stdio.h>
#include <unistd.h>
#define LB 256
int	one;
int	two;
int	three;

char	*ldr[3];

FILE *ib1;
FILE *ib2;
static i32 comm_rd(FILE *file, char *buf);
static void comm_wr(char *str, i32 n);
static void copy(FILE *ibuf, char *lbuf, i32 n);
static i32 compare(char *a, char *b);
static FILE *openfil(char *s);
int
main(int argc, char *argv[])
{
	int	l;
	char	lb1[LB],lb2[LB];

	ldr[0] = "";
	ldr[1] = "\t";
	ldr[2] = "\t\t";
	if(argc > 1)  {
		if(*argv[1] == '-' && argv[1][1] != 0) {
			l = 1;
			while(*++argv[1]) {
				switch(*argv[1]) {
				case'1':
					if(!one) {
						one = 1;
						ldr[1][0] =
						ldr[2][l--] = '\0';
					}
					break;
				case '2':
					if(!two) {
						two = 1;
						ldr[2][l--] = '\0';
					}
					break;
				case '3':
					three = 1;
					break;
				default:
				fprintf(stderr,"comm: illegal flag\n");
				exit(1);
				}
			}
			argv++;
			argc--;
		}
	}

	if(argc < 3) {
		fprintf(stderr,"comm: arg count\n");
		exit(1);
	}

	ib1 = openfil(argv[1]);
	ib2 = openfil(argv[2]);


	if(comm_rd(ib1,lb1) < 0) {
		if(comm_rd(ib2,lb2) < 0)	exit(0);
		copy(ib2,lb2,2);
	}
	if(comm_rd(ib2,lb2) < 0)	copy(ib1,lb1,1);

	while(1) {

		switch(compare(lb1,lb2)) {

			case 0:
				comm_wr(lb1,3);
				if(comm_rd(ib1,lb1) < 0) {
					if(comm_rd(ib2,lb2) < 0)	exit(0);
					copy(ib2,lb2,2);
				}
				if(comm_rd(ib2,lb2) < 0)	copy(ib1,lb1,1);
				continue;

			case 1:
				comm_wr(lb1,1);
				if(comm_rd(ib1,lb1) < 0)	copy(ib2,lb2,2);
				continue;

			case 2:
				comm_wr(lb2,2);
				if(comm_rd(ib2,lb2) < 0)	copy(ib1,lb1,1);
				continue;
		}
	}
}

static i32
comm_rd(file,buf)
FILE *file;
char *buf;
{

	register int i, c;
	i = 0;
	while((c = getc(file)) != EOF) {
		*buf = c;
		if(c == '\n' || i > LB-2) {
			*buf = '\0';
			return(0);
		}
		i++;
		buf++;
	}
	return(-1);
}

static void
comm_wr(str,n)
	char	*str;
	i32 n;
{

	switch(n) {

		case 1:
			if(one)	return;
			break;

		case 2:
			if(two)	return;
			break;

		case 3:
			if(three)	return;
	}
	printf("%s%s\n",ldr[n-1],str);
}

static void
copy(ibuf,lbuf,n)
FILE *ibuf;
char *lbuf;
i32 n;
{
	do {
		comm_wr(lbuf,n);
	} while(comm_rd(ibuf,lbuf) >= 0);

	exit(0);
}

static i32
compare(a,b)
	char	*a,*b;
{
	register char *ra,*rb;

	ra = --a;
	rb = --b;
	while(*++ra == *++rb)
		if(*ra == '\0')	return(0);
	if(*ra < *rb)	return(1);
	return(2);
}
static FILE *
openfil(s)
char *s;
{
	FILE *b;
	if(s[0]=='-' && s[1]==0)
		b = stdin;
	else if((b=fopen(s,"r")) == NULL) {
		fprintf(stderr,"comm: cannot open %s\n",s);
		exit(1);
	}
	return(b);
}
