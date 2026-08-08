#ifndef EPOCH68_RAM_H
#define EPOCH68_RAM_H

#include <stdint.h>

enum {
	epoch68_ram_window_size = 256 * 1024,
	epoch68_ram_size = 32 * 1024 * 1024,
	epoch68_ram_pages = epoch68_ram_size / epoch68_ram_window_size
};

void epoch68_ram_clear(void);
uint8_t epoch68_ram_read8(uint8_t page, uint32_t offset);
void epoch68_ram_write8(uint8_t page, uint32_t offset, uint8_t value);
uint8_t epoch68_ram_page(uint32_t page);

#endif
