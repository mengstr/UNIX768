#include "../h/param.h"
#include "../h/systm.h"
#include "../h/dir.h"
#include "../h/user.h"
#include "../h/proc.h"

#define EPOCH68_USER_BASE 0x00200000L
#define EPOCH68_USER_PAGE_SIZE (256UL * 1024UL)
#define EPOCH68_CLICK_SIZE 64UL
#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_MIRROR1_PAGE_REG 1

static u32 epoch68_user_start(caddr_t addr);
static i32 epoch68_user_range_ok(caddr_t addr, u32 count);
static unsigned char epoch68_user_page(void);
static volatile unsigned char *epoch68_user_ptr(caddr_t addr, u32 count);
static void epoch68_copy_bytes(unsigned char *dst,
    volatile unsigned char *src, u32 count);
static void epoch68_copyout_bytes(volatile unsigned char *dst,
    unsigned char *src, u32 count);

static u32
epoch68_user_start(addr)
caddr_t addr;
{
	return((u32)addr);
}

static i32
epoch68_user_range_ok(addr, count)
caddr_t addr;
u32 count;
{
	u32 start;
	u32 span;
	u32 end;

	start = (u32)epoch68_user_start(addr);
	span = (u32)count;
	end = start + span;
	if (start >= EPOCH68_USER_PAGE_SIZE) {
		return(0);
	}
	if (end < start || end > (u32)EPOCH68_USER_PAGE_SIZE) {
		return(0);
	}
	return(1);
}

static unsigned char
epoch68_user_page()
{
	if (u.u_procp == 0)
		return(0);
	return((unsigned char)u.u_procp->p_addr);
}

static volatile unsigned char *
epoch68_user_ptr(addr, count)
caddr_t addr;
u32 count;
{
	if (!epoch68_user_range_ok(addr, count))
		return((volatile unsigned char *)0);
	EPOCH68_MMIO_BASE[EPOCH68_MIRROR1_PAGE_REG] = epoch68_user_page();
	return((volatile unsigned char *)(EPOCH68_USER_BASE + epoch68_user_start(addr)));
}

static void
epoch68_copy_bytes(dst, src, count)
register unsigned char *dst;
register volatile unsigned char *src;
register u32 count;
{
	while (count-- > 0)
		*dst++ = *src++;
}

static void
epoch68_copyout_bytes(dst, src, count)
register volatile unsigned char *dst;
register unsigned char *src;
register u32 count;
{
	while (count-- > 0)
		*dst++ = *src++;
}

i32
copyin(from, to, count)
caddr_t from;
caddr_t to;
i32 count;
{
	register volatile unsigned char *src;

	src = epoch68_user_ptr(from, (u32)count);
	if (src == 0)
		return(-1);
	epoch68_copy_bytes((unsigned char *)to, src, (u32)count);
	return(0);
}

i32
copyout(from, to, count)
caddr_t from;
caddr_t to;
i32 count;
{
	register volatile unsigned char *dst;

	dst = epoch68_user_ptr(to, (u32)count);
	if (dst == 0)
		return(-1);
	epoch68_copyout_bytes(dst, (unsigned char *)from, (u32)count);
	return(0);
}

i32
copyiin(from, to, count)
caddr_t from;
caddr_t to;
i32 count;
{
	return(copyin(from, to, count));
}

i32
copyiout(from, to, count)
caddr_t from;
caddr_t to;
i32 count;
{
	return(copyout(from, to, count));
}

i32
fubyte(addr)
caddr_t addr;
{
	register volatile unsigned char *src;

	src = epoch68_user_ptr(addr, 1);
	if (src == 0)
		return(-1);
	return((i32)*src);
}

i32
fuibyte(addr)
caddr_t addr;
{
	return(fubyte(addr));
}

i32
fuword(addr)
caddr_t addr;
{
	register volatile unsigned char *src;
	i32 value;

	src = epoch68_user_ptr(addr, 2);
	if (src == 0)
		return(-1);
	value = ((i32)src[0] << 8) | (i32)src[1];
	if (value & 0x8000)
		value |= ~0xffff;
	return(value);
}

i32
fuiword(addr)
caddr_t addr;
{
	return(fuword(addr));
}

i32
subyte(addr, value)
caddr_t addr;
i32 value;
{
	register volatile unsigned char *dst;

	dst = epoch68_user_ptr(addr, 1);
	if (dst == 0)
		return(-1);
	*dst = (unsigned char)value;
	return(0);
}

i32
suibyte(addr, value)
caddr_t addr;
i32 value;
{
	return(subyte(addr, value));
}

i32
suword(addr, value)
caddr_t addr;
i32 value;
{
	register volatile unsigned char *dst;

	dst = epoch68_user_ptr(addr, 2);
	if (dst == 0)
		return(-1);
	dst[0] = (unsigned char)((value >> 8) & 0xff);
	dst[1] = (unsigned char)(value & 0xff);
	return(0);
}

i32
suiword(addr, value)
caddr_t addr;
i32 value;
{
	return(suword(addr, value));
}

void
clearseg(click)
i32 click;
{
	register volatile unsigned char *dst;
	register u32 count;

	dst = epoch68_user_ptr((caddr_t)((u32)click * EPOCH68_CLICK_SIZE),
	    EPOCH68_CLICK_SIZE);
	if (dst == 0)
		return;
	for (count = 0; count < EPOCH68_CLICK_SIZE; count++)
		dst[count] = 0;
}

void
copyseg(src_click, dst_click)
i32 src_click;
i32 dst_click;
{
	register volatile unsigned char *src;
	register volatile unsigned char *dst;

	src = epoch68_user_ptr((caddr_t)((u32)src_click * EPOCH68_CLICK_SIZE),
	    EPOCH68_CLICK_SIZE);
	dst = epoch68_user_ptr((caddr_t)((u32)dst_click * EPOCH68_CLICK_SIZE),
	    EPOCH68_CLICK_SIZE);
	if (src == 0 || dst == 0)
		return;
	epoch68_copyout_bytes(dst, (unsigned char *)src, EPOCH68_CLICK_SIZE);
}
