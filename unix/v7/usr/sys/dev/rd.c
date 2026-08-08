/*
 * Epoch68 ramdisk driver
 */

#include "../h/param.h"
#include "../h/systm.h"
#include "../h/buf.h"
#include "../h/conf.h"
#include "../h/dir.h"
#include "../h/user.h"

#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_DISK_WINDOW ((volatile unsigned char *)0x00600000)
#define EPOCH68_DISK_PAGE_REG 3
#define EPOCH68_DISK_PAGE_SIZE (256UL * 1024UL)

struct rdpart {
	daddr_t	start;
	daddr_t	size;
};

static struct rdpart rdparts[] = {
	{ 0, 32768 },		/* full 16 MB V7 disk image */
};

struct	buf	rdtab;
struct	buf	rrdbuf;

static
i32 rdvalid(i32 minor)
{
	return(minor >= 0 && minor < (sizeof(rdparts) / sizeof(rdparts[0])));
}

static
void rdxfer(register struct buf *bp)
{
	register volatile unsigned char *mmio;
	register volatile unsigned char *window;
	u32 disk_offset;
	u32 page;
	u32 page_offset;
	register unsigned char *cp;
	register u16 count;
	register i32 partno;
	daddr_t physbn;

	partno = minor(bp->b_dev);
	if (!rdvalid(partno) || bp->b_blkno < 0 || bp->b_blkno >= rdparts[partno].size) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}

	physbn = rdparts[partno].start + bp->b_blkno;
	disk_offset = ((u32)physbn * 512UL);
	page = disk_offset / EPOCH68_DISK_PAGE_SIZE;
	page_offset = disk_offset % EPOCH68_DISK_PAGE_SIZE;
	mmio = EPOCH68_MMIO_BASE;
	window = EPOCH68_DISK_WINDOW;
	mmio[EPOCH68_DISK_PAGE_REG] = (unsigned char)page;

	cp = (unsigned char *)bp->b_un.b_addr;
	count = bp->b_bcount;
	while (count-- > 0) {
		if (bp->b_flags & B_READ)
			*cp++ = window[page_offset++];
		else
			window[page_offset++] = *cp++;
		if (page_offset == EPOCH68_DISK_PAGE_SIZE && count > 0) {
			page++;
			page_offset = 0;
			mmio[EPOCH68_DISK_PAGE_REG] = (unsigned char)page;
		}
	}

	bp->b_resid = 0;
	iodone(bp);
}

i32
rdopen(i32 dev, i32 flag)
{
	if (!rdvalid(minor(dev))) {
		u.u_error = ENXIO;
		return(-1);
	}
	return(0);
}

i32
rdstrategy(struct buf *bp)
{
	rdxfer(bp);
	return(0);
}

i32
rdread(i32 dev)
{
	physio(rdstrategy, &rrdbuf, dev, B_READ);
	return(0);
}

i32
rdwrite(i32 dev)
{
	physio(rdstrategy, &rrdbuf, dev, B_WRITE);
	return(0);
}
