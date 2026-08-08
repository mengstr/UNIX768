/*
 * platform/epoch68/emulator standalone serial disk driver.
 *
 * This duplicates the small UART1 protocol used by the kernel sd driver, but
 * adapts it to the monitor's standalone iob strategy interface.
 */

#include <sys/param.h>
#include <sys/inode.h>
#include "uart.h"
#include "v7io.h"

i32 sdstrategy(struct iob *io, i32 func);

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
#define SD_STATUS_OK 0

static u16
sdcrc_update(u16 crc, u8 ch)
{
	register i32 i;

	crc ^= ((u16)ch) << 8;
	for (i = 0; i < 8; i++)
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc <<= 1;
	return(crc);
}

static void
sdputc(i32 c)
{
	register volatile u8 *uart;
	register i32 t;

	uart = (volatile u8 *)(u32)
	    (EPOCH68_UART0_BASE + (EPOCH68_UART1 * EPOCH68_UART_STRIDE));
	for (t = SD_TIMEOUT; t > 0 &&
	    (uart[EPOCH68_UART_LSR] & EPOCH68_UART_LSR_THRE) == 0; t--)
		;
	uart[EPOCH68_UART_RBR_THR_DLL] = (u8)c;
}

static i32
sdgetc(void)
{
	register volatile u8 *uart;
	register i32 t;

	uart = (volatile u8 *)(u32)
	    (EPOCH68_UART0_BASE + (EPOCH68_UART1 * EPOCH68_UART_STRIDE));
	for (t = SD_TIMEOUT; t > 0; t--)
		if (uart[EPOCH68_UART_LSR] & EPOCH68_UART_LSR_DR)
			return(uart[EPOCH68_UART_RBR_THR_DLL] & 0xff);
	return(-1);
}

static void
sdput16(u16 v)
{
	sdputc(v >> 8);
	sdputc(v);
}

static i32
sdrequest(i32 op, u32 lba, char *data)
{
	u8 h[9];
	u8 rh[4];
	u16 crc, want;
	register i32 i, c;
	i32 status;
	i32 count;

	h[0] = SD_VERSION;
	h[1] = op;
	h[2] = 0;
	h[3] = (u8)(lba >> 24);
	h[4] = (u8)(lba >> 16);
	h[5] = (u8)(lba >> 8);
	h[6] = (u8)lba;
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
	count = (((i32)rh[2]) << 8) | rh[3];

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
	want = ((u16)c) << 8;
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
sdstrategy(struct iob *io, i32 func)
{
	register i32 i;
	i32 sectors;
	u32 lba;
	char *p;

	if ((io->i_cc % SD_SECTOR_SIZE) != 0)
		return(-1);
	if (io->i_unit != 0)
		return(-1);
	sectors = io->i_cc / SD_SECTOR_SIZE;
	lba = (u32)io->i_bn;
	if (lba + sectors > SD_NSECTORS)
		return(-1);

	p = io->i_ma;
	for (i = 0; i < sectors; i++) {
		if (func == READ) {
			if (sdrequest(SD_OP_READ, lba + i, p) < 0)
				return(-1);
		} else {
			if (sdrequest(SD_OP_WRITE, lba + i, p) < 0)
				return(-1);
		}
		p += SD_SECTOR_SIZE;
	}
	return(io->i_cc);
}
