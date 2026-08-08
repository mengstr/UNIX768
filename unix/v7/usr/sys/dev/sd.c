/*
 * Epoch68 serial disk driver.
 *
 * The transport uses UART1, shared with the ROM/YMODEM side channel.  The
 * host-side ysend --auto server recognizes these frames independently of
 * YMODEM file requests.
 */

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"

#define UART_BASE ((volatile unsigned char *)0x00a00100)
#define UART_STRIDE 8
#define UART_CHANNEL 1
#define UART_RBR_THR_DLL 0
#define UART_LSR 5
#define UART_LSR_DR 0x01
#define UART_LSR_THRE 0x20

#define SD_SECTOR_SIZE 512
#define SD_NSECTORS 2880
#define SD_TIMEOUT 2000000L

#define SD_MAGIC0 0xe7
#define SD_MAGIC1 0x68
#define SD_MAGIC2_REQ 0x53
#define SD_MAGIC2_RESP 0x73
#define SD_MAGIC3 0x44
#define SD_VERSION 1
#define SD_OP_READ 'R'
#define SD_OP_WRITE 'W'
#define SD_OP_INFO 'I'
#define SD_STATUS_OK 0

struct	buf sdtab;

i32 sdopen(i32 dev, i32 flag);
i32 sdstrategy(struct buf *bp);
static volatile unsigned char *sduart(void);
static unsigned short sdcrc_update(i32 crc, i32 ch);
static void sdputc(i32 c);
static i32 sdgetc(void);
static void sdput16(i32 v);
static void sdput32(u32 v);
static i32 sdrequest(i32 op, u32 lba, char *data);

static volatile unsigned char *
sduart(void)
{
	return(UART_BASE + (UART_CHANNEL * UART_STRIDE));
}

static unsigned short
sdcrc_update(crc, ch)
i32 crc;
i32 ch;
{
	register i16 i;

	crc ^= ((unsigned short)ch) << 8;
	for (i = 0; i < 8; i++)
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc <<= 1;
	return(crc);
}

static void
sdputc(c)
i32 c;
{
	register volatile unsigned char *uart;
	register i32 t;

	uart = sduart();
	for (t = SD_TIMEOUT; t > 0 && (uart[UART_LSR] & UART_LSR_THRE) == 0; t--)
		;
	uart[UART_RBR_THR_DLL] = (unsigned char)c;
}

static i32
sdgetc(void)
{
	register volatile unsigned char *uart;
	register i32 t;

	uart = sduart();
	for (t = SD_TIMEOUT; t > 0; t--)
		if (uart[UART_LSR] & UART_LSR_DR)
			return(uart[UART_RBR_THR_DLL] & 0xff);
	return(-1);
}

static void
sdput16(v)
i32 v;
{
	sdputc(v >> 8);
	sdputc(v);
}

static void
sdput32(v)
	u32 v;
{
	sdputc((i16)(v >> 24));
	sdputc((i16)(v >> 16));
	sdputc((i16)(v >> 8));
	sdputc((i16)v);
}

static i32
sdrequest(op, lba, data)
i32 op;
u32 lba;
char *data;
{
	unsigned char h[9];
	unsigned char rh[4];
	unsigned short crc, want;
	register i16 i;
	register i32 c;
	i16 status;
	i16 count;

	h[0] = SD_VERSION;
	h[1] = op;
	h[2] = 0;
	h[3] = (unsigned char)(lba >> 24);
	h[4] = (unsigned char)(lba >> 16);
	h[5] = (unsigned char)(lba >> 8);
	h[6] = (unsigned char)lba;
	h[7] = 0;
	h[8] = 1;

	crc = 0;
	for (i = 0; i < sizeof(h); i++)
		crc = sdcrc_update(crc, h[i]);

	sdputc(SD_MAGIC0);
	sdputc(SD_MAGIC1);
	sdputc(SD_MAGIC2_REQ);
	sdputc(SD_MAGIC3);
	for (i = 0; i < sizeof(h); i++)
		sdputc(h[i]);
	sdput16(crc);

	if (op == SD_OP_WRITE) {
		crc = 0;
		for (i = 0; i < SD_SECTOR_SIZE; i++) {
			c = data[i] & 0xff;
			crc = sdcrc_update(crc, c);
			sdputc(c);
		}
		sdput16(crc);
	}

	if (sdgetc() != SD_MAGIC0 || sdgetc() != SD_MAGIC1 ||
	    sdgetc() != SD_MAGIC2_RESP || sdgetc() != SD_MAGIC3)
		return(-1);
	for (i = 0; i < sizeof(rh); i++) {
		c = sdgetc();
		if (c < 0)
			return(-1);
		rh[i] = c;
	}
	if (rh[0] != SD_VERSION)
		return(-1);
	status = rh[1];
	count = (((i16)rh[2]) << 8) | rh[3];

	crc = 0;
	for (i = 0; i < sizeof(rh); i++)
		crc = sdcrc_update(crc, rh[i]);
	if (op == SD_OP_READ && status == SD_STATUS_OK) {
		if (count != 1)
			return(-1);
		for (i = 0; i < SD_SECTOR_SIZE; i++) {
			c = sdgetc();
			if (c < 0)
				return(-1);
			data[i] = c;
			crc = sdcrc_update(crc, c);
		}
	}
	c = sdgetc();
	if (c < 0)
		return(-1);
	want = ((unsigned short)c) << 8;
	c = sdgetc();
	if (c < 0)
		return(-1);
	want |= c;
	if (crc != want)
		return(-1);
	if (status != SD_STATUS_OK)
		return(-1);
	return(0);
}

i32
sdopen(dev, flag)
i32 dev;
i32 flag;
{
	if (minor(dev) != 0) {
		u.u_error = ENXIO;
		return(-1);
	}
	return(0);
}

i32
sdstrategy(bp)
struct buf *bp;
{
	if (minor(bp->b_dev) != 0 || bp->b_blkno < 0 || bp->b_blkno >= SD_NSECTORS ||
	    bp->b_bcount != SD_SECTOR_SIZE) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return(0);
	}
	if (bp->b_flags & B_READ) {
		if (sdrequest(SD_OP_READ, (u32)bp->b_blkno, bp->b_un.b_addr) < 0)
			bp->b_flags |= B_ERROR;
	} else {
		if (sdrequest(SD_OP_WRITE, (u32)bp->b_blkno, bp->b_un.b_addr) < 0)
			bp->b_flags |= B_ERROR;
	}
	bp->b_resid = 0;
	iodone(bp);
	return(0);
}
