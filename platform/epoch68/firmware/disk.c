#include <sys/inttypes.h>
#include "disk.h"
#include "uart.h"

#define EPOCH68_MMIO_BASE ((volatile unsigned char *)0x00a00000)
#define EPOCH68_DISK_WINDOW ((volatile unsigned char *)0x00600000)
#define EPOCH68_DISK_PAGE_REG 3
#define EPOCH68_DISK_PAGE_SIZE (256UL * 1024UL)

static i32
epoch68_disk_transfer(u32 lba, void *buffer, u16 count, i32 is_write)
{
    u32 offset = lba * EPOCH68_DISK_SECTOR_SIZE;
    u32 total = (u32)count * EPOCH68_DISK_SECTOR_SIZE;
    u32 done = 0;
    volatile u8 *mmio = EPOCH68_MMIO_BASE;
    volatile u8 *window = EPOCH68_DISK_WINDOW;
    u8 *dst = (u8 *)buffer;
    const u8 *src = (const u8 *)buffer;
    u32 i;

    while (done < total) {
        u32 absolute = offset + done;
        u32 page = absolute / EPOCH68_DISK_PAGE_SIZE;
        u32 page_offset = absolute % EPOCH68_DISK_PAGE_SIZE;
        u32 chunk = EPOCH68_DISK_PAGE_SIZE - page_offset;

        if (chunk > (total - done)) {
            chunk = total - done;
        }

        mmio[EPOCH68_DISK_PAGE_REG] = (u8)page;

        if (is_write) {
            for (i = 0; i < chunk; i++) {
                window[page_offset + i] = src[done + i];
            }
        } else {
            for (i = 0; i < chunk; i++) {
                dst[done + i] = window[page_offset + i];
            }
        }

        done += chunk;
    }

    return 0;
}

i32
epoch68_disk_read(u32 lba, void *buffer, u16 count)
{
    return epoch68_disk_transfer(lba, buffer, count, 0);
}

i32
epoch68_disk_write(u32 lba, const void *buffer, u16 count)
{
    return epoch68_disk_transfer(lba, (void *)buffer, count, 1);
}
