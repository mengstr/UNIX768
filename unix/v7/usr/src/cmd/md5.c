/*
 * Local MD5 checksum command for Epoch68 V7.
 *
 * The command name is `md5`, but it prints output in the common
 * `md5sum`-style format:
 *
 * 	<32-hex-digest><space><space><filename>
 *
 * With no file arguments, or with `-`, it hashes standard input and
 * prints `-` as the file name.
 */

#include <stdio.h>
#include <string.h>
#include <sys/inttypes.h>

typedef unsigned char md5_u8;
typedef unsigned long md5_u32;

struct md5_ctx {
	md5_u32 state[4];
	md5_u32 count_lo;
	md5_u32 count_hi;
	md5_u8 buffer[64];
};

static void md5_init(struct md5_ctx *ctx);
static void md5_update(struct md5_ctx *ctx, md5_u8 *input, unsigned len);
static void md5_final(struct md5_ctx *ctx, md5_u8 digest[16]);
static void md5_transform(md5_u32 state[4], md5_u8 block[64]);
static void store32(md5_u8 *dst, md5_u32 value);
static void digest_hex(md5_u8 digest[16], char hex[33]);
static int hash_stream(FILE *fp, md5_u8 digest[16]);
static int digest_path(char *name);
static md5_u32 add32(md5_u32 a, md5_u32 b);
static md5_u32 rotate_left(md5_u32 value, int shift);
static md5_u32 md5_step(int round, md5_u32 a, md5_u32 b, md5_u32 c,
	md5_u32 d, md5_u32 x, md5_u32 t, int shift);

int
main(int argc, char **argv)
{
	i32 i;
	i32 errors;

	errors = 0;
	if(argc == (i32)1)
		return((i32)(digest_path((char *)0) != 0));
	for(i = (i32)1; i < argc; i++)
		if(digest_path(argv[i]) != 0)
			errors = (i32)1;
	fflush(stdout);
	return(errors);
}

static int
digest_path(char *name)
{
	FILE *fp;
	md5_u8 digest[16];
	char hex[33];
	char *label;

	if(name == (char *)0 || (name[0] == '-' && name[1] == '\0')) {
		fp = stdin;
		label = "-";
	} else {
		fp = fopen(name, "r");
		if(fp == NULL) {
			fprintf(stderr, "md5: can't open %s\n", name);
			return(-1);
		}
		label = name;
	}

	if(hash_stream(fp, digest) != 0) {
		if(fp != stdin)
			fclose(fp);
		fprintf(stderr, "md5: read error on %s\n", label);
		return(-1);
	}
	if(fp != stdin && fclose(fp) == EOF) {
		fprintf(stderr, "md5: close error on %s\n", label);
		return(-1);
	}

	digest_hex(digest, hex);
	printf("%s  %s\n", hex, label);
	fflush(stdout);
	return(0);
}

static int
hash_stream(FILE *fp, md5_u8 digest[16])
{
	struct md5_ctx ctx;
	md5_u8 data[1024];
	int n;

	md5_init(&ctx);
	while((n = fread((char *)data, 1, sizeof(data), fp)) > 0)
		md5_update(&ctx, data, (unsigned)n);
	if(ferror(fp))
		return(-1);
	md5_final(&ctx, digest);
	return(0);
}

static md5_u32
add32(md5_u32 a, md5_u32 b)
{
	return((a + b) & 0xffffffffUL);
}

static void
store32(md5_u8 *dst, md5_u32 value)
{
	dst[0] = (md5_u8)(value & 0xffUL);
	dst[1] = (md5_u8)((value >> 8) & 0xffUL);
	dst[2] = (md5_u8)((value >> 16) & 0xffUL);
	dst[3] = (md5_u8)((value >> 24) & 0xffUL);
}

static void
md5_init(struct md5_ctx *ctx)
{
	ctx->count_lo = 0;
	ctx->count_hi = 0;
	ctx->state[0] = 0x67452301UL;
	ctx->state[1] = 0xefcdab89UL;
	ctx->state[2] = 0x98badcfeUL;
	ctx->state[3] = 0x10325476UL;
}

static void
md5_update(struct md5_ctx *ctx, md5_u8 *input, unsigned len)
{
	unsigned index;
	unsigned part_len;
	unsigned i;
	md5_u32 old_lo;

	old_lo = ctx->count_lo;
	index = (unsigned)((old_lo >> 3) & 0x3fUL);
	ctx->count_lo = add32(ctx->count_lo, ((md5_u32)len << 3));
	if(ctx->count_lo < old_lo)
		ctx->count_hi = add32(ctx->count_hi, 1);
	ctx->count_hi = add32(ctx->count_hi, ((md5_u32)len >> 29));
	part_len = 64 - index;
	if(len >= part_len) {
		memcpy((char *)ctx->buffer + index, (char *)input, part_len);
		md5_transform(ctx->state, ctx->buffer);
		for(i = part_len; i + 63 < len; i += 64)
			md5_transform(ctx->state, input + i);
		index = 0;
	} else {
		i = 0;
	}
	memcpy((char *)ctx->buffer + index, (char *)input + i, len - i);
}

static void
md5_final(struct md5_ctx *ctx, md5_u8 digest[16])
{
	static md5_u8 padding[64] = { 0x80 };
	md5_u8 bits[8];
	unsigned index;
	unsigned pad_len;

	store32(bits, ctx->count_lo);
	store32(bits + 4, ctx->count_hi);
	index = (unsigned)((ctx->count_lo >> 3) & 0x3fUL);
	pad_len = (index < 56) ? (56 - index) : (120 - index);
	md5_update(ctx, padding, pad_len);
	md5_update(ctx, bits, 8);
	store32(digest, ctx->state[0]);
	store32(digest + 4, ctx->state[1]);
	store32(digest + 8, ctx->state[2]);
	store32(digest + 12, ctx->state[3]);
	memset((char *)ctx, 0, sizeof(*ctx));
}

static void
md5_transform(md5_u32 state[4], md5_u8 block[64])
{
	md5_u32 a;
	md5_u32 b;
	md5_u32 c;
	md5_u32 d;
	md5_u32 x[16];
	int i;
	int j;

	for(i = 0, j = 0; i < 16; i++, j += 4)
		x[i] = (md5_u32)block[j] |
		    ((md5_u32)block[j + 1] << 8) |
		    ((md5_u32)block[j + 2] << 16) |
		    ((md5_u32)block[j + 3] << 24);

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];

	a = md5_step(0, a, b, c, d, x[ 0], 0xd76aa478UL,  7);
	d = md5_step(0, d, a, b, c, x[ 1], 0xe8c7b756UL, 12);
	c = md5_step(0, c, d, a, b, x[ 2], 0x242070dbUL, 17);
	b = md5_step(0, b, c, d, a, x[ 3], 0xc1bdceeeUL, 22);
	a = md5_step(0, a, b, c, d, x[ 4], 0xf57c0fafUL,  7);
	d = md5_step(0, d, a, b, c, x[ 5], 0x4787c62aUL, 12);
	c = md5_step(0, c, d, a, b, x[ 6], 0xa8304613UL, 17);
	b = md5_step(0, b, c, d, a, x[ 7], 0xfd469501UL, 22);
	a = md5_step(0, a, b, c, d, x[ 8], 0x698098d8UL,  7);
	d = md5_step(0, d, a, b, c, x[ 9], 0x8b44f7afUL, 12);
	c = md5_step(0, c, d, a, b, x[10], 0xffff5bb1UL, 17);
	b = md5_step(0, b, c, d, a, x[11], 0x895cd7beUL, 22);
	a = md5_step(0, a, b, c, d, x[12], 0x6b901122UL,  7);
	d = md5_step(0, d, a, b, c, x[13], 0xfd987193UL, 12);
	c = md5_step(0, c, d, a, b, x[14], 0xa679438eUL, 17);
	b = md5_step(0, b, c, d, a, x[15], 0x49b40821UL, 22);

	a = md5_step(1, a, b, c, d, x[ 1], 0xf61e2562UL,  5);
	d = md5_step(1, d, a, b, c, x[ 6], 0xc040b340UL,  9);
	c = md5_step(1, c, d, a, b, x[11], 0x265e5a51UL, 14);
	b = md5_step(1, b, c, d, a, x[ 0], 0xe9b6c7aaUL, 20);
	a = md5_step(1, a, b, c, d, x[ 5], 0xd62f105dUL,  5);
	d = md5_step(1, d, a, b, c, x[10], 0x02441453UL,  9);
	c = md5_step(1, c, d, a, b, x[15], 0xd8a1e681UL, 14);
	b = md5_step(1, b, c, d, a, x[ 4], 0xe7d3fbc8UL, 20);
	a = md5_step(1, a, b, c, d, x[ 9], 0x21e1cde6UL,  5);
	d = md5_step(1, d, a, b, c, x[14], 0xc33707d6UL,  9);
	c = md5_step(1, c, d, a, b, x[ 3], 0xf4d50d87UL, 14);
	b = md5_step(1, b, c, d, a, x[ 8], 0x455a14edUL, 20);
	a = md5_step(1, a, b, c, d, x[13], 0xa9e3e905UL,  5);
	d = md5_step(1, d, a, b, c, x[ 2], 0xfcefa3f8UL,  9);
	c = md5_step(1, c, d, a, b, x[ 7], 0x676f02d9UL, 14);
	b = md5_step(1, b, c, d, a, x[12], 0x8d2a4c8aUL, 20);

	a = md5_step(2, a, b, c, d, x[ 5], 0xfffa3942UL,  4);
	d = md5_step(2, d, a, b, c, x[ 8], 0x8771f681UL, 11);
	c = md5_step(2, c, d, a, b, x[11], 0x6d9d6122UL, 16);
	b = md5_step(2, b, c, d, a, x[14], 0xfde5380cUL, 23);
	a = md5_step(2, a, b, c, d, x[ 1], 0xa4beea44UL,  4);
	d = md5_step(2, d, a, b, c, x[ 4], 0x4bdecfa9UL, 11);
	c = md5_step(2, c, d, a, b, x[ 7], 0xf6bb4b60UL, 16);
	b = md5_step(2, b, c, d, a, x[10], 0xbebfbc70UL, 23);
	a = md5_step(2, a, b, c, d, x[13], 0x289b7ec6UL,  4);
	d = md5_step(2, d, a, b, c, x[ 0], 0xeaa127faUL, 11);
	c = md5_step(2, c, d, a, b, x[ 3], 0xd4ef3085UL, 16);
	b = md5_step(2, b, c, d, a, x[ 6], 0x04881d05UL, 23);
	a = md5_step(2, a, b, c, d, x[ 9], 0xd9d4d039UL,  4);
	d = md5_step(2, d, a, b, c, x[12], 0xe6db99e5UL, 11);
	c = md5_step(2, c, d, a, b, x[15], 0x1fa27cf8UL, 16);
	b = md5_step(2, b, c, d, a, x[ 2], 0xc4ac5665UL, 23);

	a = md5_step(3, a, b, c, d, x[ 0], 0xf4292244UL,  6);
	d = md5_step(3, d, a, b, c, x[ 7], 0x432aff97UL, 10);
	c = md5_step(3, c, d, a, b, x[14], 0xab9423a7UL, 15);
	b = md5_step(3, b, c, d, a, x[ 5], 0xfc93a039UL, 21);
	a = md5_step(3, a, b, c, d, x[12], 0x655b59c3UL,  6);
	d = md5_step(3, d, a, b, c, x[ 3], 0x8f0ccc92UL, 10);
	c = md5_step(3, c, d, a, b, x[10], 0xffeff47dUL, 15);
	b = md5_step(3, b, c, d, a, x[ 1], 0x85845dd1UL, 21);
	a = md5_step(3, a, b, c, d, x[ 8], 0x6fa87e4fUL,  6);
	d = md5_step(3, d, a, b, c, x[15], 0xfe2ce6e0UL, 10);
	c = md5_step(3, c, d, a, b, x[ 6], 0xa3014314UL, 15);
	b = md5_step(3, b, c, d, a, x[13], 0x4e0811a1UL, 21);
	a = md5_step(3, a, b, c, d, x[ 4], 0xf7537e82UL,  6);
	d = md5_step(3, d, a, b, c, x[11], 0xbd3af235UL, 10);
	c = md5_step(3, c, d, a, b, x[ 2], 0x2ad7d2bbUL, 15);
	b = md5_step(3, b, c, d, a, x[ 9], 0xeb86d391UL, 21);

	state[0] = add32(state[0], a);
	state[1] = add32(state[1], b);
	state[2] = add32(state[2], c);
	state[3] = add32(state[3], d);
	memset((char *)x, 0, sizeof(x));
}

static md5_u32
rotate_left(md5_u32 value, int shift)
{
	return(((value << shift) | (value >> (32 - shift))) & 0xffffffffUL);
}

static md5_u32
md5_step(int round, md5_u32 a, md5_u32 b, md5_u32 c, md5_u32 d,
	md5_u32 x, md5_u32 t, int shift)
{
	md5_u32 f;

	if(round == 0)
		f = (b & c) | ((~b) & d);
	else if(round == 1)
		f = (b & d) | (c & (~d));
	else if(round == 2)
		f = b ^ c ^ d;
	else
		f = c ^ (b | (~d));
	a = add32(a, f);
	a = add32(a, x);
	a = add32(a, t);
	a = rotate_left(a, shift);
	return(add32(a, b));
}

static void
digest_hex(md5_u8 digest[16], char hex[33])
{
	static char digits[] = "0123456789abcdef";
	int i;

	for(i = 0; i < 16; i++) {
		hex[i * 2] = digits[(digest[i] >> 4) & 0x0f];
		hex[i * 2 + 1] = digits[digest[i] & 0x0f];
	}
	hex[32] = '\0';
}
