/*
 * Expand Huffman coded input to standard output.
 *
 * This is the historical pcat/unpack algorithm with the pre-stdio iobuf
 * calls and 16-bit int assumptions made explicit for the Epoch68 V7 port.
 *
 * Input file format:
 *	PACKED flag defined below (16-bit integer)
 *	Number of chars in expanded file (two 16-bit words, PDP long order)
 *	Number of words in expanded tree (16-bit integer)
 *	Tree in compressed form:
 *		If 0 <= byte <= 0376, expand by zero padding to left
 *		If byte == 0377, next two bytes for one word
 *		Terminal nodes: first word is zero; second is character
 *		Non-terminal nodes: incremental 0/1 pointers
 *	Code string for number of characters in expanded file
 */

#include <stdio.h>
#include <unistd.h>

#define SUF0	'.'
#define SUF1	'z'

#define LNAME	80
#define PACKED	017437		/* <US><US> - unlikely value */

long size;
int tree[1024];
FILE *infile;

static i32 get16(void);
static i32 gettreeword(void);
static i32 unpack_file(char *name);
static void expand(void);

int
main(int argc, char *argv[])
{
	register int k;
	int errors;

	errors = 0;
	if(argc == 1) {
		infile = stdin;
		if(unpack_file((char *)0) < 0)
			errors++;
	} else {
		for(k = 1; k < argc; k++)
			if(unpack_file(argv[k]) < 0)
				errors++;
	}
	fflush(stdout);
	exit(errors != 0);
}

static i32
unpack_file(char *name)
{
	register int i;
	register int *t;
	int sep, keysize;
	int lo, hi;
	char filename[LNAME], *cp;

	if(name != 0) {
		sep = -1;
		cp = filename;
		for(i = 0; i < (LNAME-3) && (*cp = name[i]); i++)
			if(*cp++ == '/')
				sep = i;
		if(i != 0 && cp[-1] == SUF1 && cp[-2] == SUF0) {
			name[i-2] = '\0';
			return(unpack_file(name));
		}
		/* sep is the index of '/', so exclude it before applying V7's
		 * 14-character component limit.  /dist/srcbase.tar.z has a
		 * 13-character basename and must be accepted. */
		if(i >= (LNAME-3) || (i-sep-1) > 14) {
			fprintf(stderr, "pcat: file name too long -- %s\n", name);
			return(-1);
		}
		*cp++ = SUF0;
		*cp++ = SUF1;
		*cp = '\0';
		if((infile = fopen(filename, "r")) == NULL) {
			fprintf(stderr, "pcat: unable to open %s\n", filename);
			return(-1);
		}
	}

	if(get16() != PACKED) {
		if(name != 0) {
			fprintf(stderr, "pcat: unable to unpack %s\n", filename);
			fclose(infile);
		} else {
			fprintf(stderr, "pcat: input is not packed\n");
		}
		return(-1);
	}

	/*
	 * The historical source used a union of int[2], float, and long.
	 * On our 32-bit C port, read the 16-bit words explicitly.  Normal
	 * V7-sized files fit the long encoding; the old float escape is not
	 * useful for our bootstrap archives.
	 */
	lo = get16();
	hi = get16();
	if(lo > 040000) {
		fprintf(stderr, "pcat: packed file uses unsupported float size\n");
		if(name != 0)
			fclose(infile);
		return(-1);
	}
	size = ((long)lo << 16) | (hi & 0177777L);

	t = tree;
	for(keysize = get16(); keysize--; )
		*t++ = gettreeword();

	expand();

	if(name != 0)
		fclose(infile);
	return(0);
}

static i32
get16(void)
{
	register int lo, hi;

	lo = getc(infile);
	hi = getc(infile);
	if(lo == EOF || hi == EOF)
		return(-1);
	return((hi << 8) | lo);
}

static i32
gettreeword(void)
{
	register int c;

	c = getc(infile);
	if(c == EOF)
		return(-1);
	if(c == 0377)
		return(get16());
	return(c);
}

static void
expand(void)
{
	register int tp, bit;
	register unsigned int word;

	bit = tp = 0;
	for(;;) {
		if(bit == 0) {
			word = get16();
			bit = 16;
		}
		tp += tree[tp + ((word & 0100000) != 0)];
		/* The historical unsigned int was 16 bits.  Preserve that wrap on
		 * the 32-bit 68000 C port before examining the next code bit. */
		word = (word << 1) & 0177777;
		bit--;
		if(tree[tp] == 0) {
			putc(tree[tp+1], stdout);
			tp = 0;
			if(--size == 0)
				return;
		}
	}
}
