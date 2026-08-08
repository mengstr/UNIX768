#ifndef EPOCH68_DISK_H
#define EPOCH68_DISK_H

#include <sys/inttypes.h>

#define EPOCH68_DISK_SECTOR_SIZE 512UL
#define EPOCH68_DISK_TOTAL_SECTORS 32768UL
#define EPOCH68_DISK_TOTAL_SIZE (EPOCH68_DISK_SECTOR_SIZE * EPOCH68_DISK_TOTAL_SECTORS)

i32 epoch68_disk_read(u32 lba, void *buffer, u16 count);
i32 epoch68_disk_write(u32 lba, const void *buffer, u16 count);

#endif
