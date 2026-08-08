/*
 * Convert absolute Motorola S-records to the Sun010 a.out form currently
 * accepted by the Epoch68 V7 exec path.
 *
 * This is intentionally small and absolute-only: it is for single-file
 * 68kasm output, not for relocation or linking.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#define HDRSIZE 32
#define MAGIC0 0x00
#define MAGIC1 0x01
#define MAGIC2 0x01
#define MAGIC3 0x0b
#define MAXIMAGE 131072L
#define IMAGE_CHUNK 16384

static unsigned char image0[IMAGE_CHUNK];
static unsigned char image1[IMAGE_CHUNK];
static unsigned char image2[IMAGE_CHUNK];
static unsigned char image3[IMAGE_CHUNK];
static unsigned char image4[IMAGE_CHUNK];
static unsigned char image5[IMAGE_CHUNK];
static unsigned char image6[IMAGE_CHUNK];
static unsigned char image7[IMAGE_CHUNK];
static unsigned char *image[] = {
	image0, image1, image2, image3, image4, image5, image6, image7
};
static long lowaddr;
static long highaddr;
static int sawdata;

static int hexbyte(char *p);
static int hexnib(int c);
static long hexval(char *p, long n);
static void put32(unsigned char *p, long v);
static int writeimage(FILE *out, long size);
static void usage(void);

int
main(int argc, char *argv[])
{
	FILE *in, *out;
	char line[128];
	unsigned char hdr[HDRSIZE];
	long addr, rel, count, naddr, ndata, i;
	int type, sum, cksum;
	char *inname, *outname;

	if(argc != 3)
		usage();
	inname = argv[1];
	outname = argv[2];
	in = fopen(inname, "r");
	if(in == NULL) {
		fprintf(stderr, "srec2aout: cannot open %s\n", inname);
		exit(1);
	}

	lowaddr = MAXIMAGE;
	highaddr = 0;
	sawdata = 0;
	while(fgets(line, sizeof(line), in) != NULL) {
		if(line[0] != 'S')
			continue;
		type = line[1] - '0';
		if(type != 1 && type != 2 && type != 3)
			continue;
		count = hexbyte(&line[2]);
		if(count < 0) {
			fprintf(stderr, "srec2aout: bad count\n");
			exit(1);
		}
		if(type == 1)
			naddr = 2;
		else if(type == 2)
			naddr = 3;
		else
			naddr = 4;
		if(count < naddr + 1) {
			fprintf(stderr, "srec2aout: bad record length\n");
			exit(1);
		}
		addr = hexval(&line[4], naddr);
		if(addr < 0) {
			fprintf(stderr, "srec2aout: bad address\n");
			exit(1);
		}
		sum = count;
		for(i = 0; i < naddr; i++)
			sum += (addr >> (8 * (naddr - i - 1))) & 0xff;
		ndata = count - naddr - 1;
		if(addr < lowaddr)
			lowaddr = addr;
		if(addr + ndata > highaddr)
			highaddr = addr + ndata;
		if(highaddr - lowaddr > MAXIMAGE) {
			fprintf(stderr, "srec2aout: image too large\n");
			exit(1);
		}
		for(i = 0; i < ndata; i++) {
			int b;

			b = hexbyte(&line[4 + naddr * 2 + i * 2]);
			if(b < 0) {
				fprintf(stderr, "srec2aout: bad data\n");
				exit(1);
			}
			sum += b;
			rel = addr + i - lowaddr;
			if(rel < 0 || rel >= MAXIMAGE) {
				fprintf(stderr, "srec2aout: image address range too large\n");
				exit(1);
			}
			image[rel / IMAGE_CHUNK][rel % IMAGE_CHUNK] = b;
		}
		cksum = hexbyte(&line[4 + naddr * 2 + ndata * 2]);
		if(cksum < 0 || (((sum + cksum) & 0xff) != 0xff)) {
			fprintf(stderr, "srec2aout: checksum error\n");
			exit(1);
		}
		sawdata = 1;
	}
	fclose(in);

	if(!sawdata) {
		fprintf(stderr, "srec2aout: no data records\n");
		exit(1);
	}

	out = fopen(outname, "w");
	if(out == NULL) {
		fprintf(stderr, "srec2aout: cannot create %s\n", outname);
		exit(1);
	}

	for(i = 0; i < HDRSIZE; i++)
		hdr[i] = 0;
	hdr[0] = MAGIC0;
	hdr[1] = MAGIC1;
	hdr[2] = MAGIC2;
	hdr[3] = MAGIC3;
	put32(&hdr[4], highaddr - lowaddr + HDRSIZE);
	put32(&hdr[8], 0);
	put32(&hdr[12], 0);
	put32(&hdr[20], 0);

	if(fwrite((char *)hdr, 1, HDRSIZE, out) != HDRSIZE ||
	    writeimage(out, highaddr - lowaddr) < 0) {
		fprintf(stderr, "srec2aout: write error\n");
		exit(1);
	}
	fclose(out);
	chmod(outname, 0755);
	exit(0);
}

static int
writeimage(FILE *out, long size)
{
	int bank;
	int count;

	bank = 0;
	while(size > 0) {
		count = size > IMAGE_CHUNK ? IMAGE_CHUNK : size;
		if(fwrite((char *)image[bank], 1, count, out) != count)
			return(-1);
		size -= count;
		bank++;
	}
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: srec2aout input.h68 output\n");
	exit(1);
}

static void
put32(unsigned char *p, long v)
{
	p[0] = (v >> 24) & 0xff;
	p[1] = (v >> 16) & 0xff;
	p[2] = (v >> 8) & 0xff;
	p[3] = v & 0xff;
}

static long
hexval(char *p, long n)
{
	long v;
	int b;

	v = 0;
	while(n-- > 0) {
		b = hexbyte(p);
		if(b < 0)
			return(-1);
		v = (v << 8) | b;
		p += 2;
	}
	return(v);
}

static int
hexbyte(char *p)
{
	int hi, lo;

	hi = hexnib(p[0]);
	lo = hexnib(p[1]);
	if(hi < 0 || lo < 0)
		return(-1);
	return((hi << 4) | lo);
}

static int
hexnib(int c)
{
	if(c >= '0' && c <= '9')
		return(c - '0');
	if(c >= 'a' && c <= 'f')
		return(c - 'a' + 10);
	if(c >= 'A' && c <= 'F')
		return(c - 'A' + 10);
	return(-1);
}
