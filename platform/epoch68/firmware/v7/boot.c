#include "uart.h"
#include "../string.h"
#include "v7io.h"

#define AOUT_HDR_SIZE 32
#define AOUT_ZMAGIC 0413
#define AOUT_MID_SUN010 1
#define BOOT_LOAD_BASE 0x00001000L
#define USER_LOAD_BASE 0x00200000L
#define USER_PAGE_SIZE (256L * 1024L)
#define KERNEL_IMAGE_LIMIT 0x0003f000L
#define USER_IMAGE_LIMIT 0x0003f000L
#define USER_STACK_TOP 0x0003fffeL
#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_SYSTEM_PAGE_REG 0
#define EPOCH68_MIRROR1_PAGE_REG 1
#define EPOCH68_BOOT_FLAGS_REG 3
#define EPOCH68_TIMER_CONTROL_REG 0x20

static u32 get32(u8 *p);
static i32 getmid(u8 *p);
static i32 getmagic(u8 *p);
static char *boot_memset(char *dst, i32 value, u32 count);
static char *boot_memcpy(char *dst, u8 *src, u32 count);

void kernel_handoff(void *user_base, void *spare, u32 limit, void *entry);
void enter_user(void *entry, u32 stack_top, u32 arg0);

i32
boot(char *path, i32 flags)
{
	u8 hdr[512];
	char *dst;
	volatile u8 *mmio = EPOCH68_MMIO_BASE;
	u32 text;
	u32 data;
	u32 bss;
	u32 entry;
	u32 limit;
	u32 remain;
	u32 raw_text;
	i32 fd;
	i32 n;

	v7io_close_all();
	fd = open(path, 0);
	if (fd < 0)
		return(-1);
	n = read(fd, (char *)hdr, sizeof(hdr));
	if (n < AOUT_HDR_SIZE) {
		uart_puts("boot: short header\n");
		close(fd);
		return(-1);
	}
	if (getmid(hdr) != AOUT_MID_SUN010 || getmagic(hdr) != AOUT_ZMAGIC) {
		uart_puts("boot: bad image\n");
		close(fd);
		return(-1);
	}

	raw_text = get32(&hdr[4]);
	data = get32(&hdr[8]);
	bss = get32(&hdr[12]);
	entry = get32(&hdr[20]);
	if (raw_text < AOUT_HDR_SIZE ||
	    data > KERNEL_IMAGE_LIMIT - BOOT_LOAD_BASE ||
	    bss > KERNEL_IMAGE_LIMIT - BOOT_LOAD_BASE - data) {
		uart_puts("boot: image too large\n");
		close(fd);
		return(-1);
	}
	text = raw_text - AOUT_HDR_SIZE;
	if (text > KERNEL_IMAGE_LIMIT - BOOT_LOAD_BASE - data - bss) {
		uart_puts("boot: image too large\n");
		close(fd);
		return(-1);
	}
	limit = BOOT_LOAD_BASE + text + data + bss;
	if (entry < BOOT_LOAD_BASE || entry >= limit ||
	    get32(&hdr[16]) != 0 ||
	    get32(&hdr[24]) != 0 || get32(&hdr[28]) != 0) {
		uart_puts("boot: bad header\n");
		close(fd);
		return(-1);
	}

	mmio[EPOCH68_MIRROR1_PAGE_REG] = 1;
	boot_memset((char *)USER_LOAD_BASE, 0, USER_PAGE_SIZE);
	dst = (char *)(USER_LOAD_BASE + BOOT_LOAD_BASE);
	remain = text + data;
	if (n > AOUT_HDR_SIZE) {
		i32 first;

		first = n - AOUT_HDR_SIZE;
		if ((u32)first > remain)
			first = (i32)remain;
		boot_memcpy(dst, hdr + AOUT_HDR_SIZE, (u32)first);
		dst += first;
		remain -= first;
	}
	while (remain > 0) {
		n = read(fd, (char *)hdr, sizeof(hdr));
		if (n <= 0) {
			uart_puts("boot: short image\n");
			close(fd);
			return(-1);
		}
		if ((u32)n > remain)
			n = (i32)remain;
		boot_memcpy(dst, hdr, (u32)n);
		dst += n;
		remain -= n;
	}
	close(fd);
	mmio[EPOCH68_MIRROR1_PAGE_REG] = 1;
	mmio[EPOCH68_BOOT_FLAGS_REG] = (u8)flags;
	mmio[EPOCH68_TIMER_CONTROL_REG] = 0x00;
	mmio[EPOCH68_UART0_BASE + EPOCH68_UART_IER_DLM] = 0x00;
	mmio[EPOCH68_UART0_BASE + EPOCH68_UART_STRIDE + EPOCH68_UART_IER_DLM] = 0x00;
	mmio[EPOCH68_UART0_BASE + (2 * EPOCH68_UART_STRIDE) + EPOCH68_UART_IER_DLM] = 0x00;
	mmio[EPOCH68_UART0_BASE + (3 * EPOCH68_UART_STRIDE) + EPOCH68_UART_IER_DLM] = 0x00;
	mmio[EPOCH68_UART0_BASE + (4 * EPOCH68_UART_STRIDE) + EPOCH68_UART_IER_DLM] = 0x00;
	mmio[EPOCH68_UART0_BASE + (5 * EPOCH68_UART_STRIDE) + EPOCH68_UART_IER_DLM] = 0x00;
	kernel_handoff((void *)USER_LOAD_BASE, (void *)0, (u32)limit,
	    (void *)entry);
	return(0);
}

i32
userboot(char *path)
{
	u8 hdr[512];
	volatile u8 *mmio = EPOCH68_MMIO_BASE;
	char *dst;
	u32 text;
	u32 data;
	u32 bss;
	u32 entry;
	u32 limit;
	u32 remain;
	u32 raw_text;
	i32 fd;
	i32 n;

	v7io_close_all();
	fd = open(path, 0);
	if (fd < 0)
		return(-1);
	n = read(fd, (char *)hdr, sizeof(hdr));
	if (n < AOUT_HDR_SIZE) {
		uart_puts("user: short header\n");
		close(fd);
		return(-1);
	}
	if (getmid(hdr) != AOUT_MID_SUN010 || getmagic(hdr) != AOUT_ZMAGIC) {
		uart_puts("user: bad image\n");
		close(fd);
		return(-1);
	}

	raw_text = get32(&hdr[4]);
	data = get32(&hdr[8]);
	bss = get32(&hdr[12]);
	entry = get32(&hdr[20]);
	if (raw_text < AOUT_HDR_SIZE || data > USER_IMAGE_LIMIT ||
	    bss > USER_IMAGE_LIMIT - data) {
		uart_puts("user: image too large\n");
		close(fd);
		return(-1);
	}
	text = raw_text - AOUT_HDR_SIZE;
	if (text > USER_IMAGE_LIMIT - data - bss) {
		uart_puts("user: image too large\n");
		close(fd);
		return(-1);
	}
	limit = text + data + bss;
	if (entry >= limit || get32(&hdr[16]) != 0 ||
	    get32(&hdr[24]) != 0 || get32(&hdr[28]) != 0) {
		uart_puts("user: bad header\n");
		close(fd);
		return(-1);
	}

	mmio[EPOCH68_MIRROR1_PAGE_REG] = 1;
	dst = (char *)USER_LOAD_BASE;
	remain = text + data;
	if (n > AOUT_HDR_SIZE) {
		i32 first;

		first = n - AOUT_HDR_SIZE;
		if ((u32)first > remain)
			first = (i32)remain;
		boot_memcpy(dst, hdr + AOUT_HDR_SIZE, (u32)first);
		dst += first;
		remain -= first;
	}
	while (remain > 0) {
		n = read(fd, (char *)hdr, sizeof(hdr));
		if (n <= 0) {
			uart_puts("user: short image\n");
			close(fd);
			return(-1);
		}
		if ((u32)n > remain)
			n = (i32)remain;
		boot_memcpy(dst, hdr, (u32)n);
		dst += n;
		remain -= n;
	}
	close(fd);
	boot_memset(dst, 0, (u32)bss);

	mmio[EPOCH68_SYSTEM_PAGE_REG] = 1;
	mmio[EPOCH68_MIRROR1_PAGE_REG] = 1;
	enter_user((void *)entry, USER_STACK_TOP, 0);
	return(0);
}

static u32
get32(u8 *p)
{
	return(((u32)p[0] << 24) |
	    ((u32)p[1] << 16) |
	    ((u32)p[2] << 8) |
	    (u32)p[3]);
}

static i32
getmid(u8 *p)
{
	return((get32(p) >> 16) & 0x3ff);
}

static char *
boot_memset(char *dst, i32 value, u32 count)
{
	u8 *p;
	u32 i;

	p = (u8 *)dst;
	for (i = 0; i < count; i++)
		p[i] = (u8)value;
	return(dst);
}

static char *
boot_memcpy(char *dst, u8 *src, u32 count)
{
	u8 *d;
	u32 i;

	d = (u8 *)dst;
	for (i = 0; i < count; i++)
		d[i] = src[i];
	return(dst);
}

static i32
getmagic(u8 *p)
{
	return(get32(p) & 0xffff);
}
