/*
**	print symbol tables for
**	object or archive files
**
**	nm [-goprun] [name ...]
*/



#include	<ar.h>
#include	<a.out.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#define	MAGIC	execbuf.a_magic
#define	BADMAG	(A_GETMID(execbuf) != A_MID_SUN010 || A_GETMAGIC(execbuf) != A_MAGIC5)
#define	SELECT	arch_flg ? arp.ar_name : *argv
int	numsort_flg;
int	undef_flg;
int	revsort_flg = 1;
int	globl_flg;
int	nosort_flg;
int	arch_flg;
int	prep_flg;
struct	ar_hdr	arp;
struct	exec	execbuf;
FILE	*fi;
long	off;

struct nment {
	struct nlist sym;
	char *name;
};

struct ackhead {
	u16 flags;
	u16 nsect;
	u16 nrelo;
	u16 nname;
	u32 nemit;
	u32 nchar;
};

struct acknment {
	u32 value;
	int type;
	char *name;
};

#define ACK_MAGIC 0x0202
#define ACK_HEAD_SIZE 20
#define ACK_SECT_SIZE 20
#define ACK_RELO_SIZE 10
#define ACK_NAME_SIZE 12
#define ACK_S_TYP 0x007f
#define ACK_S_EXT 0x0080
#define ACK_S_ETC 0x7f00
#define ACK_S_UND 0x0000
#define ACK_S_ABS 0x0001
#define ACK_S_MIN 0x0002
#define ACK_S_SCT 0x0100
#define ACK_S_FIL 0x0300
#define ACK_S_COM 0x1000
#define ACK_ARHDR_SIZE 26

i32 compare(const void *vp1, const void *vp2);
int nextel(FILE *f);
static i32 ackcompare(const void *vp1, const void *vp2);
static u16 ackget2(const unsigned char *p);
static u32 ackget4(const unsigned char *p);
static long filesize(FILE *f);
static int ackobject(FILE *f, long base, long size, char *file,
    char *member, int many);
static int ackarchive(FILE *f, long size, char *file);

int
main (int argc, char **argv)
{
	int narg;

	if (--argc>0 && argv[1][0]=='-' && argv[1][1]!=0) {
		argv++;
		while (*++*argv) switch (**argv) {
		case 'n':		/* sort numerically */
			numsort_flg++;
			continue;

		case 'g':		/* globl symbols only */
			globl_flg++;
			continue;

		case 'u':		/* undefined symbols only */
			undef_flg++;
			continue;

		case 'r':		/* sort in reverse order */
			revsort_flg = -1;
			continue;

		case 'p':		/* don't sort -- symbol table order */
			nosort_flg++;
			continue;

		case 'o':		/* prepend a name to each line */
			prep_flg++;
			continue;

		default:		/* oops */
			fprintf(stderr, "nm: invalid argument -%c\n", *argv[0]);
			exit(1);
		}
		argc--;
	}
	if (argc == 0) {
		argc = 1;
		argv[1] = "a.out";
	}
	narg = argc;
	while(argc--) {
		u16 ar_magic;
		unsigned char magic[2];
		long fsize;

		fi = fopen(*++argv,"r");
		if (fi == NULL) {
			fprintf(stderr, "nm: cannot open %s\n", *argv);
			continue;
		}
		arch_flg = 0;
		fsize = filesize(fi);
		if (fsize < 2 || fread((char *)magic, 1, 2, fi) != 2) {
			fprintf(stderr, "nm: %s-- bad format\n", *argv);
			fclose(fi);
			continue;
		}
		if (ackget2(magic) == ACK_MAGIC) {
			ackobject(fi, 0L, fsize, *argv, (char *)0, narg > 1);
			fclose(fi);
			continue;
		}
		if (ackget2(magic) == AALMAG || ackget2(magic) == ARMAG) {
			ackarchive(fi, fsize, *argv);
			fclose(fi);
			continue;
		}
		fseek(fi, 0L, 0);
		off = sizeof(ar_magic);
		fread((char *)&ar_magic, 1, sizeof(ar_magic), fi);
		if (ar_magic == ARMAG)
			arch_flg++;
		else {
			fseek(fi, 0L, 0);
			fread((char *)&execbuf, 1, sizeof(execbuf), fi);
			if (BADMAG) {
			fprintf(stderr, "nm: %s-- bad format\n", *argv);
			fclose(fi);
			continue;
			}
		}
		fseek(fi, 0L, 0);
		if (arch_flg) {
			nextel(fi);
			if (narg > 1)
				printf("\n%s:\n", *argv);
		}
		do {
			long base, o;
			register i, j, n, c;
			struct nment *symp = NULL;
			struct nment ent;
			char *strtab;
			u32 strsize;

			base = ftell(fi);
			fread((char *)&execbuf, 1, sizeof(struct exec), fi);
			if (BADMAG)		/* archive element not in  */
				continue;	/* proper format - skip it */
			o = base + (long)execbuf.a_text + execbuf.a_data;
			fseek(fi, o, 0);
			n = execbuf.a_syms / sizeof(struct nlist);
			if (n == 0) {
				fprintf(stderr, "nm: %s-- no name list\n", SELECT);
				continue;
			}
			symp = (struct nment *)malloc(n * sizeof(struct nment));
			if (symp == NULL) {
				fprintf(stderr, "nm: out of memory on %s\n", *argv);
				exit(2);
			}
			for (i=0; i<n; i++)
				fread((char *)&symp[i].sym, 1, sizeof(struct nlist), fi);
			fread((char *)&strsize, 1, sizeof(strsize), fi);
			if (strsize < sizeof(strsize)) {
				fprintf(stderr, "nm: %s-- bad string table\n", SELECT);
				free((char *)symp);
				continue;
			}
			strtab = malloc((unsigned)strsize);
			if (strtab == NULL) {
				fprintf(stderr, "nm: out of memory on %s\n", *argv);
				exit(2);
			}
			*((u32 *)strtab) = strsize;
			fread(strtab + sizeof(strsize), 1,
			    (unsigned)(strsize - sizeof(strsize)), fi);
			j = 0;
			for (i=0; i<n; i++) {
				ent = symp[i];
				if (ent.sym.n_strx < strsize)
					ent.name = strtab + ent.sym.n_strx;
				else
					ent.name = "";
				if (globl_flg && (ent.sym.n_type&N_EXT)==0)
					continue;
				switch (ent.sym.n_type&N_TYPE) {

				case N_UNDF:
					c = 'u';
					if (ent.sym.n_value)
						c = 'c';
					break;

				default:
				case N_ABS:
					c = 'a';
					break;

				case N_TEXT:
					c = 't';
					break;

				case N_DATA:
					c = 'd';
					break;

				case N_BSS:
					c = 'b';
					break;

				case N_FN:
					c = 'f';
					break;

				}
				if (undef_flg && c!='u')
					continue;
				if (ent.sym.n_type&N_EXT)
					c = toupper(c);
				ent.sym.n_type = c;
				symp[j++] = ent;
			}
			i = j;
			if (nosort_flg==0)
				qsort(symp, i, sizeof(struct nment), compare);
			if ((arch_flg || narg>1) && prep_flg==0)
				printf("\n%s:\n", SELECT);
			for (n=0; n<i; n++) {
				if (prep_flg) {
					if (arch_flg)
						printf("%s:", *argv);
					printf("%s:", SELECT);
				}
				c = symp[n].sym.n_type;
				if (!undef_flg) {
					if (c=='u' || c=='U')
						printf("      ");
					else
						printf(FORMAT, symp[n].sym.n_value);
					printf(" %c ", c);
				}
				printf("%s\n", symp[n].name);
			}
			free((char *)strtab);
			free((char *)symp);
		} while(arch_flg && nextel(fi));
		fclose(fi);
	}
	exit(0);
}

static u16
ackget2(const unsigned char *p)
{
	return((u16)((u32)p[0] | ((u32)p[1] << 8)));
}

static u32
ackget4(const unsigned char *p)
{
	return((u32)ackget2(p) | ((u32)ackget2(p + 2) << 16));
}

static long
filesize(FILE *f)
{
	long here, size;

	here = ftell(f);
	if (fseek(f, 0L, 2) != 0)
		return(-1L);
	size = ftell(f);
	fseek(f, here, 0);
	return(size);
}

static int
acktype(u16 type)
{
	int c;
	u16 section;

	section = type & ACK_S_TYP;
	if ((type & ACK_S_ETC) == ACK_S_COM)
		c = 'c';
	else if ((type & ACK_S_ETC) == ACK_S_FIL)
		c = 'f';
	else if ((type & ACK_S_ETC) == ACK_S_SCT)
		c = 's';
	else switch (section) {
	case ACK_S_UND:
		c = 'u';
		break;
	case ACK_S_ABS:
		c = 'a';
		break;
	case ACK_S_MIN:
		c = 't';
		break;
	case ACK_S_MIN + 1:	/* read-only data */
	case ACK_S_MIN + 2:
		c = 'd';
		break;
	case ACK_S_MIN + 3:
		c = 'b';
		break;
	default:
		c = 's';
		break;
	}
	if (type & ACK_S_EXT)
		c = toupper(c);
	return(c);
}

static int
ackobject(FILE *f, long base, long size, char *file, char *member, int many)
{
	unsigned char rawhead[ACK_HEAD_SIZE];
	unsigned char rawname[ACK_NAME_SIZE];
	struct ackhead head;
	struct acknment *symp;
	char *strings;
	char *select;
	long nameoff, charoff, endoff;
	u32 stroff;
	int i, j, n, c;

	if (size < ACK_HEAD_SIZE || fseek(f, base, 0) != 0 ||
	    fread((char *)rawhead, 1, ACK_HEAD_SIZE, f) != ACK_HEAD_SIZE)
		return(0);
	if (ackget2(rawhead) != ACK_MAGIC)
		return(0);
	head.flags = ackget2(rawhead + 4);
	head.nsect = ackget2(rawhead + 6);
	head.nrelo = ackget2(rawhead + 8);
	head.nname = ackget2(rawhead + 10);
	head.nemit = ackget4(rawhead + 12);
	head.nchar = ackget4(rawhead + 16);
	nameoff = ACK_HEAD_SIZE + (long)head.nsect * ACK_SECT_SIZE;
	nameoff += (long)head.nemit + (long)head.nrelo * ACK_RELO_SIZE;
	charoff = nameoff + (long)head.nname * ACK_NAME_SIZE;
	endoff = charoff + (long)head.nchar;
	if (nameoff < ACK_HEAD_SIZE || charoff < nameoff || endoff < charoff ||
	    endoff > size || head.nchar >= 65535L ||
	    (long)head.nname * sizeof(struct acknment) >= 65535L) {
		fprintf(stderr, "nm: %s-- bad ACK object\n", member ? member : file);
		return(1);
	}
	select = member ? member : file;
	if (head.nname == 0) {
		fprintf(stderr, "nm: %s-- no name list\n", select);
		return(1);
	}
	strings = malloc((unsigned)(head.nchar + 1));
	symp = (struct acknment *)malloc((unsigned)((long)head.nname *
	    sizeof(*symp)));
	if (strings == NULL || symp == NULL) {
		fprintf(stderr, "nm: out of memory on %s\n", file);
		exit(2);
	}
	if (fseek(f, base + charoff, 0) != 0 ||
	    fread(strings, 1, (unsigned)head.nchar, f) != (unsigned)head.nchar) {
		fprintf(stderr, "nm: %s-- bad ACK string table\n", select);
		free(strings);
		free((char *)symp);
		return(1);
	}
	strings[head.nchar] = 0;
	j = 0;
	for (i = 0; i < head.nname; i++) {
		u16 type;
		char *p;
		char *limit;

		if (fseek(f, base + nameoff + (long)i * ACK_NAME_SIZE, 0) != 0 ||
		    fread((char *)rawname, 1, ACK_NAME_SIZE, f) != ACK_NAME_SIZE)
			break;
		stroff = ackget4(rawname);
		type = ackget2(rawname + 4);
		if (stroff == 0)
			continue;
		if (stroff < (u32)charoff || stroff >= (u32)endoff) {
			fprintf(stderr, "nm: %s-- bad ACK symbol name\n", select);
			continue;
		}
		p = strings + (stroff - (u32)charoff);
		limit = strings + head.nchar;
		while (p < limit && *p)
			p++;
		if (p == limit) {
			fprintf(stderr, "nm: %s-- bad ACK symbol name\n", select);
			continue;
		}
		if (globl_flg && (type & ACK_S_EXT) == 0)
			continue;
		c = acktype(type);
		if (undef_flg && c != 'u' && c != 'U')
			continue;
		symp[j].name = strings + (stroff - (u32)charoff);
		symp[j].type = c;
		symp[j].value = ackget4(rawname + 8);
		j++;
	}
	if (i != head.nname)
		fprintf(stderr, "nm: %s-- bad ACK name table\n", select);
	if (nosort_flg == 0)
		qsort(symp, j, sizeof(*symp), ackcompare);
	if ((member != (char *)0 || many) && prep_flg == 0)
		printf("\n%s:\n", select);
	for (n = 0; n < j; n++) {
		if (prep_flg) {
			if (member != (char *)0)
				printf("%s:", file);
			printf("%s:", select);
		}
		c = symp[n].type;
		if (!undef_flg) {
			if (c == 'u' || c == 'U')
				printf("      ");
			else
				printf(FORMAT, symp[n].value);
			printf(" %c ", c);
		}
		printf("%s\n", symp[n].name);
	}
	free(strings);
	free((char *)symp);
	return(1);
}

static int
ackarchive(FILE *f, long size, char *file)
{
	unsigned char hdr[ACK_ARHDR_SIZE];
	char member[15];
	long off, membersize, dataoff;
	int i, found;

	off = 2;
	found = 0;
	while (off + ACK_ARHDR_SIZE <= size) {
		if (fseek(f, off, 0) != 0 ||
		    fread((char *)hdr, 1, ACK_ARHDR_SIZE, f) != ACK_ARHDR_SIZE)
			break;
		for (i = 0; i < 14; i++)
			member[i] = hdr[i];
		member[14] = 0;
		for (i = 13; i >= 0 && (member[i] == 0 || member[i] == ' '); i--)
			member[i] = 0;
		membersize = ((long)ackget2(hdr + 22) << 16) |
		    (long)ackget2(hdr + 24);
		dataoff = off + ACK_ARHDR_SIZE;
		if (membersize < 0 || dataoff + membersize < dataoff ||
		    dataoff + membersize > size) {
			fprintf(stderr, "nm: %s-- bad archive\n", file);
			return(1);
		}
		if (ackobject(f, dataoff, membersize, file, member, 1))
			found = 1;
		off = dataoff + membersize + (membersize & 1);
	}
	if (!found)
		fprintf(stderr, "nm: %s-- no object members\n", file);
	return(1);
}

static i32
ackcompare(const void *vp1, const void *vp2)
{
	const struct acknment *p1 = (const struct acknment *)vp1;
	const struct acknment *p2 = (const struct acknment *)vp2;
	int i;

	if (numsort_flg) {
		if (p1->value > p2->value)
			return(revsort_flg);
		if (p1->value < p2->value)
			return(-revsort_flg);
	}
	i = strcmp(p1->name, p2->name);
	if (i > 0)
		return(revsort_flg);
	if (i < 0)
		return(-revsort_flg);
	return((i32)0);
}

i32
compare (const void *vp1, const void *vp2)
{
	struct nment *p1 = (struct nment *)vp1;
	struct nment *p2 = (struct nment *)vp2;
	register i;

	if (numsort_flg) {
		if (p1->sym.n_value > p2->sym.n_value)
			return(revsort_flg);
		if (p1->sym.n_value < p2->sym.n_value)
			return(-revsort_flg);
	}
	i = strcmp(p1->name, p2->name);
	if (i > 0)
		return(revsort_flg);
	if (i < 0)
		return(-revsort_flg);
	return((i32)0);
}

int
nextel (FILE *af)
{
	register r;

	fseek(af, off, 0);
	r = fread((char *)&arp, 1, sizeof(struct ar_hdr), af);  /* read archive header */
	if (r <= 0)
		return(0);
	if (arp.ar_size & 1)
		++arp.ar_size;
	off = ftell(af) + arp.ar_size;	/* offset to next element */
	return(1);
}
