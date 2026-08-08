#include	<stdio.h>
#include 	<a.out.h>

/*
	size -- determine object size

*/

int
main (int argc, char **argv)
{
	struct exec buf;
	u32 text, sum;
	int gorp;
	FILE *f;

	if (argc==1) {
		*argv = "a.out";
		argc++;
		--argv;
	}
	gorp = argc;
	while(--argc) {
		++argv;
		if ((f = fopen(*argv, "r"))==NULL) {
			printf("size: %s not found\n", *argv);
			continue;
		}
		fread((char *)&buf, sizeof(buf), 1, f);
		if(A_GETMID(buf) != A_MID_SUN010 ||
		   A_GETMAGIC(buf) != A_MAGIC5) {
			printf("size: %s not an object file\n", *argv);
			fclose(f);
			continue;
		}
		text = buf.a_text - sizeof(struct exec);
		if (gorp>2)
			printf("%s: ", *argv);
		printf("%lu+%lu+%lu = ", text, buf.a_data, buf.a_bss);
		sum = text + buf.a_data + buf.a_bss;
		printf("%lub = 0%lob\n", sum, sum);
		fclose(f);
	}
}
