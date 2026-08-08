#include "../h/param.h"
#include "../h/dir.h"
#include "../h/user.h"

#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_MIRROR2_PAGE_REG 2
#define EPOCH68_RAM_WINDOW ((volatile unsigned char *)0x00400000)
#define EPOCH68_RAM_PAGE_SIZE (256UL * 1024UL)
#define EPOCH68_RAM_PAGES 64UL
#define EPOCH68_RAM_SIZE (EPOCH68_RAM_PAGE_SIZE * EPOCH68_RAM_PAGES)
#define EPOCH68_KMETA_BASE 0x00c00000UL
#define EPOCH68_KMETA_PAGE 63

i32 passc(i32 c);
i32 spl7(void);
void splx(i32 s);

static i32
epoch68_page_byte(unsigned char page, u32 off)
{
	register volatile unsigned char *mmio;
	register unsigned char oldpage, value;
	register i32 s;

	mmio = EPOCH68_MMIO_BASE;
	s = spl7();
	oldpage = mmio[EPOCH68_MIRROR2_PAGE_REG];
	mmio[EPOCH68_MIRROR2_PAGE_REG] = page;
	value = EPOCH68_RAM_WINDOW[off];
	mmio[EPOCH68_MIRROR2_PAGE_REG] = oldpage;
	splx(s);
	return((i32)value);
}

static i32
epoch68_ram_byte(u32 addr)
{
	return(epoch68_page_byte((unsigned char)(addr / EPOCH68_RAM_PAGE_SIZE),
	    addr % EPOCH68_RAM_PAGE_SIZE));
}

static i32
epoch68_kmem_byte(u32 addr)
{
	if (addr < EPOCH68_RAM_PAGE_SIZE)
		return(epoch68_page_byte(0, addr));
	if (addr >= EPOCH68_KMETA_BASE &&
	    addr < EPOCH68_KMETA_BASE + EPOCH68_RAM_PAGE_SIZE)
		return(epoch68_page_byte(EPOCH68_KMETA_PAGE,
		    addr - EPOCH68_KMETA_BASE));
	return(-1);
}

i32
mmread(i32 dev)
{
	register i32 c;
	register i32 m;
	register u32 addr;

	m = minor(dev);
	if (m == 2)
		return(0);
	if (m != 0 && m != 1) {
		u.u_error = ENXIO;
		return(-1);
	}
	while (u.u_count != 0) {
			addr = (u32)u.u_offset;
		if (m == 0) {
			if (addr >= EPOCH68_RAM_SIZE)
				break;
			c = epoch68_ram_byte(addr);
		} else {
			c = epoch68_kmem_byte(addr);
			if (c < 0) {
				u.u_error = ENXIO;
				break;
			}
		}
		if (passc(c) < 0)
			break;
	}
	return(0);
}

i32
mmwrite(i32 dev)
{
	if (minor(dev) == 2) {
		u.u_count = 0;
		return(0);
	}
	u.u_error = EACCES;
	return(-1);
}
