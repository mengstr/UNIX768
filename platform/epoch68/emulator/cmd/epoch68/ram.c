#include "ram.h"

#include <string.h>

static uint8_t epoch68_ram[epoch68_ram_size];

uint8_t epoch68_ram_page(uint32_t page) {
	return (uint8_t)(page % epoch68_ram_pages);
}

static uint32_t epoch68_ram_offset(uint8_t page, uint32_t offset) {
	return ((uint32_t)epoch68_ram_page(page) * epoch68_ram_window_size) +
		(offset & (epoch68_ram_window_size - 1));
}

void epoch68_ram_clear(void) {
	memset(epoch68_ram, 0, sizeof(epoch68_ram));
}

uint8_t epoch68_ram_read8(uint8_t page, uint32_t offset) {
	return epoch68_ram[epoch68_ram_offset(page, offset)];
}

void epoch68_ram_write8(uint8_t page, uint32_t offset, uint8_t value) {
	epoch68_ram[epoch68_ram_offset(page, offset)] = value;
}
