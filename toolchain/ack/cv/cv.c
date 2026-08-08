/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Convert ACK a.out to the native Sun010-style a.out expected by the
 * Epoch68/V7 loader. This is derived from ACK's sun3 converter, but defaults
 * the machine id to SUN010 and prefers ZMAGIC whenever the incoming ACK layout
 * already supports it.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <out.h>
#include <object.h>

#ifndef MACH
#define MACH 1
#endif

#define	OMAGIC	0407	/* old-fashioned */
#define	NMAGIC	0410	/* text write protexted */
#define ZMAGIC	0413	/* demand paging */
#define KERNEL_BOOT_LOAD_BASE 0x00001000L

struct bhdr {
	int16_t	machtype;
	int16_t	magic;
	int32_t	tsize;
	int32_t	dsize;
	int32_t	bsize;
	int32_t	ssize;
	int32_t	entry;
	int32_t	rtsize;
	int32_t	rdsize;
};

struct machrelo {
	int32_t	address;
	int32_t	relodata;
};
#define setpcrel(X,f)	(X |= (f<<7))
#define setsymbolnum(X,n)	(X = (X & 0377) | ((long)n << 8))
#define setextern(X,f)	(X |= (f << 4))
#define setlength(X,l)	(X = (X & ~0x60)|((long) l << 5))

struct sym {
	int32_t	name;
	int8_t	type;
	int8_t	other;
	int16_t	desc;
	int32_t	value;
};

#define N_UNDF	0
#define	N_ABS	02
#define	N_TEXT	04
#define	N_DATA	06
#define	N_BSS	010
#define	N_EXT	01
#define N_FN	0x1f

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

char	*output_file;
int	outputfile_created;
long magic;

int rom_in_data;

char *program ;

char flag ;

/* Output file definitions and such */

struct bhdr bh;

#define ENTRY 0x02000
#define TOT_HDRSIZE	(sizeof(struct bhdr))



#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

int		output;

int unresolved;
long	textsize ; 
long	datasize ;
long	bsssize;
int kernel_image;

static long align(long a, long b);
static int follows(struct outsect *pa, struct outsect *pb);
static void writef(const void *addr, int sz, long cnt);
static void emits(struct outsect *section);
static void emit_relo(void);
static long get(int sz);
static void put(long l, int sz);
static void patch(struct outrelo *ap, struct outname *an, struct machrelo *mp);
static void cvlong(int32_t *l);
static void cvshort(int16_t *s);
static int is_rest_local(struct outname *A, int i);
static void emit_symtab(void);
static void fatal(const char *s, ...);
void rd_fatal(void);

static long
align(long a, long b)
{
	a += b - 1;
	return a - a % b;
}

static int
follows(struct outsect *pa, struct outsect *pb)
{
	/* return 1 if pa follows pb */

	return pa->os_base == align(pb->os_base+pb->os_size, pa->os_lign);
}

int
main(int argc, char *argv[])
{
	int		nsect;

	program= argv[0] ;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		if (flag == 'u') unresolved++;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 3:	if ((output = creat(argv[2], 0644)) < 0 ||
		    (close(output), output = open(argv[2],2)) < 0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		outputfile_created = 1;
		if (! rd_open(argv[1]))
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s [-u]  <ACK object> <Sun object>.\n", program);
	}
	rd_ohead(&outhead);
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_flags & HF_LINK) {
		if (! unresolved) {
			fprintf(stderr,"Warning: contains unresolved references.\n");
		}
		unresolved++;
	}
	else if (outhead.oh_nrelo > 0 && !unresolved)
		fprintf(stderr, "Warning: relocation information present.\n");
	if ( outhead.oh_nsect!=LSECT && outhead.oh_nsect!=NSECT )
		fatal("Input file must have %d sections, not %ld\n",
			NSECT,outhead.oh_nsect) ;
	rd_sect(outsect, outhead.oh_nsect);
	/* A few checks */
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( !unresolved && ! follows(&outsect[BSSSG], &outsect[DATASG]))
		fatal("bss segment must follow data segment\n") ;
	if ( outsect[ROMSG].os_lign == 0x20000 ) {
		/* 410/413 file with ROMSG in data space */
		rom_in_data = 1;
		magic= NMAGIC ;
		textsize= outsect[TEXTSG].os_size ;
		datasize= outsect[BSSSG].os_base - outsect[ROMSG].os_base ;
		if (! follows(&outsect[DATASG], &outsect[ROMSG]))
			fatal("data segment must follow rom\n") ;
		outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
	} else
	if ( outsect[DATASG].os_lign == 0x20000 ) {
		/* 410/413 file with ROMSG in instruction space */
		rom_in_data = 0;
		magic= NMAGIC ;
		textsize= (outsect[ROMSG].os_base - outsect[TEXTSG].os_base) +
				outsect[ROMSG].os_size ;
		if (! follows(&outsect[ROMSG],&outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
		outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
		outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		datasize= outsect[DATASG].os_size ;
	} else {
		/* Plain 407 file */
		rom_in_data = 0;
		magic= OMAGIC ;
		if (!unresolved) {
			textsize= (outsect[DATASG].os_base - outsect[TEXTSG].os_base);
			if (! follows(&outsect[ROMSG],&outsect[TEXTSG]))
				fatal("rom segment must follow text\n") ;
			if (! follows(&outsect[DATASG],&outsect[ROMSG]))
				fatal("data segment must follow rom\n") ;
			outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
			outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
			outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
		}
		else {
			textsize = outsect[TEXTSG].os_size+outsect[ROMSG].os_size;
		}
		datasize = outsect[DATASG].os_size;
	}
	/*
	 * Epoch68/V7 loads text from file offset sizeof(struct bhdr) to user
	 * address zero, and stores a_text as header plus loaded text size.
	 * Accept three ACK layouts:
	 *  - zero-based userland text
	 *  - paged userland text at TOT_HDRSIZE+ENTRY
	 *  - kernel text based at BOOT_LOAD_BASE
	 */
	kernel_image = 0;
	if (outsect[TEXTSG].os_base == 0) {
		magic = ZMAGIC;
		textsize += TOT_HDRSIZE;
	} else if (outsect[TEXTSG].os_base == KERNEL_BOOT_LOAD_BASE) {
		magic = ZMAGIC;
		textsize += TOT_HDRSIZE;
		kernel_image = 1;
	} else if (outsect[TEXTSG].os_base == TOT_HDRSIZE+ENTRY) {
		magic = ZMAGIC;
		textsize = (textsize + TOT_HDRSIZE + (0x2000 - 1)) & ~(0x2000 - 1);
		datasize = (datasize + (0x2000 - 1)) & ~(0x2000 - 1);
	}
	bsssize = outsect[BSSSG].os_size;
	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT],&outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	if (magic != OMAGIC && unresolved) {
		fatal("unresolved references with wrong magic number\n");
	}

	if ((magic == ZMAGIC &&
	    outsect[TEXTSG].os_base != 0 &&
	    outsect[TEXTSG].os_base != KERNEL_BOOT_LOAD_BASE &&
	    outsect[TEXTSG].os_base != TOT_HDRSIZE+ENTRY) ||
	    (magic != ZMAGIC && !unresolved && outsect[TEXTSG].os_base != ENTRY)) {
		fatal("Illegal entry point.\n");
	}

	bh.magic = magic;
	bh.machtype = MACH;
	bh.tsize = textsize;
	bh.bsize = bsssize;
	bh.dsize = datasize;
	bh.rtsize = 0;
	bh.rdsize = 0;
	if (magic == ZMAGIC && outsect[TEXTSG].os_base == 0) bh.entry = 0;
	else if (magic == ZMAGIC && outsect[TEXTSG].os_base == KERNEL_BOOT_LOAD_BASE) bh.entry = KERNEL_BOOT_LOAD_BASE;
	else if (magic == ZMAGIC) bh.entry = TOT_HDRSIZE+ENTRY;
	else if (!unresolved)	bh.entry = ENTRY;
	else bh.entry = 0;

	/* Action at last */
	lseek(output,(long) TOT_HDRSIZE,0);
	emits(&outsect[TEXTSG]) ;
	if (rom_in_data && magic == ZMAGIC) {
		lseek(output,textsize,0);
	}
	emits(&outsect[ROMSG]) ;
	if (!rom_in_data && magic == ZMAGIC) {
		lseek(output,textsize,0);
	}
	emits(&outsect[DATASG]) ;
	if (magic == ZMAGIC) {
		lseek(output,textsize + datasize,0);
	}
	if (unresolved) emit_relo();
	if (!kernel_image) {
		emit_symtab();
		bh.ssize = outhead.oh_nname * sizeof(struct sym);
	}
	else {
		bh.ssize = 0;
	}
	lseek(output,0L,0);
	cvshort(&(bh.machtype));
	cvshort(&(bh.magic));
	cvlong(&(bh.tsize));
	cvlong(&(bh.dsize));
	cvlong(&(bh.bsize));
	cvlong(&(bh.ssize));
	cvlong(&(bh.entry));
	cvlong(&(bh.rtsize));
	cvlong(&(bh.rdsize));
	writef(&bh, 1, (long) TOT_HDRSIZE);
	if ( outputfile_created  && !unresolved ) chmod(argv[2],0755);
	return 0;
}

static void
writef(const void *addr, int sz, long cnt)
{
	const char *p = (const char *) addr;

	cnt *= sz;

	while (cnt) {
		int i = cnt >= 0x4000 ? 0x4000 : cnt;

		cnt -= i;
		if (write(output, p, i) < i) {
			fatal("write error\n");
		}
		p += i;
	}
}

/*
 * Transfer the emitted byted from one file to another.
 */
static void
emits(struct outsect *section)
{
	char		*p;
	long sz = section->os_flen;

	rd_outsect(section - outsect);
	while (sz) {
		unsigned int i = (sz >= 0x4000 ? 0x4000 : sz);
		if (!(p = malloc(i))) {
			fatal("No memory.\n");
		}
		rd_emit(p, i);
		if (write(output, p, i) < i) {
			fatal("write error.\n");
		}
		free(p);
		sz -= i;
	}

	sz = section->os_size - section->os_flen;
	if (sz) {
		if (!(p = calloc(0x4000, 1))) {
			fatal("No memory.\n");
		}
		while (sz) {
			unsigned int i = (sz >= 0x4000 ? 0x4000 : sz);
			if (write(output, p, i) < i) {
				fatal("write error.\n");
			}
			sz -= i;
		}
		free(p);
	}
}

struct outname *ACKnames;

static void
emit_relo(void)
{
	struct outrelo *ACKrelo;
	struct machrelo *MACHtrelo,*MACHdrelo;
	struct outrelo *ap;
	struct machrelo *mtp, *mdp;
	unsigned int cnt = outhead.oh_nrelo;

	ACKrelo = (struct outrelo *) calloc(cnt, sizeof(struct outrelo));
	MACHtrelo = (struct machrelo *) calloc(cnt, sizeof(struct machrelo));
	MACHdrelo = (struct machrelo *) calloc(cnt, sizeof(struct machrelo));
	ACKnames = (struct outname *) calloc(outhead.oh_nname, sizeof(struct outname));
	if (!(ap = ACKrelo) || !(mtp = MACHtrelo) || !(mdp = MACHdrelo) ||
	    !ACKnames) {
		fatal("No memory.\n");
	}
	rd_relo(ACKrelo, cnt);
	rd_name(ACKnames, outhead.oh_nname);
	while (cnt-- != 0) {
		struct machrelo *mp;

		if (ap->or_sect - S_MIN <= ROMSG) mp = mtp++;
		else mp = mdp++;
		setlength(mp->relodata,(ap->or_type&RELSZ) >> 1);
		setpcrel(mp->relodata, ((ap->or_type & RELPC) != 0));
		mp->address = ap->or_addr;
		if (ap->or_sect == ROMSG+S_MIN) {
			mp->address += outsect[TEXTSG].os_size;
		}
		if (ap->or_nami < outhead.oh_nname) {
			if (ACKnames[ap->or_nami].on_type & S_EXT) {
				setsymbolnum(mp->relodata, ap->or_nami);
				setextern(mp->relodata,1);
			}
			else {
				patch(ap, &ACKnames[ap->or_nami], mp);
			}
		}
		else {
			setsymbolnum(mp->relodata, N_ABS);
		}
		cvlong(&(mp->address));
		cvlong(&(mp->relodata));
		ap++;
	}
	bh.rtsize = (char *) mtp - (char *) MACHtrelo;
	bh.rdsize = (char *) mdp - (char *) MACHdrelo;
	writef(MACHtrelo, 1, bh.rtsize);
	writef(MACHdrelo, 1, bh.rdsize);
	free(ACKrelo);
	free(MACHtrelo);
	free(MACHdrelo);
}

static long
get(int sz)
{
	char buf[10];
	long l = 0;
	char *p = buf;

	read(output,buf,sz);
	while (sz--) {
		l = (l << 8) | (*p++ & 0377);
	}
	return l;
}

static void
put(long l, int sz)
{
	char buf[10];
	char *p = buf;

	*p++ = l >> 24;
	*p++ = l >> 16;
	*p++ = l >> 8;
	*p++ = l;
	p -= sz;
	if (write(output, p, sz) < sz) {
		fatal("write error.\n");
	}
}

static void
patch(struct outrelo *ap, struct outname *an, struct machrelo *mp)
{
	int whichsect = (an->on_type & S_TYP) - S_MIN;
	long correction = 0;
	long where = TOT_HDRSIZE+ap->or_addr;
	long X;
	long here;
	int sz;

	if (!(an->on_type & S_SCT)) {
		fprintf(stderr,"funny on_type %x\n", an->on_type);
	}
	switch(whichsect) {
	case TEXTSG:
		setsymbolnum(mp->relodata,N_TEXT);
		return;
	case DATASG:
		correction = outsect[ROMSG].os_size + outsect[TEXTSG].os_size;
		setsymbolnum(mp->relodata,N_DATA);
		break;
	case ROMSG:
		correction = outsect[TEXTSG].os_size;
		setsymbolnum(mp->relodata,N_TEXT);
		break;
	case BSSSG:
		correction = outsect[ROMSG].os_size + outsect[TEXTSG].os_size+
				outsect[DATASG].os_size;
		setsymbolnum(mp->relodata,N_BSS);
		break;
	default:
		assert(0);
	}

	switch(ap->or_sect - S_MIN) {
	case DATASG:
		where += outsect[ROMSG].os_size;
	case ROMSG:
		where += outsect[TEXTSG].os_size;
	case TEXTSG:
		break;
	default:
		assert(0);
	}
	here = lseek(output, 0L, 1);
	lseek(output, where, 0);
	sz = ap->or_type & RELSZ;
	X = get(sz) + correction;
	lseek(output, where, 0);
	put(X,sz);
	lseek(output, here, 0);
}

static void
cvlong(int32_t *l)
{
	int32_t x = *l;
	char *p = (char *) l;

	*p++ = x >> 24;
	*p++ = x >> 16;
	*p++ = x >> 8;
	*p = x;
}

static void
cvshort(int16_t *s)
{
	int16_t x = *s;
	char *p = (char *) s;

	*p++ = x >> 8;
	*p = x;
}

static int
is_rest_local(struct outname *A, int i)
{
	while (i--) {
		if (A->on_type & S_EXT) return 0;
		A++;
	}
	return 1;
}

static void
emit_symtab(void)
{
	unsigned short i = outhead.oh_nname;
	struct outname *A;
	struct sym *MACHnames;
	struct sym *M;
	char *chars;
	long offX = OFF_CHAR(outhead) - 4;

	if (!(A = ACKnames)) {
	    	if (!(A = (struct outname *)
			calloc(i, sizeof(struct outname)))) {
			fatal("No memory.\n");
		}
		rd_name(A, outhead.oh_nname);
	}
	if (!(M = (struct sym *) calloc(i, sizeof(struct sym)))) {
		fatal("No memory.\n");
	}
	MACHnames = M;
	ACKnames = A;
	for (; i; i--, A++) {
		M->value = A->on_valu;
		M->desc = A->on_desc;
		if ((A->on_type & S_SCT) ||
		    (A->on_type & S_ETC) == S_FIL) {
			static int rest_local;
			if (! unresolved || rest_local || (rest_local = is_rest_local(A, i))) {
				outhead.oh_nname--;
				continue;
			}
		}
		if (A->on_type & S_STB) {
			M->type = A->on_type >> 8;
		}
		else if (A->on_type & S_COM) {
			M->type = N_UNDF | N_EXT;
		}
		else switch(A->on_type & S_TYP) {
			case S_UND:
				switch(A->on_type & S_ETC) {
				default:
					M->type = N_UNDF;
					break;
				case S_MOD:
					M->type = N_FN;
					break;
				case S_LIN:
					M->type = N_ABS;
					break;
				}
				break;
			case S_ABS:
				M->type = N_ABS;
				break;
			case S_MIN + TEXTSG:
				M->type = N_TEXT; 
				break;
			case S_MIN + ROMSG:
				if (unresolved) {
					M->value += outsect[TEXTSG].os_size;
				}
				M->type = (rom_in_data ? N_DATA : N_TEXT);
				break;
			case S_MIN + DATASG:
				if (unresolved) {
					M->value += outsect[TEXTSG].os_size +
						    outsect[ROMSG].os_size;
				}
				M->type = N_DATA;
				break;
			case S_MIN + BSSSG:
				if (unresolved) {
					M->value += outsect[TEXTSG].os_size +
						    outsect[ROMSG].os_size +
						    outsect[DATASG].os_size;
				}
				M->type = N_BSS;
				break;
			case S_MIN + LSECT:
				M->type = N_BSS;
				break;
			default:
				fprintf(stderr,"warning: unknown s_type: %d\n",
					A->on_type & S_TYP);
		}
		if (A->on_type & S_EXT) M->type |= N_EXT;
		M->name = A->on_foff;
		M++;
	}
	M = MACHnames;
	for (i = outhead.oh_nname; i; i--, M++) {
		if (M->name) {
			M->name -= offX;
		}
		else M->name = outhead.oh_nchar + 3;	/* pointer to nullbyte */
		cvlong(&(M->name));
		cvlong(&(M->value));
		cvshort(&(M->desc));
	}
	writef(MACHnames, sizeof(struct sym), (long) outhead.oh_nname);
	free(MACHnames);
	free(ACKnames);
	if ((unsigned) outhead.oh_nchar != outhead.oh_nchar ||
	    !( chars = malloc((unsigned) outhead.oh_nchar))) {
		fatal("No memory\n.");
	}
	put(outhead.oh_nchar+4,4);
	rd_string(chars,outhead.oh_nchar);
	writef(chars, 1, outhead.oh_nchar);
	free(chars);
}

/* VARARGS1 */
static void
fatal(const char *s, ...)
{
	va_list ap;

	fprintf(stderr,"%s: ",program) ;
	va_start(ap, s);
	vfprintf(stderr, (char *)s, ap);
	va_end(ap);
	if (outputfile_created)
		unlink(output_file);
	exit(-1);
}

void
rd_fatal(void)
{
	fatal("read error.\n");
}
