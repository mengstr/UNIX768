/*
 * Host-buildable pack-format generator for Epoch68 bring-up.
 *
 * This preserves the historical pack/pcat file format used by pcat.c, but
 * uses normal host stdio and explicit 16-bit output. It is intended as a
 * build-side helper until the historical V7 pack utility is ported fully.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define SUF0	'.'
#define SUF1	'z'

#define NNODES	512
#define LNAME	80
#define PACKED	017437

struct node {
	struct node *zlink;
	struct node *olink;
	long freq;
	int sym;
	struct node *sortl;
	struct node *sorth;
};

static struct node nodes[NNODES], sortstart, maxnode, *root, *leaves[256];
static int used, depth, freqflag;
static int tree[1024];
static unsigned char code[32];
static unsigned char codes[256][33];
static FILE *infile, *outfile;

static int pack_file(char *name);
static int sortcount(void);
static struct node *formtree(void);
static int puttree(void);
static int maketree(struct node *no);
static void gcode(int len, struct node *nod);
static int compress_file(void);
static int put16(int v);
static int copy_name(char *dst, char *src);
static int append_z_suffix(char *dst, int dstsz);
static long packed_size_estimate(int ncodes, int treesize);

int
main(int argc, char **argv)
{
	int k, errors;

	errors = 0;
	for(k = 1; k < argc; k++) {
		if(strcmp(argv[k], "-") == 0) {
			freqflag = !freqflag;
			continue;
		}
		if(pack_file(argv[k]) < 0)
			errors++;
	}
	if(argc == 1) {
		fprintf(stderr, "usage: pack file ...\n");
		return(1);
	}
	return(errors != 0);
}

static int
pack_file(char *name)
{
	struct node *order[256], *n;
	struct stat st, zst;
	char filename[LNAME];
	int i, ncodes, treesize;
	long nchars;

	if(copy_name(filename, name) < 0)
		return(-1);
	if(filename[strlen(filename)-1] == SUF1 &&
	    filename[strlen(filename)-2] == SUF0) {
		fprintf(stderr, "%s: already packed\n", name);
		return(-1);
	}
	if(stat(name, &st) < 0) {
		perror(name);
		return(-1);
	}
	if((st.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s: not a plain file\n", name);
		return(-1);
	}
	if(st.st_nlink != 1) {
		fprintf(stderr, "%s: has links\n", name);
		return(-1);
	}
	if(append_z_suffix(filename, LNAME) < 0) {
		fprintf(stderr, "%s: file name too long\n", name);
		return(-1);
	}
	if(stat(filename, &zst) == 0) {
		fprintf(stderr, "%s: already exists\n", filename);
		return(-1);
	}
	if((infile = fopen(name, "rb")) == NULL) {
		perror(name);
		return(-1);
	}
	if((outfile = fopen(filename, "wb")) == NULL) {
		perror(filename);
		fclose(infile);
		return(-1);
	}

	used = depth = 0;
	memset(leaves, 0, sizeof(leaves));
	memset(nodes, 0, sizeof(nodes));
	memset(codes, 0, sizeof(codes));

	if(sortcount() < 0)
		goto fail;
	if(used < 2) {
		fprintf(stderr, "%s: trivial file\n", name);
		goto fail;
	}

	n = &maxnode;
	for(i = used, ncodes = used; i--; )
		order[i] = n = n->sortl;

	root = formtree();
	if(put16(PACKED) < 0)
		goto fail;
	if(put16((int)((root->freq >> 16) & 0177777)) < 0)
		goto fail;
	if(put16((int)(root->freq & 0177777)) < 0)
		goto fail;
	treesize = puttree();

	depth = 0;
	gcode(0, root);

	nchars = packed_size_estimate(ncodes, treesize);
	if(freqflag) {
		fprintf(stderr, "%s: %ld bytes, packed estimate %ld bytes\n",
		    name, root->freq, nchars);
	}
	if((nchars + 511) / 512 >= (root->freq + 511) / 512) {
		fprintf(stderr, "%s: not packed (no blocks saved)\n", name);
		goto fail;
	}

	rewind(infile);
	if(compress_file() < 0)
		goto fail;
	if(fclose(outfile) == EOF) {
		outfile = NULL;
		goto fail_inonly;
	}
	outfile = NULL;
	fclose(infile);
	infile = NULL;
	chmod(filename, st.st_mode & 07777);
	(void)chown(filename, st.st_uid, st.st_gid);
	unlink(name);
	printf("%s: %ld%% Compression\n", name,
	    100 * (root->freq - nchars) / root->freq);
	return(0);

fail:
	if(outfile != NULL) {
		fclose(outfile);
		outfile = NULL;
	}
	unlink(filename);
fail_inonly:
	if(infile != NULL) {
		fclose(infile);
		infile = NULL;
	}
	return(-1);
}

static int
copy_name(char *dst, char *src)
{
	int i, sep;

	sep = -1;
	for(i = 0; i < LNAME-3 && src[i] != '\0'; i++) {
		dst[i] = src[i];
		if(dst[i] == '/')
			sep = i;
	}
	dst[i] = '\0';
	if(src[i] != '\0' || (i-sep) > 13) {
		fprintf(stderr, "%s: file name too long\n", src);
		return(-1);
	}
	return(0);
}

static int
append_z_suffix(char *dst, int dstsz)
{
	int len;

	len = strlen(dst);
	if(len + 2 >= dstsz)
		return(-1);
	dst[len++] = '.';
	dst[len++] = 'z';
	dst[len] = '\0';
	return(0);
}

static int
sortcount(void)
{
	struct node *p, *q;
	int c;

	maxnode.sorth = sortstart.sorth = &maxnode;
	sortstart.sortl = maxnode.sortl = &sortstart;
	maxnode.freq = 2000000000L;

	while((c = getc(infile)) != EOF) {
		if((p = leaves[c]) == NULL) {
			if(used >= 256)
				return(-1);
			p = leaves[c] = &nodes[used++];
			p->zlink = NULL;
			p->olink = NULL;
			p->sym = c;
			p->freq = 1;
			q = p->sorth = sortstart.sorth;
			p->sortl = &sortstart;
			q->sortl = sortstart.sorth = p;
		} else {
			p->freq += 1;
			q = p->sorth;
			if(p->freq > q->freq) {
				do {
					q = q->sorth;
				} while(q->freq < p->freq);
				p->sortl->sorth = p->sorth;
				p->sorth->sortl = p->sortl;
				p->sortl = q->sortl;
				p->sorth = q;
				q->sortl->sorth = p;
				q->sortl = p;
			}
		}
	}
	return(0);
}

static struct node *
formtree(void)
{
	struct node *p, *q, *r;

	p = sortstart.sorth;
	while((q = p->sorth) != &maxnode) {
		r = &nodes[used++];
		r->freq = p->freq + q->freq;
		r->zlink = p;
		r->olink = q;
		r->sym = -1;
		p = q->sorth;
		while(r->freq > p->freq)
			p = p->sorth;
		r->sortl = p->sortl;
		r->sorth = p;
		p->sortl->sorth = r;
		p->sortl = r;
		p = q->sorth;
	}
	return(p);
}

static int
puttree(void)
{
	int i, extra, j;

	extra = depth = 0;
	maketree(root);
	put16(depth);
	for(i = 0; i < depth; i++) {
		j = tree[i];
		if(j < 0377) {
			putc(j, outfile);
		} else {
			putc(0377, outfile);
			put16(j);
			extra++;
		}
	}
	return(depth + extra*2);
}

static int
maketree(struct node *no)
{
	int d;

	d = depth;
	depth += 2;
	if(no->zlink == NULL) {
		tree[d] = 0;
		tree[d+1] = no->sym;
	} else {
		tree[d] = maketree(no->zlink) - d;
		tree[d+1] = maketree(no->olink) - d;
	}
	return(d);
}

static void
gcode(int len, struct node *nod)
{
	int l, bit, i, nbytes;

	l = len;
	if(nod->zlink == NULL) {
		nbytes = 1 + ((l + 7) >> 3);
		codes[nod->sym][0] = l;
		memcpy(&codes[nod->sym][1], code, nbytes - 1);
		return;
	}
	bit = 0200 >> (l & 07);
	i = l >> 3;
	code[i] &= ~bit;
	gcode(l + 1, nod->zlink);
	code[i] |= bit;
	gcode(l + 1, nod->olink);
}

static long
packed_size_estimate(int ncodes, int treesize)
{
	int i;
	long bits;

	bits = 0;
	for(i = 0; i < ncodes; i++)
		bits += nodes[i].freq * codes[nodes[i].sym][0];
	return((bits + 7) / 8 + treesize + 8);
}

static int
compress_file(void)
{
	int c, i, nbits, bits;
	unsigned int word;
	unsigned char *p;

	bits = 0;
	word = 0;
	while((c = getc(infile)) != EOF) {
		p = codes[c];
		nbits = *p++;
		for(i = 0; i < nbits; i++) {
			word <<= 1;
			if((p[i >> 3] << (i & 07)) & 0200)
				word++;
			++bits;
			if((bits & 017) == 0) {
				if(put16(word) < 0)
					return(-1);
				word = 0;
			}
		}
	}
	if(bits != 0 && put16(word << (16 - (bits & 017))) < 0)
		return(-1);
	return(fflush(outfile) == EOF ? -1 : 0);
}

static int
put16(int v)
{
	if(putc(v & 0377, outfile) == EOF)
		return(-1);
	if(putc((v >> 8) & 0377, outfile) == EOF)
		return(-1);
	return(0);
}
