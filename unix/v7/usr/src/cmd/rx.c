/*
 * Receive one YMODEM file from the Epoch68 host-side ysend helper.
 *
 * Usage:
 *	rx /path/in/build/ramdisk/file
 *
 * The request path is sent to ysend as "YSEND <path>\n" over /dev/ymodem.
 * The received YMODEM metadata basename is used as the destination in the
 * current directory and is overwritten with creat(2).
 */

#include <string.h>
#include <unistd.h>

#define SOH	001
#define STX	002
#define EOT	004
#define ACK	006
#define NAK	025
#define CAN	030
#define CRCREQ	'C'

#define BLK128	128
#define BLK1K	1024
#define DIRSIZ	14

unsigned char packet[BLK1K];
int yfd;

char *basename(char *path);
i32 badname(char *name);
unsigned short crc16(unsigned char *data, i32 len);
unsigned short crc16_update(unsigned short crc, i32 ch);
i32 metadata_parse(unsigned char *data, i32 length, char *name, i32 namesz, unsigned long *sizep);
i32 packet_read(unsigned char *data, unsigned char *blockp, i32 *lengthp);
unsigned long parse_decimal(unsigned char *s);
void putdec(unsigned long value);
void puterr(char *s);
void putint(i32 value);
void putstr(char *s);
void putymodem(char *s);
i32 read_full(unsigned char *buf, i32 len);
void sendc(i32 c);

int
main(int argc, char **argv)
{
	unsigned char block;
	unsigned long filesize;
	unsigned long remaining;
	int fd;
	i32 length;
	i32 chunk;
	i32 expected;
	i32 rc;
	char name[DIRSIZ + 1];

	if (argc != 2) {
		puterr("usage: rx host-path\n");
		exit(1);
	}

	yfd = open("/dev/ymodem", 2);
	if (yfd < 0) {
		puterr("rx: cannot open /dev/ymodem\n");
		exit(1);
	}

	putymodem("YSEND ");
	putymodem(argv[1]);
	putymodem("\n");

	fd = -1;
	expected = 1;
	block = 0;
	length = 0;

	sendc(CRCREQ);
	rc = packet_read(packet, &block, &length);
	if (rc == CAN) {
		puterr("rx: canceled\n");
		exit(1);
	}
	if (rc != 0 || block != 0) {
		sendc(CAN);
		puterr("rx: bad metadata rc=");
		putint(rc);
		puterr(" block=");
		putdec((unsigned long)block);
		puterr(" len=");
		putdec((unsigned long)length);
		puterr(" first=");
		putdec((unsigned long)packet[0]);
		puterr("\n");
		exit(1);
	}
	if (metadata_parse(packet, length, name, sizeof(name), &filesize) < 0) {
		sendc(CAN);
		puterr("rx: bad file name\n");
		exit(1);
	}
	if (badname(name) || strcmp(name, basename(argv[1])) != 0) {
		sendc(CAN);
		puterr("rx: wrong file\n");
		exit(1);
	}

	fd = creat(name, 0666);
	if (fd < 0) {
		sendc(CAN);
		puterr("rx: cannot create ");
		puterr(name);
		puterr("\n");
		exit(1);
	}

	remaining = filesize;
	sendc(ACK);
	sendc(CRCREQ);

	for (;;) {
		rc = packet_read(packet, &block, &length);
		if (rc == EOT) {
			sendc(ACK);
			sendc(CRCREQ);
			rc = packet_read(packet, &block, &length);
			if (rc != 0 || block != 0 || packet[0] != '\0' || remaining != 0) {
				sendc(CAN);
				close(fd);
				puterr("rx: bad trailer\n");
				exit(1);
			}
			sendc(ACK);
			close(fd);
			puterr("rx: received ");
			puterr(name);
			puterr(", ");
			putdec(filesize);
			puterr(" bytes\n");
			exit(0);
		}
		if (rc == CAN) {
			close(fd);
			puterr("rx: canceled\n");
			exit(1);
		}
		if (rc != 0) {
			sendc(NAK);
			continue;
		}
		if (block == ((expected - 1) & 0377)) {
			sendc(ACK);
			continue;
		}
		if (block != (expected & 0377)) {
			sendc(CAN);
			close(fd);
			puterr("rx: bad block\n");
			exit(1);
		}
		if (remaining == 0) {
			sendc(CAN);
			close(fd);
			puterr("rx: extra data\n");
			exit(1);
		}
		chunk = length;
		if ((unsigned long)chunk > remaining)
			chunk = (int)remaining;
		if (write(fd, packet, chunk) != chunk) {
			sendc(CAN);
			close(fd);
			puterr("rx: write failed\n");
			exit(1);
		}
		remaining -= (unsigned long)chunk;
		expected++;
		sendc(ACK);
	}
}

i32
getc1(void)
{
	unsigned char c;

	if (read(yfd, &c, 1) != 1)
		return(-1);
	return((int)c);
}

void
sendc(i32 c)
{
	unsigned char ch;

	ch = c;
	write(yfd, &ch, 1);
}

i32
packet_read(unsigned char *data, unsigned char *blockp, i32 *lengthp)
{
	unsigned char block;
	unsigned char block_inv;
	unsigned char crcbytes[2];
	unsigned short want;
	unsigned short got;
	int marker;
	int length;
	int i;

	marker = getc1();
	if (marker < 0)
		return(-4);
	if (marker == EOT || marker == CAN)
		return(marker);
	if (marker == SOH)
		length = BLK128;
	else if (marker == STX)
		length = BLK1K;
	else
		return(-1);

	if (read_full(&block, 1) != 1)
		return(-4);
	if (read_full(&block_inv, 1) != 1)
		return(-4);
	if (((block + block_inv) & 0377) != 0377) {
		for (i = 0; i < length + 2; i++)
			if (getc1() < 0)
				return(-4);
		return(-2);
	}

	if (read_full(data, length) != length)
		return(-4);
	if (read_full(crcbytes, 2) != 2)
		return(-4);
	want = (unsigned short)((unsigned short)crcbytes[0] << 8);
	want |= (unsigned short)crcbytes[1];
	got = crc16(data, length);
	*lengthp = length;
	if (got != want)
		return(-3);

	*blockp = block;
	return(0);
}

i32
read_full(unsigned char *buf, i32 len)
{
	int n;
	int done;

	done = 0;
	while (done < len) {
		n = read(yfd, &buf[done], len - done);
		if (n <= 0)
			return(done);
		done += n;
	}
	return(done);
}

i32
metadata_parse(unsigned char *data, i32 length, char *name, i32 namesz,
    unsigned long *sizep)
{
	int i;
	int n;
	unsigned char *size;

	if (data[0] == '\0')
		return(-1);
	n = strlen((char *)data);
	if (n <= 0 || n + 1 > namesz)
		return(-1);
	strcpy(name, (char *)data);
	size = 0;
	for (i = 0; i < length; i++) {
		if (data[i] == '\0') {
			if (i + 1 < length)
				size = &data[i + 1];
			break;
		}
	}
	if (size == 0)
		return(-1);
	*sizep = parse_decimal(size);
	return(0);
}

unsigned long
parse_decimal(s)
unsigned char *s;
{
	unsigned long value;

	value = 0;
	while (*s >= '0' && *s <= '9') {
		value = (value * 10) + (unsigned long)(*s - '0');
		s++;
	}
	return(value);
}

i32
badname(char *name)
{
	char *p;

	if (name == 0 || *name == '\0')
		return(1);
	if (strlen(name) > DIRSIZ)
		return(1);
	for (p = name; *p; p++)
		if (*p == '/' || *p == '(' || *p == ')')
			return(1);
	return(0);
}

char *
basename(path)
char *path;
{
	char *base;

	base = path;
	while (*path) {
		if (*path == '/')
			base = path + 1;
		path++;
	}
	return(base);
}

unsigned short
crc16_update(unsigned short crc, i32 ch)
{
	int i;

	crc ^= (unsigned short)((unsigned short)(ch & 0377) << 8);
	for (i = 0; i < 8; i++) {
		if (crc & 0100000)
			crc = (unsigned short)((crc << 1) ^ 010041);
		else
			crc <<= 1;
	}
	return(crc);
}

unsigned short
crc16(unsigned char *buf, i32 len)
{
	unsigned short crc;
	int i;

	crc = 0;
	for (i = 0; i < len; i++)
		crc = crc16_update(crc, buf[i]);
	return(crc);
}

void
putstr(s)
char *s;
{
	write(1, s, strlen(s));
}

void
putymodem(s)
char *s;
{
	write(yfd, s, strlen(s));
}

void
puterr(s)
char *s;
{
	write(2, s, strlen(s));
}

void
putdec(n)
unsigned long n;
{
	char buf[16];
	int i;

	i = sizeof(buf);
	buf[--i] = '\0';
	if (n == 0)
		buf[--i] = '0';
	while (n != 0) {
		buf[--i] = (n % 10) + '0';
		n /= 10;
	}
	puterr(&buf[i]);
}

void
putint(i32 n)
{
	if (n < 0) {
		puterr("-");
		putdec((unsigned long)(-n));
		return;
	}
	putdec((unsigned long)n);
}
